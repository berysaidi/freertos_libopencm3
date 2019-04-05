target remote localhost:3333
monitor reset
monitor halt
monitor flash protect 0 0 11 off
monitor arm semihosting enable
load
