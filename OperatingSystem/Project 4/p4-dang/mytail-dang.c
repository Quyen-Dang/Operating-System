#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char* argv[])
{
    if(argc <= 2)
    {
        printf("Error! There are too few arguments!");
        return 1;
    }
    else if(argc == 3)
    {
        memmove(argv[1], argv[1] + 1, strlen(argv[1]));//remove the '-' before n (number of line reading)
        int n = atoi(argv[1]);
        char* file = argv[2];
        int readLine, lseekLine;
        char buf;
        
        int fd = open(file, O_RDONLY);
        if(fd < 0)
        {
            printf("Cannot open file.\n");
            return 1;
        }

        lseekLine = lseek(fd, 0, SEEK_END);//Read file from the end
        int i = 0;
        while(i < n)
        {
            lseekLine = lseek(fd, lseekLine - 1, SEEK_SET);
            readLine = read(fd, &buf, 1);
            if(buf == '\n')
                i++;
        }
        printf("Last %d lines of file %s are:\n", n, file);
        
        while(read(fd, &buf, 1) != 0){
            printf("%c", buf );
        }
        printf("\n");
       
        close(fd);   
    }
    return 0;
}