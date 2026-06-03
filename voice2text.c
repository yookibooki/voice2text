#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <alsa/asoundlib.h>
#include <curl/curl.h>

#define LOCK_FILE "/dev/shm/v2t.lock"
#define AUDIO_FILE "/dev/shm/v2t.wav"
#define GROQ_URL "https://api.groq.com/openai/v1/audio/transcriptions"
#define MODEL_NAME "whisper-large-v3-turbo"

#pragma pack(push, 1)
typedef struct {
    char riff[4]; int32_t overall_size; char wave[4]; char fmt_chunk_marker[4];
    int32_t length_of_fmt; int16_t format_type; int16_t channels;
    int32_t sample_rate; int32_t byterate; int16_t block_align;
    int16_t bits_per_sample; char data_chunk_header[4]; int32_t data_size;
} WavHeader;
#pragma pack(pop)

struct MemoryStruct { char *memory; size_t size; };

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) return 0;
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

// Ultra-fast fork execution without invoking an expensive system shell shell (/bin/sh)
int exec_tool(char *args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        int devnull = open("/dev/null", O_RDWR);
        dup2(devnull, STDIN_FILENO);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        close(devnull);
        execvp(args[0], args);
        _exit(127);
    }
    int status;
    waitpid(pid, &status, 0);
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : -1;
}

// Native pipe writing to xclip to drastically cut down RAM memory footprints
void pipe_to_xclip(const char *text) {
    int pipefd[2];
    if (pipe(pipefd) == -1) return;
    pid_t pid = fork();
    if (pid == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        char *args[] = {"xclip", "-selection", "clipboard", NULL};
        execvp(args[0], args);
        _exit(127);
    }
    close(pipefd[0]);
    write(pipefd[1], text, strlen(text));
    close(pipefd[1]);
    waitpid(pid, NULL, 0);
}

// Unbinds UI keys explicitly to ensure execution inside modifier-heavy configurations
void send_paste_macro(void) {
    char *args[] = {"xdotool", "keyup", "alt", "shift", "space", "key", "ctrl+shift+v", NULL};
    exec_tool(args);
}

void parse_and_paste(const char *json) {
    const char *p = strstr(json, "\"text\":");
    if (!p) return;
    p += 7;
    while (*p == ' ' || *p == '"') p++;

    char *output = malloc(strlen(p) + 1);
    size_t idx = 0;
    while (*p && *p != '"') {
        if (*p == '\\' && *(p+1) == 'n') { output[idx++] = '\n'; p += 2; }
        else { output[idx++] = *p; p++; }
    }
    output[idx] = '\0';

    pipe_to_xclip(output);
    send_paste_macro();
    free(output);
}

void run_recording_session(void) {
    snd_pcm_t *pcm;
    if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_CAPTURE, 0) < 0) return;

    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm, params);
    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, params, 1);
    unsigned int rate = 16000;
    snd_pcm_hw_params_set_rate_near(pcm, params, &rate, NULL);
    snd_pcm_hw_params(pcm, params);
    snd_pcm_prepare(pcm);

    FILE *f = fopen(AUDIO_FILE, "wb");
    WavHeader header = {0};
    fwrite(&header, sizeof(header), 1, f);

    short buf[512];
    long long total_bytes = 0;

    // Low-overhead polling loop checking for the stop modifier signal inside the lockfile
    while (1) {
        int lfd = open(LOCK_FILE, O_RDONLY);
        if (lfd >= 0) {
            char check = 0;
            read(lfd, &check, 1);
            close(lfd);
            if (check == 'S') break; // Received stop signal
        } else {
            break; // Lockfile removed unexpectedly
        }

        snd_pcm_sframes_t frames = snd_pcm_readi(pcm, buf, 512);
        if (frames < 0) frames = snd_pcm_recover(pcm, frames, 0);
        if (frames > 0) {
            fwrite(buf, 1, frames * 2, f);
            total_bytes += (frames * 2);
        }
    }

    // Wrap up WAV headers natively
    memcpy(header.riff, "RIFF", 4); header.overall_size = 36 + total_bytes;
    memcpy(header.wave, "WAVE", 4); memcpy(header.fmt_chunk_marker, "fmt ", 4);
    header.length_of_fmt = 16; header.format_type = 1; header.channels = 1;
    header.sample_rate = 16000; header.byterate = 32000; header.block_align = 2;
    header.bits_per_sample = 16; memcpy(header.data_chunk_header, "data", 4);
    header.data_size = total_bytes;

    fseek(f, 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, f);
    fclose(f);
    snd_pcm_close(pcm);

    // Call API natively via libcurl
    char api_key[256] = {0};
    char path[512];
    snprintf(path, sizeof(path), "%s/.config/voice2text/groq.key", getenv("HOME"));
    FILE *kf = fopen(path, "r");
    if (kf) { fgets(api_key, sizeof(api_key), kf); fclose(kf); }
    char *nl = strchr(api_key, '\n'); if (nl) *nl = '\0';
    nl = strchr(api_key, '\r'); if (nl) *nl = '\0';

    CURL *curl = curl_easy_init();
    if (curl) {
        struct MemoryStruct chunk = { .memory = malloc(1), .size = 0 };
        char auth[512]; snprintf(auth, sizeof(auth), "Authorization: Bearer %s", api_key);
        struct curl_slist *headers = curl_slist_append(NULL, auth);

        curl_mime *mime = curl_mime_init(curl);
        curl_mimepart *part = curl_mime_addpart(mime);
        curl_mime_name(part, "file"); curl_mime_filedata(part, AUDIO_FILE); curl_mime_type(part, "audio/wav");
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "model"); curl_mime_data(part, MODEL_NAME, CURL_ZERO_TERMINATED);

        curl_easy_setopt(curl, CURLOPT_URL, GROQ_URL);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        if (curl_easy_perform(curl) == CURLE_OK) {
            parse_and_paste(chunk.memory);
        }

        curl_mime_free(mime);
        curl_slist_free_all(headers);
        free(chunk.memory);
        curl_easy_cleanup(curl);
    }
    unlink(AUDIO_FILE);
    unlink(LOCK_FILE);
}

int main(void) {
    int fd = open(LOCK_FILE, O_CREAT | O_EXCL | O_WRONLY, 0666);
    if (fd < 0) {
        // File already exists! Write the stop character to toggle recording off.
        int signalfd = open(LOCK_FILE, O_WRONLY);
        if (signalfd >= 0) {
            write(signalfd, "S", 1);
            close(signalfd);
        }
        return 0;
    }
    close(fd);

    // This is the single active instance doing the heavy lifting
    run_recording_session();
    return 0;
}
