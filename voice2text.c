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

#define LOCK_FILE       "/dev/shm/v2t.lock"
#define AUDIO_FILE      "/dev/shm/v2t.wav"
#define GROQ_URL        "https://api.groq.com/openai/v1/audio/transcriptions"
#define MODEL_NAME      "whisper-large-v3-turbo"
#define SAMPLE_RATE     16000
#define BUFFER_FRAMES   512
#define BYTES_PER_FRAME 2   /* S16_LE mono */

#pragma pack(push, 1)
typedef struct {
    char    riff[4];
    int32_t file_size;
    char    wave[4];
    char    fmt_id[4];
    int32_t fmt_size;
    int16_t format;         /* 1 = PCM */
    int16_t channels;
    int32_t sample_rate;
    int32_t byte_rate;
    int16_t block_align;
    int16_t bits_per_sample;
    char    data_id[4];
    int32_t data_size;
} WavHeader;
#pragma pack(pop)

typedef struct { char *data; size_t size; } ResponseBuf;

/* ---------- libcurl helpers ---------- */

static size_t write_callback(void *src, size_t size, size_t nmemb, void *userp) {
    size_t n = size * nmemb;
    ResponseBuf *buf = userp;
    char *p = realloc(buf->data, buf->size + n + 1);
    if (!p) return 0;
    buf->data = p;
    memcpy(buf->data + buf->size, src, n);
    buf->size += n;
    buf->data[buf->size] = '\0';
    return n;
}

/* ---------- process helpers ---------- */

static int exec_tool(const char *const args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        int devnull = open("/dev/null", O_RDWR);
        if (devnull >= 0) {
            dup2(devnull, STDIN_FILENO);
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }
        execvp(args[0], (char *const *)args);
        _exit(127);
    }
    int status;
    waitpid(pid, &status, 0);
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : -1;
}

static void pipe_to_xclip(const char *text) {
    int pipefd[2];
    if (pipe(pipefd) == -1) return;
    pid_t pid = fork();
    if (pid == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        const char *args[] = {"xclip", "-selection", "clipboard", NULL};
        execvp(args[0], (char *const *)args);
        _exit(127);
    }
    close(pipefd[0]);
    (void)write(pipefd[1], text, strlen(text));
    close(pipefd[1]);
    waitpid(pid, NULL, 0);
}

/*
 * Release any held modifier keys, then send Ctrl+Shift+V.
 * Releasing Alt/Shift/Space first ensures the paste lands correctly
 * in modifier-heavy keybinding setups.
 */
static void send_paste_macro(void) {
    const char *args[] = {"xdotool", "keyup", "alt", "shift", "space",
                          "key", "ctrl+shift+v", NULL};
    exec_tool(args);
}

/* ---------- transcription helpers ---------- */

/* Extract the "text" field from a Groq JSON response and paste it. */
static void parse_and_paste(const char *json) {
    const char *p = strstr(json, "\"text\":");
    if (!p) return;
    p += 7;
    while (*p == ' ' || *p == '"') p++;

    char *out = malloc(strlen(p) + 1);
    if (!out) return;

    size_t i = 0;
    while (*p && *p != '"') {
        if (*p == '\\' && *(p + 1) == 'n') { out[i++] = '\n'; p += 2; }
        else out[i++] = *p++;
    }
    out[i] = '\0';

    pipe_to_xclip(out);
    send_paste_macro();
    free(out);
}

/* Read the Groq API key from ~/.config/voice2text/groq.key. */
static void read_api_key(char *buf, size_t buflen) {
    const char *home = getenv("HOME");
    if (!home || !buflen) { if (buflen) buf[0] = '\0'; return; }

    char path[512];
    snprintf(path, sizeof(path), "%s/.config/voice2text/groq.key", home);
    FILE *f = fopen(path, "r");
    if (!f) { buf[0] = '\0'; return; }
    if (!fgets(buf, buflen, f)) buf[0] = '\0';
    fclose(f);

    buf[strcspn(buf, "\r\n")] = '\0';
}

