#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

void listFile(char* basedir, int root)
{
    char buf[500];
    DIR* directory;
    directory = opendir(basedir);
    struct dirent* dirnt;

    if((dirnt = readdir(directory)) == NULL)
    {
        printf("Error open the directory.\n");
        exit;
    }

    while((dirnt = readdir(directory)) != NULL)
    {
        if (strcmp(dirnt->d_name, ".") == 0 || strcmp(dirnt->d_name, "..") == 0)
            continue;
        
        printf("%*s %s\n", root, "",dirnt->d_name);

        if (dirnt->d_type == DT_DIR)
        {
            strcpy(buf, basedir);
            strcat(buf, "/");
            strcat(buf, dirnt->d_name);
            listFile(buf, root + 2);
        }
    }

    closedir(directory);
}

int main(int argc, char* argv[])
{
    char* dir;
    char buf[500];
    if(argc == 1)
    {
        dir = getcwd(buf, sizeof(buf));
    }
    else if(argc == 2)
    {
        dir = argv[1];
    }
    listFile(dir, 0);
    return 0;
}

