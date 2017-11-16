#include <stdio.h>
#include <signal.h>


void sig_fn(int sig_num){
	printf("\nsignal number : %d\n", sig_num);
	printf("Ctrl-C is pressed. Try Again\n");
}

int main(){
	struct sigaction act;
	act.sa_handler = sig_fn;

	sigaction(SIGINT, &act, NULL);
	while(1){
		printf("running\n");
		pause();
	}

	return 0;
}