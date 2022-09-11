#include "DataStructure.h"

/* 큐 관련 함수 */
//큐 초기화
void QueueInit(Queue *queue)
{
	queue -> front = NULL;
	queue -> rear = NULL;
}

//큐에 데이터 추가
void Enqueue(Queue *queue, Data data[])
{
	Node * newNode = (Node *)malloc(sizeof(Node));

	strcpy(newNode->data, data);
	newNode -> next = NULL;

	if(queue -> front == NULL)
		queue -> front = newNode;
	else
		queue -> rear -> next = newNode;

	queue -> rear = newNode;
}

//큐에서 데이터 삭제
void Dequeue(Queue *queue, Data data[])
{
	if(!isEmpty(queue))
	{
		Node *tmpNode = queue -> front;
		strcpy(data, tmpNode -> data);

		if(tmpNode == queue -> rear)
		{
			queue -> front = NULL;
			queue -> rear = NULL;
		}
		else
			queue -> front = tmpNode -> next;

		free(tmpNode);
	}
}

//큐가 비었는지 확인
bool isEmpty(Queue *queue)
{
	return (queue -> front == NULL);
}

/* 리스트 관련 함수 */
//키 값과 일치하는 데이터를 찾고, 찾으면 데이터의 위치를 리턴
List *isExist(Table *table, unsigned char *key)
{
	List *tmp = table -> head -> next;

	while(tmp!=NULL){
		if(!strcmp(key, tmp->key)){
			break;
		}
		
		tmp = tmp -> next;
	}

	return tmp;
}

//리스트 초기화
void ListInit(List *list)
{
	Node *dummy = (Node *)malloc(sizeof(Node));

	list -> head = dummy;
	list -> head -> next = NULL;
	list -> tail = list -> head;
	list -> cur = NULL;
	list -> before = NULL;
	list -> dataNum = 0;
}

//리스트에 데이터 추가
void Insert(List *list, Data data[])
{
	Node *newNode = (Node *)malloc(sizeof(Node));
	strcpy(newNode -> data, data);
	newNode -> next = NULL;
	
	list -> tail -> next = newNode;
	list -> tail = newNode;
	
	(list -> dataNum)++;
}

//count 자리의 데이터로 커서 이동
void ListSearch(Table *table, List *list, int count)
{
	table -> cur -> cur = table -> cur -> head -> next;
	table -> cur -> before = table -> cur -> head;
	count--;
	
	while(count>0 && table->cur->cur!=NULL){
		table -> cur -> before = table -> cur -> cur;
		table -> cur -> cur = table -> cur -> cur -> next;
		count--;
	}
}

//리스트 데이터 삭제
Data *Delete(List *list)
{
	Node *tmpNode = list -> cur;
	Data *data = (Data *)malloc(sizeof(Data)*BUFFER_SIZE);
	strcpy(data, tmpNode->data);	

	list -> before -> next = tmpNode -> next;
	list -> cur = list -> before;

	(list -> dataNum)--;

	free(tmpNode);
	return data;
}

/* 테이블 관련 함수 */
//테이블 초기화
void TableInit(Table *table)
{
	List *dummy = (List *)malloc(sizeof(List));
	dummy -> next = NULL;

	table -> head = dummy;
	table -> tail = dummy;
	table -> cur = NULL;
	table -> before = NULL;
	table -> dataNum = 0;
}

//테이블에 데이터 추가
void TableInsert(Table *table, Data data[], unsigned char *key, int size)
{
	List *tmp = isExist(table, key);

	// 일치하는 키가 없을 경우 새 리스트를 만들어 테이블에 넣음
	if(tmp==0){
		
		List *newList = (List *)malloc(sizeof(List));
		ListInit(newList);

		newList -> size = size;
		strcpy(newList->key, key);
		
		tmp = table -> head;

		while(tmp -> next != NULL && compare(size, tmp->next->size) != 0){
			tmp = tmp -> next;
		}
		
		if(tmp -> next ==  NULL)
			newList -> next = NULL;
		else
			newList -> next = tmp -> next;
		
		tmp -> next = newList;
		(table->dataNum)++;	
		Insert(newList, data);
	}
	// 있는 경우 리스트에 데이터를 넣음
	else
		Insert(tmp, data);
}

//테이블 데이터 삭제
void TableDelete(Table *table)
{
	List *tmpList = table -> cur;

	table -> before -> next = tmpList -> next;
	table -> cur = table -> before;

	(table -> dataNum)--;
	free(tmpList);
}

//중복파일이 없는 경우 테이블에서 삭제
void DeleteNotEqual(Table *table)
{
	if(table -> dataNum > 0){

		table -> cur = table -> head -> next;
		table -> before = table -> head;

		while(table -> cur != NULL){
			//중복 파일 세트 내의 데이터 개수가 2개 미만인 것 삭제
			if(table->cur->dataNum < 2)
				TableDelete(table);

			table -> before = table -> cur;
			table -> cur = table -> cur -> next;
		}
	}
}

//count 자리의 데이터로 커서 이동
void TableSearch(Table *table, int count)
{
	table -> cur = table -> head -> next;
	table -> before = table -> head;
	count--;

	while(count>0 && table->cur!=NULL){
		table -> before = table -> cur;
		table -> cur = table -> cur -> next;
		count--;
	}
}

//비교 함수
int compare(int n1, int n2)
{
	if(n1 < n2)
		return 0;
	else
		return 1;
}
