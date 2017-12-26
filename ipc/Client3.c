#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#define KEYNUM 100
#define MSG_SIZE 1024
#define CheckMessage 1
#define SendMessage 2
#define Exit 3
key_t k[4] = {0,17891,27892,37893};

int shm_id[100];
int sender=3; //보낼 사람
void *shm_addr[100];

//메세지큐 구조체
typedef struct{
   long mtype; 
   char mtext[1024];
}MsgType;

//메세지를 보내주는 함수
void *thread_snd(void *Msg)
{
   
   pthread_t *tid;
   MsgType * M = (MsgType *)Msg;
   key_t key = k[sender];
   M->mtype += 3; 
   int que_id= msgget(key,IPC_CREAT|0666);
   if(msgsnd(que_id,M,MSG_SIZE,IPC_NOWAIT)<0)
   {
      printf("ERROR : %d, %s\n",errno , strerror(errno));
      exit(0);
   }
   

}
 
//메세지를 받는 함수
void *thread_rcv(void *pid)
{
   int *tmpid = (int*)pid,chk = 0;
   key_t key = k[*tmpid];
   int que_id= msgget(key,IPC_CREAT|0666);
   MsgType msg;
   while(1){
      if(msgrcv(que_id, &msg, 1024, -3, IPC_NOWAIT)>0){
         printf("--------------personal message %d--->%d--------------\n",msg.mtype,*tmpid);
         printf("[%ld] : %s\n",msg.mtype, msg.mtext);
         printf("-----------------------------------------------------\n\n");
      }
   }
   //-4를 mtype으로 줌으로써 mtype이 3, 2, 1을 읽어온다.
}

 

int main()
{
   int command;
   int client_number;
   int rc,first=1, second=2, third=3;
   MsgType Message1;
   MsgType Message2;
   pthread_t *t;
   t = (pthread_t*)malloc(sizeof(pthread_t)*7);

   for(int i=0;i<100;i++)
   {
      if(-1 == (shm_id[i] = shmget( (key_t)(KEYNUM+i), MSG_SIZE, IPC_CREAT|0666)))
      {
         printf("shmget fail1\n");
         return -1;
       }
      if((void*)-1 == (shm_addr[i] = shmat( shm_id[i], (void*)0, 0)))
      {
          printf("shmat fail2\n");
          return -1;
       }
   }
   //shmget으로 shm_id를 얻고, shmat으로 해당 address를 받아온다

   
   rc = pthread_create(&t[3], NULL, thread_rcv, (void*)&third);
   if (rc){
      fprintf(stderr, "pthread_create() error\n");
      exit(0);
   }

   //메세지를 받아주는 쓰레드 생성
   //----------------------------------------------------------
   
   while(1)
   {
      //printf("Who are you?.\n1. Client1\n2. Client2\n3. Client3\n4. Quit\n*************************************\n");
      //scanf("%d",&sender);
      printf("Whom do you want to send Message?\n1. Client1\n2. Client2\n3. Client3\n4. to Everyone\n5. lock Chat_log\n6. EXIT\n");
      scanf("%ld",&Message1.mtype);
	  if(Message1.mtype==6) return 0; //프로그램 종료
      else if(!(Message1.mtype>=1 &&Message1.mtype<=7))printf("input error\n"); //입력 범위 오류
	  else if(Message1.mtype==5)
      {
           printf("---------------------------chat_log----------------------------\n");
         for(int i=0;i<100;i++) {
            if(strlen(shm_addr[i])>0) printf("%s\n",(char*)shm_addr[i]);
         }
         printf("---------------------------chat_log----------------------------\n");
      }
     else
	  {
		 printf("Please enter the Message\n");
		 scanf("%s",Message1.mtext);
		 while(getchar()!=(int)'\n');
		 rc = pthread_create(&t[sender+3], NULL, thread_snd, (void*)&Message1); //메세지 보내는 쓰레드 생성
		 if (rc)
		 {
			 fprintf(stderr, "pthread_create() error\n");
			 exit(0);
		 } 
      }  
	}
	free(t);
	return 0;
}
