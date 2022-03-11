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

extern char **environ; // the environment variable array

// need to standardise function input before doing this
void execute(char *args[], int numargs, int background, void (*pFunction)(char**, int))
{
    int pid = fork();
    if (pid == 0) // child processing
    {
        int err = is_io(args, numargs); // checking for input/output redirection
        if (!err)
        {   
            if (background) // checking for background execution
            {
                setpgid(0, 0);
            }
            pFunction(args, numargs);
            exit(0);
        }
    }
    if (pid == -1)
    {
        perror("fork failed");
        exit(1);
    }
    else // parent processing
    {
        if (!background)
        {
            wait(NULL);
        }
        
    }
}


int is_io(char* args[], int numargs)
{

    for (int i = 0; i < numargs; ++i)
    {
        if (!strcmp(args[i], "<")) // checking for file input
        {
            args[i] = NULL; // remove "<" from args so it does not get passed as argument to function later
            i++;
            if (args[i] && access(args[i], F_OK & R_OK) == 0) // if there is an argument after <, and that file exists
            {
                int in;
                if(!access(args[i], F_OK) == 0) // file does not exist
                {
                    fprintf(stderr, "error opening file\n");
                    return 1;
                }
                else
                {
                    char argbuf[200];
                    char **new_args;
                    //dup2(in, STDIN_FILENO);
                    FILE * fp = freopen(args[i], "r", stdin);
                    if (fgets(argbuf, 200, fp))
                    {
                        new_args = tokenize(argbuf); // tokenising input from file
                        args[i] = NULL;
                        char **arg = new_args;
                        i--; // go back to first NULL element
                        int j = 0;
                        while (*arg != NULL)
                        {
                            args[i] = new_args[j];
                            i++;
                            j++;
                            *arg++;
                        }
                        continue;
                    }
                    else
                    {
                        fprintf(stderr, "error opening file\n");
                        return 1;
                    }
                    
                }
                
            }
            else
            {
                return 1;
            }
        }

        if (!strcmp(args[i], ">")) // checking for output redirection
        {
            args[i] = NULL;
            i++;
            if (args[i])
            {
                int out;
                out = creat(args[i], 0644);
                dup2(out, STDOUT_FILENO); // duplicate the stdout to new "out" file
                args[i] = NULL;
                continue;
            }
            else
            {
                return 1;
            }
        }

        if (!strcmp(args[i], ">>")) // checking for output redirection with append
        {
            args[i] = NULL;
            i++;
            if (args[i]) // filename provided
            {
                int add = open(args[i], O_CREAT | O_RDWR | O_APPEND, 0644);
                dup2(add, STDOUT_FILENO);
                continue;
            }
        }
    }
    
    return 0;
}

void clr(char *args[], int numargs)
{
    system("clear");
}

void dir(char *args[], int numargs)
{
    if(args[1]) // if there is a path specified
    {
        char *s1[] = {"ls", "-al", args[1], NULL};
        execvp("ls", s1);
        perror("exec");
    }

    else { // child processing, no path specified
        char *s1[] = {"ls", "-al", NULL};
        execvp("ls", s1);
        perror("exec");
    }
}

void cd(char *args[], int numargs)
{
    char *current;
    char *current_dir;
    char *target_dir;
    char cd_buf[200];
    int err = is_io(args, numargs); // checking for I/O redirection
    if (args[1]) // path specified
    {
        char *first = &args[1][0];
        if (strcmp(first, "/")) //specified path is absolute
        {
            target_dir = args[1];
        }
        else
        { // if path not absolute, generate string from current path + provided path
            current = getcwd(cd_buf, 200); 
            current_dir = strcat(current, "/");
            target_dir = strcat(current, args[1]);
        }
        

        if (chdir(target_dir) != 0) // chdir failed
        {
            perror("chdir failed");
        }
        else
        {
            
            if (!err)
            {
                current = getcwd(cd_buf, 200);
                setenv("PWD", current, 1);
                printf("%s\n", current);
            }
            
        }
    }
    else // no dir specified
    {
        if (!err)
        {
            current = getcwd(cd_buf, 200);
            setenv("PWD", current, 1);
            printf("%s\n", current);
        }
    }
}

void printenv(char *args[], int numargs)
{
    for (int i = 0; environ[i] != NULL; ++i)
    {
        printf("%s\n", environ[i]); // print every environment variable
    }
}

void echo(char *args[], int numargs)
{
    if (args[1]) // more than 1 arg
    {
        char **arg = args;
        *arg++; // start at the second arg, dont echo "echo"
        while(*arg)
        {
            printf("%s ", *arg);
            *arg++;
        }
        printf("\n");
    }
    else
    {
        printf("echo requires more arguments\n");
    }
}

void poz(char *args[], int numargs)
{
    while(1)
    {
        char c = getchar(); // waiting for user to input newline character
        if (c == 10)
        {
            break;
        }
    }
}

void help(char *original_dir, char *args[], int numargs)
{
    char mandir[] = "../manual/";
    int pid = fork();
    if (pid == 0) // child
    { // this chunk of code ensures that the help function will execute no matter which directory we are currently in
        if (chdir(original_dir) != 0) //chdir failed
        {
            fprintf(stderr, "An error has occured switching directory\n");
            exit(1);
        }
        else
        {
            if (chdir(mandir) != 0) // chdir failed
            {
                fprintf(stderr, "An error has occured switching directory\n");
                exit(1);
            }
            else{
                execlp("more", "more", "readme.md", (char*)NULL);
            }
            
        }
    }
    else
    {
        wait(NULL);
    }
    
}

char **tokenize(char str[])
{
    char strbuf[200];
    strcpy(strbuf, str);
    char **tokenbuf = malloc(sizeof(char*) * 20); // allocating memory for string array
    char *token;
    const char s[] = " \n\t";
    int i = 0;
    // first token
    token = strtok(strbuf, SEPARATORS);
    while(token != NULL) // tokenizing string
    {
        tokenbuf[i] = token;
        token = strtok(NULL, s);
        i++;
    }
    return tokenbuf;
}

int check_bgd(char *args[]) // checking for "&" symbol, returns 1 if present, else 0
{
    char **argcycle = args;
    while (*argcycle != NULL)
    {
        *argcycle++;
    }
    *argcycle--;
    if (!strcmp(*argcycle, "&"))
    {
        *argcycle = NULL;
        return 1;
    }
    return 0;
}



/* I, Przemyslaw Majda, student number 20505049, acknowledge
the DCU Academic Integrity Policy and that all work is my own */