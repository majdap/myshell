#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "myshell.h"

#define MAX_BUFFER 1024                        // max line buffer
#define MAX_ARGS 64                            // max # args
#define SEPARATORS " \t\n"                     // token separators

extern char **environ;

int main (int argc, char ** argv)
{
    char cd_buf[200];
    char buf[MAX_BUFFER];                      // line buffer
    char * args[MAX_ARGS];                     // pointers to arg strings
    char ** arg;                               // working pointer thru args
    char * prompt = ":> ";                    // shell prompt
    char *shell;
    char *current;
    char *current_dir;
    char *target_dir;
    char original_dir[200];
    FILE *bat;
    FILE *input;

    shell = getcwd(cd_buf, 200);
    strcpy(original_dir, shell);
    strcat(shell, "/myshell");
    setenv("SHELL", shell, 1);

    if (argv[1]) // if batch file provided as input
    {
        if (access(argv[1], F_OK) == 0)
        {
            bat = freopen(argv[1], "r", stdin);
        }
        else
        {
            fprintf(stderr, "Invalid input file %s\n", argv[1]);
            exit(1);
        }
    }

    /* keep reading input until "quit" command or eof of redirected input */
    while (!feof(stdin))
    {
        
        /* get command line from input */
        fputs (prompt, stdout); // write prompt
        if (fgets(buf, MAX_BUFFER, stdin))
        { // read a line
            /* tokenize the input into args array */
            arg = args;
            *arg++ = strtok(buf,SEPARATORS);   // tokenise input

            while ((*arg++ = strtok(NULL,SEPARATORS)));

            // last entry will be NULL 

            if (args[0])
            {                     // if there's anything there
                /* check for internal/external command */

                // check for background execution
                int background = check_bgd(args);
                void (*pFunction)(char**, int);

                arg = args;
                int numargs = 0; // this will be used to iterate through args
                while (*arg) // this loop counts how many args we have
                { // it also replaces any arg "~" with the HOME environment variable
                    numargs++;
                    if (!strcmp(*arg, "~"))
                    {
                        *arg = getenv("HOME");
                    }
                    *arg++;
                }

                if (!strcmp(args[0], "clr"))
                {
                    pFunction = clr;
                    execute(args, numargs, background, *pFunction);
                    continue;
                }
                
                if (!strcmp(args[0], "cd"))
                {
                    cd(args, numargs);
                    continue;
                }

                if (!strcmp(args[0], "dir"))
                {
                    pFunction = dir;
                    execute(args, numargs, background, *pFunction);
                    continue;
                }

                if (!strcmp(args[0], "environ"))
                {
                    pFunction = printenv;
                    execute(args, numargs, background, *pFunction);
                    continue;
                }

                if (!strcmp(args[0], "echo"))
                {
                    pFunction = echo;
                    execute(args, numargs, background, *pFunction);
                    continue;
                }

                if (!strcmp(args[0], "pause"))
                {
                    pFunction = poz;
                    execute(args, numargs, background, *pFunction);
                    continue;
                }

                if (!strcmp(args[0], "help"))
                {
                    help(original_dir, args, numargs);
                    continue;
                }


                if (!strcmp(args[0],"quit"))   // "quit" command
                {
                    exit(0);                     // break out of 'while' loop
                }

                // if not inbuilt command:
                else
                {
                    if(args[0])
                    {
                        int pid = fork();
                        if (pid == 0)
                        {
                            int err = is_io(args, numargs);
                            execvp(args[0], args);
                            perror("exec");
                            exit(1);
                        }
                        else{
                            wait(NULL);
                        }
                    }
                    continue;
                    
                }
                arg = args;
                while (*arg)
                {
                    *arg++;
                }

                
                continue;
            }
        }
    } // end of while loop
    if (argv[1])
    {
        fclose(bat);
    }
    return 0;
}

/* I, Przemyslaw Majda, student number 20505049, acknowledge
the DCU Academic Integrity Policy and that all work is my own */
