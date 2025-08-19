# Pyro is not meant to be used yet.
Pyro is only supported on Linux x86_64 and Linux ARM64, since @rocklake got fed up with Windows assembly and MacOS assembly is weird.\
Here are the instructions (like functions in Python) if you want to try to make a program in Pyro.\
This is not currently a complete list, we will probably update it.

end:{exit_code}
* Ends the program. end:0 causes an EXIT_SUCCESS, and should be used at the end of all of your Pyro programs.

syscall:
* The language automatically calls the kernel when needed, but you can call it manually with this.

<!-- ^TODO: Add how to add the code to syscall since there isn't a way shown -->

raw_log:"{text}", 10
* Prints text to the console.

back:{lines to go back}
* Goes back lines to rerun code
* For some reason it is insanely hard to predict memory locations, so this often leads to a segmentation fault more often than not. You will have to manually change the asm to make this work, sorry.

<!-- TODO: Add instructions to tell people how to compile the programs they make -->
