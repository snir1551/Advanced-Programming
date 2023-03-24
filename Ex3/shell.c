#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define BUFFER_SIZE1 1024

volatile sig_atomic_t unprocessedSig;
void sig_handler(int signo){
    printf("\nYou typed Control-C!\n");

}



int main() {


struct sigaction act;
act.sa_handler = sig_handler;
sigaction(SIGINT,&act,NULL);


char *x;
char *token;
char *outfile;
int i, fd, amper, redirect, retid, status, argc1, piping;
char *argv[10], **argv1;
char* hello =  "hello: ";
int fildes[2];
char *prompt = malloc(sizeof(hello));
strcpy(prompt, hello);
char command[BUFFER_SIZE], last_command[BUFFER_SIZE];
char *name;
int save_last_cammand=0;


while (1)
{
    if(save_last_cammand == 0){
        printf("%s", prompt);
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';
        if(strcmp(command, "!!")){
            strcpy(last_command, command);
        }
    }
    else{ 
        strcpy(command, last_command);
        save_last_cammand = 0;
    }

    piping = 0;

    /* parse command line */
    i = 0;
    token = strtok (command," ");
    while (token != NULL)
    {
        argv[i] = token;
        token = strtok (NULL, " ");
        i++;
        if (token && ! strcmp(token, "|")) {
            piping = 1;
            break;
        }
    }
    argv[i] = NULL;
    argc1 = i;



    /* Is command empty */
    if (argv[0] == NULL)
        continue;

    /* Does command contain pipe */
    if (piping) {
        i = 0;
        while (token!= NULL)
        {
            token = strtok (NULL, " ");
            argv1[i] = token;
            i++;
        }
        argv1[i] = NULL;
    }

    // for prompt
    if (i == 3 && !strcmp(argv[1], "=" ) &&  !strcmp(argv[0], "prompt" ) ) {
        strcpy(prompt,argv[2]);
        strcat(prompt, ": ");
        continue;
    }
    if(i == 3 && !strcmp(argv[1], "=" ) && argv[0][0] == '$' && strlen(argv[0]) > 1)
    {
        setenv(argv[0],argv[2],1);
        continue;
    }
    if(i == 2 &&  !strcmp(argv[0], "read" ) )
    {
         name=calloc(BUFFER_SIZE1, sizeof(char));
        fgets(name, BUFFER_SIZE1, stdin);

        name[strcspn(name, "\n")] = 0;
        //fgets(buf12, BUFFER_SIZE1, stdin);
        //buf12[strlen(buf12)-1] = '\0';

        //  remove trailing newline
       
        //printf("string is: %s\n", buf);
         char* dest = malloc(strlen(argv[1]));
         dest[0] = '$';
         strcat(dest, argv[1]);
         //printf("string is: %s\n", dest);
         //printf("string is: %s\n", argv);
        setenv(dest,name,1);
        free(dest);
        free(name);
        continue;
    }



    /* Does command line end with & */ 
    if (! strcmp(argv[i - 1], "&")) {
        amper = 1;
        argv[i - 1] = NULL;
    }
    else 
        amper = 0; 

    if (! strcmp(argv[i - 2], ">")) {
        redirect = 1;
        argv[i - 2] = NULL;
        outfile = argv[i - 1];
    }
    else if(! strcmp(argv[i - 2], "2>")) {
        redirect = 2;
        argv[i - 2] = NULL;
        outfile = argv[i - 1];
    }
    else 
        redirect = 0; 



    // for !!
      if (argc1 == 1 && piping == 0 && redirect == 0 && !strcmp(argv[0], "!!") && save_last_cammand == 0) { 
        save_last_cammand = 1;    
        continue;
    }
    // for quit
    if (argc1 == 1 && piping == 0 && redirect == 0 && !strcmp(argv[0], "quit")) { 
        free(prompt);
        exit(0);
        //_Exit(0);
    }
    // for cd
    if (argc1 == 2 && piping == 0 && redirect == 0 && !strcmp(argv[0], "cd")) { 
        chdir(argv[1]);  
        continue;
    }
    //if(argc1 == 2 && piping == 0 && redirect == 0 && !strcmp(argv[0], "echo") && !strcmp(argv[1], "$?")) { 
        //printf("%d\n", status);    
        //continue;
    //}
    if (argc1 > 0 && piping == 0 && redirect == 0 && !strcmp(argv[0], "echo" ) ) { 
        for (size_t i = 1; i < argc1; i++)
        {
            x = getenv(argv[i]);
            if(!strcmp(argv[i], "$?"))
            {
                printf("%d", status);
            }
            else if(x != NULL && argv[i][0] == '$')
            {
               //printf("%ld",strlen(x));
                printf("%s", x);    
            }
            else
                printf("%s", argv[i]);
            if(i != argc1-1){
                printf(" ");
            }
        }
        printf("\n");
        continue;
    }


    /* for commands not part of the shell command language */ 

    if (fork() == 0) { 
        /* redirection of IO ? */
        if (redirect == 1) {
            fd = creat(outfile, 0660); 
            //STDERR_FILENO
            close (STDOUT_FILENO) ; 
            dup(fd); 
            close(fd); 
            /* stdout is now redirected */
        } 
        else if(redirect == 2)
        {
            fd = creat(outfile, 0660); 
            //STDERR_FILENO
            close (STDERR_FILENO) ; 
            dup(fd); 
            close(fd); 
            /* stdout is now redirected */
        }
       

        if (piping) {
            pipe (fildes);
            if (fork() == 0) { 
                /* first component of command line */ 
                close(STDOUT_FILENO); 
                dup(fildes[1]); 
                close(fildes[1]); 
                close(fildes[0]); 
                /* stdout now goes to pipe */ 
                /* child process does command */ 
                execvp(argv1[0], argv1);
            } 
            /* 2nd command component of command line */ 
            close(STDIN_FILENO);
            dup(fildes[0]);
            close(fildes[0]); 
            close(fildes[1]); 
            /* standard input now comes from pipe */ 
            execvp(argv1[0], argv1);
        } 
        else
        {
            execvp(argv[0], argv);
            exit(0);
        }
            
    }
    /* parent continues here */
    if (amper == 0)
        retid = wait(&status);

}
}
