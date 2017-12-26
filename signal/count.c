#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
int fd;
int num;

clock_t start_time,end_time;

pid_t process1;
pid_t process2;
pid_t process3;

int next;
void handler();
void QUIT();


int main(int argc, char** argv)
{
	num = atoi(argv[1]);
	int check;
	if(argc!=3)
	{
		printf("argc is not 3.\n");
		return 0;
	}
	if(num>0) printf("argv[1] : %d\n",num);
	else {
		printf("Not more than 0.\n");
		return 0;
	}
	if(access(argv[2],F_OK)==-1)
	{
		printf("file does not exist. Therefore creates a new one.\n");
		fd = open(argv[2],O_RDWR | O_CREAT, 0666);
	}
	else
	{
		fd = open(argv[2],O_RDWR , 0666);
		pread(fd,&check,4,0);
		if(check!=0)
		{
			fd = open(argv[2],O_TRUNC|O_RDWR, 0666);
			printf("Existing content is not zero. Therefore, the file is rewritten by 0.\n");
		}
	}
	printf("fd : %d\n",fd);
	process1= getpid();
	if(signal(SIGUSR1,handler)==SIG_ERR)
	{
		perror("signal error\n");
	}
	if(signal(SIGUSR2,QUIT)==SIG_ERR)
	{
		perror("signal error\n");
	}

	clock_t t;
	double start_time;
	if((process2 = fork())<0)
	{
		printf("fork errer\n");
	}
	if(process2==0) //process2
	{	
		if((process3=fork())<0)
		{
			printf("fork errer\n");
		}
		if(process3==0) //process3
		{	
			next=process1;
			start_time=clock();
			kill(process1,SIGUSR1);
			for(;;) 
			{
				pause();
				//printf("process2 send signal to process3\n");
			}
		}
		else //process2
		{
			process2=getpid();
			next=process3;
			for(;;)	
			{
				pause();
				//printf("process1 send signal to process2\n");
			}
		}

	}
	else //process1
	{
			next=process2;
			for(;;) 
			{
				pause();
				//printf("process3 send signal to process1\n");
			}
	}
	return 0;
}


void handler()
{
    int number;
    pread(fd,&number,4,0);

    if(number>=num){
    		end_time = clock();
    		printf("time: %f\n",(double)(end_time-start_time)/CLOCKS_PER_SEC);
    		printf("completer number : %d\n",number);
    		kill(process1,SIGUSR2);
    		exit(1);
    }
    else
    {
    		number++;
    		pwrite(fd,&number,4,0);
    		kill(next,SIGUSR1);
    }
}

void QUIT()
{
	exit(0);
}
