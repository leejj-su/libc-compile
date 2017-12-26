#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#define KEYNUM 100
#define MSG_SIZE 1024 //메세지사이즈

key_t k[4]= {0,17891,27892,37893}; //key들의 배열

int msq_id[4]; //메세지큐 아이디
void *shm_addr[100];
int shm_id[100];

char id1[3] = "[1]";
char id2[3] = "[2]";
char id3[3] = "[3]";
char* ids[3]={id1,id2,id3};

//메세지큐 구조체
typedef struct{
   long mtype;
   char mtext[MSG_SIZE];
}MsgType;

//메세지를 보내는 쓰레드 함수
void send_msg (MsgType * msg,int queue_to_rcv){
   int snd_err,qid;
   MsgType sndmsg;
   sndmsg = *msg;
   qid = msq_id[queue_to_rcv]; //보낼 메세지 큐
   printf("Client%d -----> Client%d :%s\n",sndmsg.mtype,queue_to_rcv,sndmsg.mtext);
   if(-1==(snd_err = msgsnd(qid, msg, MSG_SIZE, IPC_NOWAIT ))){
      fprintf(stderr, "msgsnd() error\n");
      exit(0);
   }
}
void *thread_server(void* i)
{
   int rcverr;
   int queue_to_rcv;
   MsgType rcv_msg;
   int msg_size = sizeof(rcv_msg)-sizeof(rcv_msg.mtype);

   int shm_arr_size=0;
   char msg[MSG_SIZE];
   MsgType *shm_msg_arr;
   int count;
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
    while(1){
      for(int j = 1;j<=3;j++){
         for(int i = 4;i<=7;i++){
          //메세지큐를 한번씩한번씩 확인한다. 이때 mtype은 4 5 6 7(받는 이)을 확인한다.
               if(-1==(rcverr=msgrcv(msq_id[j],&rcv_msg,MSG_SIZE,i,IPC_NOWAIT))){
               //printf("fail");
               //printf("%d, %s\n",errno, strerror(errno));
               continue;
               /*printf("%d, %s\n",errno, strerror(errno));
               fprintf(stderr, "msgrcv() error\n");
                    exit(0);*/
                   }

             if(rcv_msg.mtype<7){
             //개인 톡인 경우
                  queue_to_rcv = rcv_msg.mtype-3; //보낼 메세지큐에 인덱스
                  rcv_msg.mtype = j; //mtype은 1,2,3 이다(보낸 이)
                  send_msg ( &rcv_msg, queue_to_rcv); //기존에 정의한 함수를 통해 메세지를 보낸다.
                  //printf("[%ld] : %s\n",rcv_msg.mtype, rcv_msg.mtext);
              
             }
             else if(rcv_msg.mtype ==7){
               strcpy((char *)(shm_addr[shm_arr_size]), ids[j-1]);
               strcpy((char*)(shm_addr[shm_arr_size]+3),rcv_msg.mtext);
               shm_arr_size++;
               if(shm_arr_size==100) shm_arr_size=0;
               printf("[%ld] : %s\n",j, rcv_msg.mtext);
              }
         }
      }
      
   }

}


int main(void)
{
   int command;
	//Message Queue part
	for(int i = 1;i<4;i++) msgctl(msq_id[i],IPC_RMID,NULL); 
	//서버가 새로 켜지면 전역변수로 선언된 key값으로 메세지큐를 삭제해줍니다.
    pthread_t t;
    for(int i = 1; i<=3;i++){ //메세지큐가 존재하지 않는다면 생성하고,해당 메세지큐들의 id를 저장해줍니다.
      if((msq_id[i]=msgget(k[i],IPC_CREAT|0644))==-1){
         printf("%d, %s\n",errno, strerror(errno));
               exit(0);
      }
      printf("msg_id[%d] : %d\n",i, msq_id[i]); //메시지큐 아이디 출력
   }
   int rc = pthread_create(&t, NULL, thread_server, NULL);
   if (rc){
      fprintf(stderr, "pthread_create() error\n");
      exit(0);
   } 
   while(1)
   {
      printf("------interface------\n1. lock chat_log\n2. Shutdown Server\n");
      scanf("%d",&command);
      if(command==2) break;
      else if(command==1)
      {
         printf("---------------------------chat_log----------------------------\n");
         for(int i=0;i<100;i++) {
            if(strlen(shm_addr[i])>0) printf("%s\n",(char*)shm_addr[i]);
         }
         printf("---------------------------chat_log----------------------------\n");
      }
      else
      {
         printf("input_error\n");
      } 
   }
   for(int i = 1;i<=3;i++) msgctl(msq_id[i],IPC_RMID,NULL); //메세지큐를 삭제합니다.
   for(int i=0;i<100;i++) {
       if(shmdt(shm_addr[i]) == -1) {
       perror("shmdt failed");
       exit(1);
       }
        shmctl(shm_id[i],IPC_RMID,NULL);
   }
   return 0;
}
