#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <unistd.h>

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        printf("There is no file or directory for statistic.");
        return 1;
    }
    else if(argc == 2)
    {
        char* file = argv[1];
        struct stat mystat;

        stat(file, &mystat);

        printf("File size is: %d\n", mystat.st_size);
        printf("Number of blocks is: %d\n", mystat.st_blocks);
        printf("Link count is: %d\n", mystat.st_nlink);
        printf("File permission is: %o\n", mystat.st_mode);
        printf("File inode is: %d\n", mystat.st_ino);

    }
    
    return 0;
}

