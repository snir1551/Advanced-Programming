#define _XOPEN_SOURCE 600 /* Get nftw() */
#include <ftw.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h> /* Standard I/O functions */
#include <stdlib.h> /* Prototypes of commonly used library functions, plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h> /* Type definitions used by many programs */
#include <unistd.h> /* Prototypes for many system calls */

int counter_dir = 0; // let the num of the directory
int counter_file = 0; // let the num of the files
int counter_total = 0; // let the num total files and directory
int counter_first_dir = 0; // let the num of first directory
char *last_user_name = ""; // let the last user name
char *last_group_name = ""; // let the last group name
long last_file_size = 0; // let the last file size
char last_permissions[9] = {'-', '-', '-', '-', '-', '-', '-', '-', '-'}; // let the last premission
size_t last_level = 0; // let the last level
size_t current_level = 0; // let the current level
char last_type; // let last type
char *last_name; // let last name
mode_t last_mode; // let last mode


char* uid_to_name(uid_t uid); //get the name of the user id

char* gid_to_name(gid_t gid); //get the name of the group id

char get_file_type(mode_t mode); // get file the file type

void mode_to_letters(int mode, char str[]); // get the permissions mode

int counter_filesDir(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb); // counter the first directory and count the total files

int tree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb); // move on the tree



int main(int argc, char const *argv[])
{
    int flag = 0;

    if (argc == 1) // only one argument so we got the default "."
    {
        nftw(".", counter_filesDir, 10, flag);
        nftw(".", tree, 10, flag);
    }
    else if (argc == 2) // two arguemnts
    {
        nftw(argv[1], counter_filesDir, 10, flag);
        nftw(argv[1], tree, 10, flag);
    }
    else // error and let explenation what to do for run that
    {
        fprintf(stderr, "write ./stree  or ./stree 'your directory'\n");
        exit(1);
    }
    char * directories; // print the name of the directory or directories
    char * files; // print the name of the file or files

    if (counter_dir == 1) directories = "directory";
    else directories = "directories";
    if (counter_file == 1) files = "file";
    else files = "files";

    if(counter_total == 1){ // the last print of the tree
        printf("└── [");
        printf("%c", last_type);
        for (size_t i = 0; i < 9; i++)
        {
            printf("%c", last_permissions[i]);
        }
        printf(" %s\t%s %15ld]  %s\n\n", last_user_name, last_group_name, last_file_size, last_name);
    }
    
    printf("%d %s, %d %s\n", counter_dir, directories, counter_file, files);
    
    return 0;
}



char get_file_type(mode_t mode)
{
    char c;

    switch (mode & S_IFMT) {  /* Print file type */
        case S_IFREG: 
            c = '-';
            break;
        case S_IFDIR: 
            c = 'd';
            break;
        case S_IFCHR:  
            c = 'c';
            break;
        case S_IFBLK:
            c = 'b'; 
            break;
        case S_IFLNK: 
            c = 'l'; 
            break;
        case S_IFIFO: 
            c = 'p'; 
            break;
        case S_IFSOCK: 
            c = 's'; 
            break;
        default:    
            printf("?"); 
            break; /* Should never happen (on Linux) */

    }

    
    return c;
}


char* uid_to_name(uid_t uid)
{
    return getpwuid(uid)->pw_name;
}

char* gid_to_name(gid_t gid)
{
    return getgrgid(gid)->gr_name;
}

void mode_to_letters(int mode, char str[])
{
    strcpy(str, "---------");
    

    if(mode & S_IRUSR)
        str[0] = 'r';
    if(mode & S_IWUSR)
        str[1] = 'w';
    if(mode & S_IXUSR)
        str[2] = 'x';
    if(mode & S_IRGRP)
        str[3] = 'r';
    if(mode & S_IWGRP)
        str[4] = 'w';
    if(mode & S_IXGRP)
        str[5] = 'x';
    if(mode & S_IROTH)
        str[6] = 'r';
    if(mode & S_IWOTH)
        str[7] = 'w';
    if(mode & S_IXOTH)
        str[8] = 'x';
}


