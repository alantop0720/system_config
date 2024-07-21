# .bashrc

# User specific aliases and functions

alias rm='rm -i'
alias cp='cp -i'
alias mv='mv -i'
alias h='history' 
alias home='cd /home'
# alias ls='ls --color=auto'
alias ..='cd ..'
alias l='ls -all -p'
alias alantop='cd /alantop'
# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

alias home='cd /home'
alias upload='rz'
alias download='sz'

stty erase ^H
stty erase ^?
