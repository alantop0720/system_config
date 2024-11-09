# .bashrc

# User specific aliases and functions

alias rm='rm -i'
alias cp='cp -i'
alias mv='mv -i'
alias h='history' 
alias home='cd /home'
# alias ls='ls --color=auto'
alias ..='cd ..'
alias l='ls -a -p'
alias a='cd /alantop'
alias ip='ifconfig ens33'
# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

alias home='cd /home'
alias upload='rz'
alias download='sz'
stty erase ^H
stty erase ^?

export PYENV_ROOT="$HOME/.pyenv"
export PATH="$PYENV_ROOT/bin:$PATH"
export PATH="$PYENV_ROOT/shims:$PATH"
eval "$(pyenv init -)"


PS1='\[\e[30;43m\][\u@hcl \w]\e[m\$'

