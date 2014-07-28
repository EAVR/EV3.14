# ~/.bashrc: executed by bash(1) for non-login shells.
export PS1='\h:\w\$ '
umask 022
# You may uncomment the following lines if you want `ls' to be colorized:
export LS_OPTIONS='--color=auto'
# eval `dircolors`
alias ls='ls $LS_OPTIONS'
alias ll='ls $LS_OPTIONS -l'
alias l='ls $LS_OPTIONS -lA'

# Some more alias to avoid making mistakes:
alias rm='rm -i'
alias cp='cp -i'
alias mv='mv -i'
# shell options
# autocorrect cd commands
#shopt -s cdspell
# disable Ctrl-D to logout
#set -o ignoreeof
# extended pattern matching
#shopt -s extglob

alias ins_pwm="insmod ~/lms2012/sys/mod/d_pwm.ko"
alias rm_pwm="rmmod d_pwm"
alias ins_analog="insmod ~/lms2012/sys/mod/d_analog.ko"
alias rm_analog="rmmod d_analog"
