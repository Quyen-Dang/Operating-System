#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
	char* params[argc];
	char* cmd;	
	if(argc == 1)
	{
		printf("You should type one or some commands in order to test the API process. \n");
		exit(0);
	}
	else if(argc >= 2)
	{
		cmd = argv[1];
		if(argc == 2)
		{
			params[0] = argv[1];
			params[1] = NULL;
		}
		else
		{
			params[0] = argv[1];
			for(int i = 2; i < argc; i++)
			{
				params[i-1] = argv[i];
			}	
			params[argc-1] = NULL;
		}
	}
	
	int id = fork();
	if(id < 0) 
	{
		fprintf(stderr, "fork failed\n");
	        exit(1);
	}
	else if(id == 0)
	{
		printf("I'm children (pid: %d)\n", (int) getpid());
		printf("We are going to have a new process here to test the API process!\n");			
		execvp(cmd, params);
	}	
	else
	{
		wait(NULL);
		printf("I'm parent of %d (pid: %d)\n", id, (int) getpid());
		printf("We are going back to the main process!\n");
		printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	}
	return 0;
}
	
