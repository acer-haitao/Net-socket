#include <stdio.h>
#include <sqlite3.h>

#define  DATABASE   "student.db"
#define  N  128

int main(int argc, const char *argv[])
{
	sqlite3 *db;
	char *errmsg;
	int n = 0;
	

	if(sqlite3_open(DATABASE,&db) != SQLITE_OK)
	{
		sqlite3_errmsg(db);
	}
	else
	{
		printf("database open success!\n");
	}

	if(sqlite3_exec(db, "create table stu(id int , name char , score int);", NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("create table success.\n");
	}

	while(1)
	{
		printf("************************************\n");
		printf("1 : insert \n");
		printf("2 : delete \n");
		printf("3 : update \n");
		printf("4 : query \n");
		printf("5 : quit \n");
		printf("************************************\n");

		printf("Input:");
		scanf("%d", &n);
		switch(n)
		{
			case 1:
				do_insert(db);
				break;
			case 2:
				//	
				break;
			case 3:
				//
				break;
			case 4:
				do_query(db);
				break;
			default:
				printf("Invalid data n!\n");
		}

	}


	return 0;
}


int do_insert(sqlite3 *db)
{
	char *errmsg;
	char sql[N] = {};
	int id;
	char name[32] = {};
	int score;
	
	printf("Input id:");
	scanf("%d",&id);
	printf("Input name:");
	scanf("%s", name);
	printf("Input score:");
	scanf("%d", &score);

	sprintf(sql, "insert into stu values(%d, '%s', %d);", id, name, score);

	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg)!= SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Insert done!\n");
	}

	return 0;
}

int callback(void* arg, int f_num , char ** f_value  , char ** f_name)
{
	int i = 0;
	printf("f_num = %d \n", f_num);
	for(i = 0; i < f_num; i++)
	{
		printf("%s %s ", f_value[i], f_name[i]);
	}
	putchar(10);

	return 0;
}

int do_query(sqlite3 *db)
{
	char *errmsg;
	char sql[N] = {};
	int id;
	char name[32] = {};
	int score;
	
	printf("Input id:");
	scanf("%d",&id);

	sprintf(sql, "select * from stu where id = %d", id);

	if(sqlite3_exec(db, sql,callback, NULL, &errmsg)!= SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Select done!\n");
	}

}

