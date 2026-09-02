#!/bin/sh
umask 077
D="$(cd "$(dirname "$0")" && pwd)"
if [ "$(uname)" = "Darwin" ]; then
mkdir -p ~/bin
ln -sf "$D/voice2text-macos.sh" ~/bin/voice2text
grep -q 'HOME/bin' ~/.zshrc 2>/dev/null || echo 'export PATH="$HOME/bin:$PATH"' >> ~/.zshrc
if [ -z "$GROQ_API_KEY" ]; then printf 'GROQ_API_KEY: '; read -r K; Q=$(printf %s "$K" | sed "s/'/'\\\\''/g"); echo "export GROQ_API_KEY='$Q'" >> ~/.zshrc; chmod 600 ~/.zshrc 2>/dev/null; fi
else
mkdir -p ~/.local/bin
ln -sf "$D/voice2text.sh" ~/.local/bin/voice2text
grep -q '.local/bin' ~/.profile 2>/dev/null || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.profile
grep -q '.local/bin' ~/.bashrc 2>/dev/null || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
if [ -z "$GROQ_API_KEY" ]; then printf 'GROQ_API_KEY: '; read -r K; Q=$(printf %s "$K" | sed "s/'/'\\\\''/g"); echo "export GROQ_API_KEY='$Q'" >> ~/.bashrc; chmod 600 ~/.bashrc 2>/dev/null; fi
fi
