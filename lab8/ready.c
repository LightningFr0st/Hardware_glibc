
#include <sys/stat.h>
#include <stdio.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <linux/mman.h>
#include <time.h>

#define NOT_Created !(ready[1] && ready[2] && ready[3] && ready[4] && ready[5] && ready[6] && ready[7] && ready[8])

volatile sig_atomic_t Received1 = 0;
volatile sig_atomic_t Received2 = 0;
volatile sig_atomic_t Sent1 = 0;
volatile sig_atomic_t Sent2 = 0;
volatile sig_atomic_t CurrProc = 1;

pid_t pid;

int* pid_array;

bool* ready;

void handleSIGUSR(int SIG) {
	if (SIG == SIGUSR1) {
		__sync_fetch_and_add(&Received1, 1);
	}
	if (SIG == SIGUSR2) {
		__sync_fetch_and_add(&Received2, 1);
	}
}

void handleSIGTERM() {
	sleep(0.5);
	printf("\n%d:%d:%d PROCESS TERMINATED, USR1 = % d, USR2 = % d, received USR1 = % d, received USR2 = % d\n",CurrProc,getpid(),
		getppid(), Sent1, Sent2, Received1, Received2);
	if (CurrProc != 1) {
		kill(getpid(), SIGKILL);
	}
	else {
		pid_t childpid = wait(NULL);
		while (childpid != -1) {
			childpid = wait(NULL);
		}
		exit(0);
	}

}
void createProc(int numCurr, int numNext) {
	if (CurrProc == numCurr) {
		sleep(2);
		pid = fork();
		if (!pid) {
			CurrProc = numNext; 
		}else{
            printf("%d created %d\n",getpid(), pid);
        }
        pid_array[CurrProc] = getpid();
        ready[CurrProc] = true;
	}
}

void createProcTree() {
	createProc(1, 2);
	createProc(2, 3);
	createProc(2, 4);
	createProc(2, 5);
	createProc(3, 6);
	createProc(4, 7);
	createProc(5, 8);
}

void sendsignals() {
	if (CurrProc == 1) {
		Sent1 = 0;
		while (Sent1 < 101) {
			kill(pid_array[8], SIGUSR1);
			sleep(0.5);
			printf("\n%d:%d:%d, USR1=%d, USR2 = % d, received USR1 = % d, received USR2 = % d, time: %f\n",
				CurrProc, getpid(), getppid(), Sent1++, Sent2,
				Received1, Received2, ((double)clock()/CLOCKS_PER_SEC)*1000000);
		}
	}
    else if (CurrProc == 8) {
		Sent1 = 0;
		while (Sent1 < 101) {
			kill(pid_array[7], SIGUSR1);
			sleep(0.5);
			printf("\n%d:%d:%d, USR1=%d, USR2 = % d, received USR1 = % d, received USR2 = % d, time: %f\n",
				CurrProc, getpid(), getppid(), Sent1++, Sent2,
				Received1, Received2, ((double)clock()/CLOCKS_PER_SEC)*1000000);
		}
	}
    else if (CurrProc == 7) {
		Sent2 = 0;
		while (Sent2 < 101) {
			kill(0, SIGUSR2);
			sleep(0.5);
			printf("\n%d:%d:%d, USR1=%d, USR2 = % d, received USR1 = % d, received USR2 = % d, time: %f\n",
				CurrProc, getpid(), getppid(), Sent1, Sent2++,
				Received1, Received2, ((double)clock()/CLOCKS_PER_SEC)*1000000);
		}
	} else if (CurrProc == 4) {
		Sent1 = 0;
		while (Sent1 < 101) {
			kill(pid_array[2], SIGUSR1);
			sleep(0.5);
			printf("\n%d:%d:%d, USR1=%d, USR2 = % d, received USR1 = % d, received USR2 = % d, time: %f\n",
				CurrProc, getpid(), getppid(), Sent1++, Sent2,
				Received1, Received2, ((double)clock()/CLOCKS_PER_SEC)*1000000);
		}
	} else if (CurrProc == 2) {
		Sent1 = 0;
		while (Sent1 < 101) {
			kill(pid_array[3], SIGUSR1);
			sleep(0.5);
			printf("\n%d:%d:%d, USR1=%d, USR2 = % d, received USR1 = % d, received USR2 = % d, time: %f\n",
				CurrProc, getpid(), getppid(), Sent1++, Sent2,
				Received1, Received2, ((double)clock()/CLOCKS_PER_SEC)*1000000);
		}
	} else if (CurrProc == 3) {
		Sent2 = 0;
		while (Sent2 < 101) {
			kill(pid_array[1], SIGUSR2);
			sleep(0.5);
			printf("\n%d:%d:%d, USR1=%d, USR2 = % d, received USR1 = % d, received USR2 = % d, time: %f\n",
				CurrProc, getpid(), getppid(), Sent1, Sent2++,
				Received1, Received2, ((double)clock()/CLOCKS_PER_SEC)*1000000);
		}
	}
	else {
		while(true){usleep(50000);}
	}
	kill(0, SIGTERM);
}

int main() {
    
    pid_array = (int*)mmap(NULL, 36, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    ready = (bool*)mmap(NULL, 36, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    memset(ready, false, 36);
	signal(SIGUSR1, handleSIGUSR);
	signal(SIGUSR2, handleSIGUSR);
	signal(SIGTERM, handleSIGTERM);
	createProcTree();

    while(NOT_Created){
        usleep(50000);    
    }

    sendsignals();

}
