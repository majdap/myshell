NAME
	Myshell

DESCRIPTION
	Myshell is a custom linux shell written in C for a university project. It executes commands read from the standard input or from a file. Output is sent to standard output or to a specified file. Myshell can also accept commands from a batch file provided as the second argument when executing the program. eg. ./myshell command.bat

ARGUMENTS
	The first argument entered will be treated as the name of a file containing shell commands. The rest of the arguments will be treated as options/commands for the file. Myshell will read and execute the file, then waits for more input. If the "quit" command is called, myshell will exit with a status of 0.[1]

RESERVED CHARACTERS
	"<", ">", and ">>" are characters reserved for input/output redirection in myshell. The "~" character will point to the home directory when passed in as an argument, eg. "cd ~" will change current directory to /home/<<user>>.

I/O REDIRECTION
	"<" redirects input from stdin to the file specified after the symbol, eg "< input.txt". Myshell will read the file, tokenize the input and pass it in as commands. ">" will store the output in a specified file instead of printing it to stdout. The file will be created if it does not exist, and it will be overwritten if it does. ">>" will append the output to the end of a file if the file exists, and will create the file and store output there if it does not exist. eg. "dir > output.txt".[2]

INBUILT FUNCTIONS
	cd - change the current directory. If the argument is not present, report the current directory. If the directory does not exist, an error is reported. This command also changed the "PWD" environment variable.
	clr - clears the terminal.
	dir - lists the contents of a directory eg. ls -al <<directory>>
	environ - lists all environment strings
	echo - echoes all of the arguments followed by a newline
	help - display the user manual
	pause - pause operation of the shell until "Enter" is pressed
	quit - quit the shell

EXTERNAL FUNCTIONS
	Myshell will try to execute any command passed to it (that is not an inbuilt) by looking through the PATH directories and looking for a file with the same name to execute. If no file is found, an exec error will be reported. Myshell uses the execvp()[3] function that takes in 

BATCH MODE
	Myshell can take its command line input from a file, ie. if the shell is invoked with a command line argument: "./myshell batchfile.bat", then batchfile is assumed to contain a set of command lines for the shell to process. When the end-of-file is reached, the shell exits. If the shell is invoked without a command line argument, it solicits input from the user via a prompt on the display.[4]

BACKGROUND EXECUTION
	If the final argument is the "&" symbol, the process will execute in the background, returning to the command line immediately while the process runs in the background.[5]

REFERENCES
	[1]. Richard Stallman et al,GNU Manual, Last updated August 2nd, 2021
	Available: https://www.gnu.org/software/libc/manual/html_node/Exit-Status.html
	[2]. Yashavant Kanetkar, "Let Us C", Infinity Science Press, 2008
	[3]. Andries Brouwer, Michael Kerrisk, Last updated August 2021
	Available: https://man7.org/linux/man-pages/man3/exec.3.html
	[4]. Randal K. Michael, "Mastering Unix Shell Scripting", John Wiley & Sons, 2011
	[5]. Richard Stallman et al, GNU Manual, Last Updated August 2nd, 2021
	Available: https://www.gnu.org/software/libc/manual/html_node/Foreground-and-Background.html




I, Przemyslaw Majda, student number 20505049, acknowledge the DCU Academic Integrity Policy and that all work is my own.