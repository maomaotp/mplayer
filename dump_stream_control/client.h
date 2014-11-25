#ifndef CLIENT_HEAD
#define CLIENT_HEAD

#define DEBUG

#define MYSQL_SERVER_IP "123.57.41.242"
#define USER_NAME "dba"
#define PASSWD "123456"
#define DB_NAME "test"

struct task
{
	int radioId;
	int programId;
	char playtime[20];
	char uri[100];
};

struct timeList 
{
	char ptime[20];
	int programId;
};

struct proTime
{
	int row;
	struct timeList *time_list;
};


int queryInfo(struct task *send_task, struct proTime *pro_time);

#endif
