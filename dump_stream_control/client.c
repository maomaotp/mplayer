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
	int inver;

	//获取流下载的pid
	recv(socketfd, mplayer_pid, sizeof(mplayer_pid), 0);
	pid = atoi(mplayer_pid);
	printf("pid==%d\n", pid);

	struct proTime pro_time;
	//time_list = (struct playtime *)calloc(200, sizeof(struct playtime));

	send_task.radioId = 40;

	queryInfo(&send_task, &pro_time);

	int i;
	for(i=0; i<pro_time.row; i++){
		send_task.programId = pro_time.time_list[i].programId;
		strcpy( send_task.playtime, pro_time.time_list[i].ptime );
#ifdef DEBUG
		printf("pro_time.time_list[%d].ptime = %s\n", i, pro_time.time_list[i].ptime);
#endif

	    send(socketfd, (char *)&send_task, sizeof(struct task), 0);
		printf("%d  %d  %s  %s\n", send_task.radioId, send_task.programId, send_task.playtime, send_task.uri);

		if(i == pro_time.row-1){
#ifdef DEBUG
			printf("the last programId\n");
#endif
			inver = 70;
			sleep( inver*60 );
			kill(pid, SIGINT);
			break;
		}
		else {
			inver = inver_time(pro_time.time_list[i].ptime, pro_time.time_list[i+1].ptime);
		}

		if(inver <= 0){
#ifdef debug
			printf("the inver is error\n");
#endif
			inver = 70;
		}

		printf("inver_time = %d\n", inver);
		sleep( inver*60 );
		kill(pid, SIGUSR1);
	}

    return 0;
}
