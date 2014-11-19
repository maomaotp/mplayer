#include "mysql/mysql.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void Init_Sql(MYSQL* mysql)
{
	mysql_init(mysql);
	
	if(NULL == mysql_real_connect(mysql, MYSQL_SERVER_IP, USER_NAME, PASSWD, DB_NAME, 0, NULL, 0)){
		printf("connect:%s\n",mysql_error(mysql));
		exit(-1);
	}
	printf("connect success!\n");

	return;
}

void queryInfo(struct task *send_task, struct playtime *time_list)
{
	MYSQL mysql;
    char sql_buf[2048];
	char sql_uri[200];

	snprintf(sql_uri, sizeof(sql_uri), "select url from Radio_Info where radioId=%d\n", send_task->radioId);
	snprintf(sql_buf, sizeof(sql_buf), "select B.programId,B.playTime from Program_Info A,Program_Time B where A.RadioID = %d and B.ProgramID = A.ProgramID and B.Day=CURDATE() order by playTime limit 20", send_task->radioId);

	Init_Sql(&mysql);
	//设置Mysql字符编码
	mysql_set_character_set(&mysql, "utf8");

	//获取radio对应的uri
	if( mysql_query(&mysql, sql_uri) ) {
		printf("error making query:%s\n",mysql_error(&mysql));
		return ;
	}
	MYSQL_RES* uri_res = mysql_use_result(&mysql);
	if( !uri_res){
		printf("err use_result:%s\n",mysql_error(&mysql));
		return ;
	}	
	MYSQL_ROW row_uri;
	row_uri = mysql_fetch_row(uri_res);		

	strcpy( send_task->uri, row_uri[0] );
	printf("send_task.uri=%s\n", send_task->uri);

	mysql_free_result(uri_res);
	mysql_close(&mysql);


	//获取传播单的播放时间，传播单ID列表
	Init_Sql(&mysql);
	//设置Mysql字符编码
	mysql_set_character_set(&mysql, "utf8");

	if( mysql_query(&mysql, sql_buf) ) {
		printf("error making query:%s\n",mysql_error(&mysql));
		return ;
	}

	MYSQL_RES* sql_res = mysql_store_result(&mysql);
	if(sql_res == NULL){
		printf("err use_result:%s\n",mysql_error(&mysql));
		return ;
	}	

	int i=0;
	MYSQL_ROW row_buf;

	while(1){
		row_buf = mysql_fetch_row(sql_res);		
		if(!row_buf) break;
		time_list[i].programId = atoi(row_buf[0]);
		strcpy(time_list[i].ptime, row_buf[1]);
		i++;
	}
	
	mysql_free_result(sql_res);
	mysql_close(&mysql);
}
