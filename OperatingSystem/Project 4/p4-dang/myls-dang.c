#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    struct stat mystat;

    if(argc <= 2)
    {
        //char buf;
        DIR* directory;
        char* dir;
        char buf[500];
        dir = getcwd(buf, sizeof(buf));
        
        directory = opendir(dir);
        struct dirent* dirnt;

        if((dirnt = readdir(directory)) == NULL)
        {
            printf("Error open the directory.\n");
            return 1;
        }

        while((dirnt = readdir(directory)) != NULL)
        {
            if(argc == 1)
            {
                printf("File: %s\n", dirnt -> d_name);
            }
            else if(argv[1] = "-l")
            {
                stat(dirnt -> d_name, &mystat);
                printf("File: %s\n", dirnt -> d_name);
                printf("Owner ID: %d\t Group ID: %d\n", mystat.st_uid, mystat.st_gid);
                printf("File size is: %d\n", mystat.st_size);
                printf("Number of blocks is: %d\n", mystat.st_blocks);
                printf("Link count is: %d\n", mystat.st_nlink);
                printf("File permission is: %o\n", mystat.st_mode);
                printf("File inode is: %d\n\n", mystat.st_ino);       
            }
        }
        closedir(directory);
    }
    else if(argc == 3)
    {
        DIR* directory;
        directory = opendir(argv[2]);
        struct dirent* dirnt;

        if(directory == NULL)
        {
            printf("Error open the directory.\n");
            return 1;
        }
        else
        {
            while((dirnt = readdir(directory)) != NULL)
            {                
                stat(dirnt -> d_name, &mystat);
                printf("File: %s\n", dirnt -> d_name);
                printf("Owner ID: %d\t Group ID: %d\n", mystat.st_uid, mystat.st_gid);
                printf("File size is: %d\n", mystat.st_size);
                printf("Number of blocks is: %d\n", mystat.st_blocks);
                printf("Link count is: %d\n", mystat.st_nlink);
                printf("File permission is: %o\n", mystat.st_mode);
                printf("File inode is: %d\n\n", mystat.st_ino);          
            }
            closedir(directory);
        }
    }
    return 0;
}
