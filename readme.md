pyro at its current stage it is not meant to be used yet though
here are the instructions

end:{exit_code}\
so end:0 for EXIT_SUCCESS

syscall:\
the language automatically calls the colonel when needed but you can call it manually with this

raw_log:"{text}", 10\
print text to the console

back:{lines to go back}\
goes back lines\
for some reason it is insanely hard to predict memory locations, so this often leads to a segmentation fault more often than not you will have to manually change the asm to make this work sorry