int counter_filesDir(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    if (type == FTW_NS)
        return 0;

    if (ftwb->level == 0)
        return 0;

    const char *name = &pathname[0];
    if(name[2] == '.')
        return 0;



    if (type == FTW_D || type == FTW_F)
    {
        if (type == FTW_F || (type == FTW_D && strcmp(".", pathname) != 0))
        {
            counter_total++;
        }
        if(ftwb->level == 1){
            counter_first_dir ++;
        } 
        return 0;
    }
    return 0;
}

int tree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    if (type == FTW_NS) // The object is NOT a symbolic link and is one for  which stat() could not be executed
        return 0;

    if (ftwb->level == 0)
    {
        printf("%s\n", pathname);
        return 0;
    }

    const char *name = &pathname[0];
    if(name[2] == '.')
        return 0;


    


    if (type == FTW_D || type == FTW_F)
    {
        current_level = ftwb->level;

        if (last_level != 0 && (counter_file + counter_dir != 0))
        {
            if(counter_first_dir > 1){
                for (size_t i = 0; i < (last_level - 1); i++)
                {
                    printf("│   ");
                }
            }
            else if(counter_first_dir != 0 && last_level > 1){
                    printf("    ");
                    for (size_t i = 0; i < (last_level - 2); i++)
                    {
                        printf("│   ");
                    }
            }
            if (last_level > current_level || (counter_first_dir == 1 && last_level == 1))
            {
                printf("└── [");
                printf("%c", last_type);
            }
            else
            {
                printf("├── [");
                printf("%c", last_type);
            }

            for (size_t i = 0; i < 9; i++)
            {
                printf("%c", last_permissions[i]);
            }
            
            printf(" %s\t%s %15ld]  %s\n", last_user_name, last_group_name, last_file_size, last_name);
            free(last_user_name);
            free(last_group_name);

            if ((counter_dir + counter_file) == counter_total - 1)
            {

                last_mode = sbuf->st_mode;
                mode_to_letters(last_mode,last_permissions);

                last_user_name = getpwuid(sbuf->st_uid)->pw_name;
                last_group_name = getgrgid(sbuf->st_gid)->gr_name;
                last_file_size = sbuf->st_size;
                last_level = current_level;
                last_type = get_file_type(sbuf->st_mode);
                strcpy(last_name, pathname + ftwb->base);

                if(counter_first_dir > 1){
                    for (size_t i = 0; i < (last_level - 1); i++)
                    {
                        printf("│   ");
                    }
                }
                else if(counter_first_dir != 0 && last_level > 1){
                    printf("    ");
                    for (size_t i = 0; i < (last_level - 2); i++)
                    {
                        printf("│   ");
                    }
                }
                printf("└── [%c", last_type);
                for (size_t i = 0; i < 9; i++)
                {
                    printf("%c", last_permissions[i]);
                }
            
                printf(" %s\t%s %15ld]  %s\n\n", last_user_name, last_group_name, last_file_size, last_name);
            }
                
        }

        last_mode = (intptr_t)(malloc(sizeof(sbuf->st_mode)));
        last_mode = sbuf->st_mode;

        *last_permissions = (intptr_t)(malloc(9));
        mode_to_letters(last_mode,last_permissions);

        last_user_name = malloc(sizeof(uid_to_name(sbuf->st_uid)));
        last_group_name = malloc(sizeof(gid_to_name(sbuf->st_gid)));
        last_file_size = (intptr_t)(malloc(sizeof(sbuf->st_size)));
        last_type = (intptr_t)(malloc(sizeof(get_file_type(sbuf->st_mode))));
        last_name = malloc(strlen(pathname + ftwb->base) + 1);

        strcpy(last_user_name, uid_to_name(sbuf->st_uid));
        strcpy(last_group_name, gid_to_name(sbuf->st_gid));
        last_type = get_file_type(sbuf->st_mode);
        last_file_size = sbuf->st_size;
        last_level = current_level;

        strcpy(last_name, pathname + ftwb->base);
        if (type == FTW_F)
            counter_file++;
        if (type == FTW_D && strcmp(".", name) != 0)
            counter_dir++;
    }
    return 0;
}

