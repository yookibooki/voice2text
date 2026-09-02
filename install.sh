#!/bin/sh
umask 077
D="$(cd "$(dirname "$0")" 2>/dev/null && pwd || pwd)"
if [ "$(uname)" = "Darwin" ]; then
T="$HOME/bin/voice2text"
S="voice2text-macos.sh"
mkdir -p ~/bin
grep -q 'HOME/bin' ~/.zshrc 2>/dev/null || echo 'export PATH="$HOME/bin:$PATH"' >> ~/.zshrc
if [ ! -f "$D/$S" ]; then curl -fsSL "https://raw.githubusercontent.com/yookibooki/voice2text/main/$S" -o "$T" && chmod +x "$T"; else ln -sf "$D/$S" "$T"; fi
if [ -z "$GROQ_API_KEY" ]; then printf 'GROQ_API_KEY: '; read -r K; Q=$(printf %s "$K" | sed "s/'/'\\\\''/g"); echo "export GROQ_API_KEY='$Q'" >> ~/.zshrc; chmod 600 ~/.zshrc 2>/dev/null; fi
else
T="$HOME/.local/bin/voice2text"
S="voice2text.sh"
mkdir -p ~/.local/bin
grep -q '.local/bin' ~/.profile 2>/dev/null || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.profile
grep -q '.local/bin' ~/.bashrc 2>/dev/null || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
if [ ! -f "$D/$S" ]; then curl -fsSL "https://raw.githubusercontent.com/yookibooki/voice2text/main/$S" -o "$T" && chmod +x "$T"; else ln -sf "$D/$S" "$T"; fi
if [ -z "$GROQ_API_KEY" ]; then printf 'GROQ_API_KEY: '; read -r K; Q=$(printf %s "$K" | sed "s/'/'\\\\''/g"); echo "export GROQ_API_KEY='$Q'" >> ~/.bashrc; chmod 600 ~/.bashrc 2>/dev/null; fi
fi
