#define MAX_BUFFER 1024                        // max line buffer
#define MAX_ARGS 64                            // max # args
#define SEPARATORS " \t\n"                     // token separators


// function declarations
int is_io(char *args[], int length);

//internal functions
// input is the same in order to use a function pointer later on
void clr(char *args[], int numargs);
void dir(char *args[], int numargs);
void cd(char *args[], int numargs);
void printenv(char *args[], int numargs);
void echo(char *args[], int numargs);
void poz(char *args[], int numargs); // pause
void help(char *original_dir, char *args[], int numargs);

char **tokenize(char *str);
int check_bgd(char *args[]);
void execute(char *args[], int numargs, void (*pFunction)(char**, int));

/* I, Przemyslaw Majda, student number 20505049, acknowledge
the DCU Academic Integrity Policy and that all work is my own */