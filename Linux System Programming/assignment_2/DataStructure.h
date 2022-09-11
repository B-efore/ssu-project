#ifndef __DATA_STRUCTS_H__
#define __DATA_STRUCTS_H__

# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <errno.h>
# include <unistd.h>
# include <dirent.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <ctype.h>
# include <pwd.h>
# include <openssl/md5.h>
# include <openssl/sha.h>

# define KEY_SIZE 45
# define BUFFER_SIZE 4096
# define PATH_SIZE 4096
# define FILE_NAME_SIZE 255
# define ARGUMENT_SIZE 5
# define INF 2147483647

typedef char Data;

typedef struct Node{
	Data data[BUFFER_SIZE];
	struct Node *next;
} Node;

typedef struct Queue{
	Node *front;
	Node *rear;
} Queue;

typedef struct List{
	unsigned char key[KEY_SIZE];
	int size;
	int dataNum;
	Node *head;
	Node *tail;
	Node *cur;
	Node *before;
	struct List *next;
} List;

typedef struct Table{
	List *head;
	List *tail;
	List *cur;
	List *before;
	int dataNum;
} Table;



//Queue 관련 함수
void QueueInit(Queue *queue);
void Enqueue(Queue *queue, Data data[]);
void Dequeue(Queue *queue, Data data[]);
bool isEmpty(Queue *queue);

//List 관련 함수
List *isExist(Table *table, unsigned char *key);
void ListInit(List *list);
void Insert(List *list, Data data[]);
void ListSearch(Table *table, List *list, int count);
Data *Delete(List *list);

//Table 관련 함수
void TableInit(Table *table);
void TableInsert(Table *table, Data data[], unsigned char *key, int size);
void TableDelete(Table *table);
void DeleteNotEqual(Table *table);
void TableSearch(Table *table, int count);
int compare(int n1, int n2);

Table table;
struct passwd *pw;
char *homedir;
char trash[FILE_NAME_SIZE * 2];

#endif
