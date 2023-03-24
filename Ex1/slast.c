#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <utmp.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define SHOWHOST


void show_info(struct utmp *utbufp);
void showtime(long);

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        perror("the format: last X");
        exit(1);
    }
        
    struct utmp current_record;
    int utmpfd;
    int reclen = sizeof(current_record);
    int num = atoi(argv[1]);
    int count = 0;
    //printf("%d\n", num);
    if((utmpfd = open(WTMP_FILE, O_RDONLY)) == -1)
    {
        perror(WTMP_FILE);
        exit(1);
    }

    //printf("%d\n", utmpfd);
    //printf("%d" , argc);
    //printf("%s" , argv[1]);
    lseek(utmpfd, -reclen, SEEK_END);

    while((read(utmpfd, &current_record, reclen)) == reclen && count<num){

        int offset=lseek(utmpfd,-(reclen)*2,SEEK_CUR);
        struct utmp *utbufp = &current_record;
        if(utbufp->ut_type==BOOT_TIME||utbufp->ut_type==USER_PROCESS){
            //print_utmp(p);
            show_info(&current_record);
            count++;
        }

        if(offset == -1){
       	  break;
	    }
    }
    

    close(utmpfd);


    return 0;

    
}

void show_info(struct utmp *utbufp)
{
    printf("%-8.8s", utbufp->ut_name);
    printf(" ");
    char* line = utbufp->ut_line;
    char* tilda = "~";
    if(strcmp(line, tilda) == 0)
    {
        printf("system boot");
    }
    else
    {
        printf("%-8.8s", utbufp->ut_line);
    }
    
    printf(" ");
    #ifdef SHOWHOST
        printf("%16.16s", utbufp->ut_host);
    #endif
printf(" ");
        showtime(utbufp->ut_time);
    //printf("%10d", utbufp->ut->time);
    
printf(" ");

 
    //printf("%s", utbufp->)
    printf("\n");
}

void showtime(long timeval)
{
    char *cp;
    cp = ctime(&timeval);

    printf("%12.16s", cp);
}

