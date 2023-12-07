#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{	
	int pip_position = argc;
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "|") == 0)
		{
			pip_position = i;
			break;
		}
	}

	char* cmd1[pip_position];
	char* cmd2[argc - pip_position];

	if(argc < 4 || pip_position > argc - 1 )
	{
		printf("You should enter more commands(cmd1 \"|\" cmd2) to test this file.\n");
		exit(0);
	}
	else
	{
		for(int i = 1; i < pip_position; i++)
		{
			cmd1[i-1] = argv[i];
		}	
		cmd1[pip_position-1] = NULL;
			
		for(int j = pip_position; j < argc; j++)
		{
 			cmd2[j - pip_position] = argv[j+1]; 
		}
		cmd2[argc - pip_position -1] = NULL;			
	}
	int fd[2];
	if(pipe(fd) == -1)
	{
		printf("Can't open the pipe\n");
		exit(1);	
	}
	int id1 = fork();
	if(id1 < 0) 
	{
		fprintf(stderr, "fork failed\n");
	        exit(1);
	}
	else if(id1 == 0)
	{			
		printf("I am the first process (pid: %d)\n", (int) getpid());
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		execvp(cmd1[0], cmd1);
	}	

	int id2 = fork();
	if(id2 < 0)
	{
		fprintf(stderr, "fork failed\n");
		exit(1);
	}
	else if(id2 == 0)
	{
		printf("I am the second process (pid: %d\n", (int) getpid());
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		close(fd[1]);
		execvp(cmd2[0],cmd2);
	}
		
	close(fd[0]);
	close(fd[1]);

	waitpid(id1, NULL, 0);
	waitpid(id2, NULL, 0);

	printf("I am the parent of two children %d and %d), (pid: %d)\n", id1, id2, (int) getpid());
	printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	return 0;
}
	
