#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>

#include "mysql/mysql.h"
#include "client.h"

int socketfd;

void init_socket()
{
    struct sockaddr_in servaddr;
    
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.120");
   
    if(connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        exit(1);
    }
}

int play_time(char *timestr)
{
	char tmp[20];
	char h[5];
	char m[5];
	char s[5];
	strcpy(tmp, timestr);
	strcpy(h,strtok(tmp, ":"));	
	strcpy(m,strtok(NULL, ":"));
	strcpy(s,strtok(NULL, ":"));

	return atoi(h)*60+atoi(m);
}

int inver_time(char *start, char *end)
{
	if (!start || !end){
		return 0;
	}

	int a = play_time(start);
	int b = play_time(end);
	
	return b-a;
}

int main()
{
	init_socket();
	char mplayer_pid[20];
	struct task send_task;
	pid_t pid;

	//获取流下载的pid
	recv(socketfd, mplayer_pid, sizeof(mplayer_pid), 0);
	pid = atoi(mplayer_pid);
	printf("pid==%d\n", pid);

	struct playtime *time_list;
	time_list = (struct playtime *)calloc(200, sizeof(struct playtime));
	if(!time_list){
		printf("calloc error\n");
		return -1;
	}

	send_task.radioId = 19;

	queryInfo(&send_task, time_list);

	int i=0;
	while(time_list[i].programId != 0){
		send_task.programId = time_list[i].programId;
		strcpy( send_task.playtime, time_list[i].ptime );

	    send(socketfd, (char *)&send_task, sizeof(struct task), 0);
		printf("%d  %d  %s  %s\n", send_task.radioId, send_task.programId, send_task.playtime, send_task.uri);

		int inver = inver_time(time_list[i].ptime, time_list[i+1].ptime);

		printf("inver_time = %d\n", inver);
		if(inver <= 0){
			sleep( 60 * 60 * 60 );
			kill(pid, SIGINT);
			break;
		}
		else{
			sleep( inver * 60 );
			kill(pid, SIGUSR1);
		}
		i++;
	}

    return 0;
}