/* POST AUDIO_FILE to the Groq Whisper API and paste the transcription. */
static void transcribe_and_paste(const char *api_key) {
    CURL *curl = curl_easy_init();
    if (!curl) return;

    ResponseBuf chunk = { .data = malloc(1), .size = 0 };

    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    struct curl_slist *headers = curl_slist_append(NULL, auth_header);

    curl_mime *mime = curl_mime_init(curl);

    curl_mimepart *part = curl_mime_addpart(mime);
    curl_mime_name(part, "file");
    curl_mime_filedata(part, AUDIO_FILE);
    curl_mime_type(part, "audio/wav");

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "model");
    curl_mime_data(part, MODEL_NAME, CURL_ZERO_TERMINATED);

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "language");
    curl_mime_data(part, "en", CURL_ZERO_TERMINATED);

    curl_easy_setopt(curl, CURLOPT_URL,           GROQ_URL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,    headers);
    curl_easy_setopt(curl, CURLOPT_MIMEPOST,      mime);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,     &chunk);

    if (curl_easy_perform(curl) == CURLE_OK)
        parse_and_paste(chunk.data);

    curl_mime_free(mime);
    curl_slist_free_all(headers);
    free(chunk.data);
    curl_easy_cleanup(curl);
}

/* ---------- recording session ---------- */

/* Populate a WavHeader for 16-bit mono PCM once total data size is known. */
static void fill_wav_header(WavHeader *h, int32_t data_size) {
    memcpy(h->riff,    "RIFF", 4);
    h->file_size     = 36 + data_size;
    memcpy(h->wave,    "WAVE", 4);
    memcpy(h->fmt_id,  "fmt ", 4);
    h->fmt_size      = 16;
    h->format        = 1;   /* PCM */
    h->channels      = 1;
    h->sample_rate   = SAMPLE_RATE;
    h->byte_rate     = SAMPLE_RATE * BYTES_PER_FRAME;
    h->block_align   = BYTES_PER_FRAME;
    h->bits_per_sample = 16;
    memcpy(h->data_id, "data", 4);
    h->data_size     = data_size;
}

/* Open the default ALSA capture device for 16-bit mono at SAMPLE_RATE. */
static snd_pcm_t *open_capture_device(void) {
    snd_pcm_t *pcm;
    if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_CAPTURE, 0) < 0) return NULL;

    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm, params);
    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, params, 1);
    unsigned int rate = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(pcm, params, &rate, NULL);
    snd_pcm_hw_params(pcm, params);
    snd_pcm_prepare(pcm);
    return pcm;
}

/* Returns 1 if the lock file signals stop ('S') or has been removed. */
static int stop_requested(void) {
    int fd = open(LOCK_FILE, O_RDONLY);
    if (fd < 0) return 1;
    char ch = 0;
    (void)read(fd, &ch, 1);
    close(fd);
    return ch == 'S';
}

static void run_recording_session(void) {
    snd_pcm_t *pcm = open_capture_device();
    if (!pcm) return;

    FILE *wav = fopen(AUDIO_FILE, "wb");
    if (!wav) { snd_pcm_close(pcm); return; }

    WavHeader header = {0};
    fwrite(&header, sizeof(header), 1, wav);   /* placeholder; rewritten after loop */

    int16_t buf[BUFFER_FRAMES];
    int32_t total_bytes = 0;

    while (!stop_requested()) {
        snd_pcm_sframes_t frames = snd_pcm_readi(pcm, buf, BUFFER_FRAMES);
        if (frames < 0) frames = snd_pcm_recover(pcm, frames, 0);
        if (frames > 0) {
            fwrite(buf, 1, frames * BYTES_PER_FRAME, wav);
            total_bytes += frames * BYTES_PER_FRAME;
        }
    }

    fill_wav_header(&header, total_bytes);
    rewind(wav);
    fwrite(&header, sizeof(header), 1, wav);
    fclose(wav);
    snd_pcm_close(pcm);

    char api_key[256] = {0};
    read_api_key(api_key, sizeof(api_key));
    if (api_key[0]) transcribe_and_paste(api_key);

    unlink(AUDIO_FILE);
    unlink(LOCK_FILE);
}

/* ---------- entry point ---------- */

/* Try to create the lock file exclusively. Returns the fd on success, -1 if
   another instance is already running. */
static int try_acquire_lock(void) {
    return open(LOCK_FILE, O_CREAT | O_EXCL | O_WRONLY, 0666);
}

/* Signal the running instance to stop by writing 'S' to the lock file. */
static void signal_stop(void) {
    int fd = open(LOCK_FILE, O_WRONLY);
    if (fd >= 0) {
        (void)write(fd, "S", 1);
        close(fd);
    }
}

int main(void) {
    int fd = try_acquire_lock();
    if (fd < 0) { signal_stop(); return 0; }
    close(fd);
    run_recording_session();
    return 0;
}
