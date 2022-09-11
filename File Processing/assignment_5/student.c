#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "student.h"

int readRecord(FILE *fp, STUDENT *s, int rrn);
void unpack(const char *recordbuf, STUDENT *s);

int writeRecord(FILE *fp, const STUDENT *s, int rrn);
void pack(char *recordbuf, const STUDENT *s);

int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

void searchRecord(FILE *fp, FIELD f, char *keyval);
void printRecord(const STUDENT *s);

FIELD getFieldID(char *fieldname);

int deleteRecord(FILE *fp, enum FIELD f, char *keyval);
int insertRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

void main(int argc, char *argv[])
{
	FILE *fp;

	if(argc<2){
		printf("The input is invalid\n");
		return;
	}

	if(!strcmp(argv[1], "-a")){

		if(argc<APPEND_ARGCNUM){
			fprintf(stderr, "Usage : %s -a record_file_name \"id\" \"name\" \"dept\" \"addr\" \"email\"\n", argv[0]);
			return;
		}

		if(access(argv[2], F_OK)<0)
		{
			fp=fopen(argv[2], "wb+");
		}
		else
		{
			fp=fopen(argv[2], "rb+");
		}

		if(fp==NULL){
			fprintf(stderr, "fopen error for %s\n", argv[2]);
			return;
		}

		if(appendRecord(fp, argv[3], argv[4], argv[5], argv[6], argv[7])==0){
			fprintf(stderr, "Fail to append to record file\n");
			return;
		}
	}
	else if(!strcmp(argv[1], "-s")){

		if(argc<SEARCH_ARGCNUM){
			fprintf(stderr, "Usage : %s -s record_file_name \"field_name=field_value\"\n", argv[0]);
			return;
		}

		if(access(argv[2], F_OK)<0)
		{
			printf("Error: The file %s does not exist.\n", argv[2]);
			return;
		}
		else
		{
			fp=fopen(argv[2], "rb");
		}

		if(fp==NULL){
			fprintf(stderr, "fopen error for %s\n", argv[2]);
			return;
		}

		int f=-1;
		char *ptr = strtok(argv[3], "=");
		
		if((f=getFieldID(ptr))<0){
			printf("field_name is invalid\n");
			return;
		}
		ptr = strtok(NULL, "=");

		searchRecord(fp, f, ptr);
	}
	else if(!strcmp(argv[1], "-d")){

		if(argc<DELETE_ARGCNUM){
			fprintf(stderr, "Usage : %s -d record_file_name \"ID=delete_ID\"\n", argv[0]);
			return;
		}

		if(access(argv[2], F_OK)<0)
		{
			printf("Error: The file %s does not exist.\n", argv[2]);
			return;
		}
		else
		{
			fp=fopen(argv[2], "rb+");
		}

		if(fp==NULL){
			fprintf(stderr, "fopen error for %s\n", argv[2]);
			return;
		}

		int f;
		char *ptr = strtok(argv[3], "=");

		if(strcmp("ID", ptr)){
			printf("field_name must be \"ID\"\n");
			return;
		}

		f = ID;
		ptr = strtok(NULL, "=");
		if(!deleteRecord(fp, f, ptr)){
			fprintf(stderr, "deleteRecord() fail\n");
			return;
		}
	}
	else if(!strcmp(argv[1], "-i"))
	{
		if(argc<INSERT_ARGCNUM){
			fprintf(stderr, "Usage : %s  -i record_filename \"field_value1\" \"field_value2\" \"field_value3\" \"field_value4\" \"field_value5\"\n", argv[0]);
			return;
		}

		if(access(argv[2], F_OK)<0)
		{
			printf("Error: The file %s does not exist.\n", argv[2]);
			return;
		}
		else
		{
			fp=fopen(argv[2], "rb+");
		}

		if(fp==NULL){
			fprintf(stderr, "fopen error for %s\n", argv[2]);
			return;
		}

		if(!insertRecord(fp, argv[3], argv[4], argv[5], argv[6], argv[7])){
			fprintf(stderr, "appendRecord() fail\n");
			return;
		}
	}
	else{
		printf("Usage : %s -a record_file_name \"id\" \"name\" \"dept\" \"addr\" \"email\"\n", argv[0]);
		printf("or\n");
		printf("Usage : %s -s record_file_name \"field_name=field_value\"\n", argv[0]);
		return;
	}

	fclose(fp);
}

FIELD getFieldID(char *fieldname)
{
	if(!strcmp(fieldname, "ID"))
		return ID;
	else if(!strcmp(fieldname, "NAME"))
		return NAME;
	else if(!strcmp(fieldname, "DEPT"))
		return DEPT;
	else if(!strcmp(fieldname, "ADDR"))
		return ADDR;
	else if(!strcmp(fieldname, "EMAIL"))
		return EMAIL;
	else
		return -1;
}

void searchRecord(FILE *fp, FIELD f, char *keyval)
{
	int rrn=0;
	int find;
	char *ptr;
	char buf[RECORD_SIZE+1];

	STUDENT student;

	fseek(fp, HEADER_SIZE, SEEK_SET);

	while(fread(buf, RECORD_SIZE, 1, fp)!=0)
	{
		if(buf[0]=='*') continue;

		find = f;

		ptr = strtok(buf, "#");
		while(find>0){
			ptr = strtok(NULL, "#");
			find--;
		}

		if(!strcmp(ptr, keyval)){
			if(readRecord(fp, &student, rrn)==0){
				printf("Fail to search record file for %d rrn\n", rrn);
				continue;
			}
			else{
				printRecord(&student);
			}
		}

		rrn++;
	}
}

int readRecord(FILE *fp, STUDENT *s, int rrn)
{
	char recordbuf[RECORD_SIZE+1];

	if(fseek(fp, -RECORD_SIZE, SEEK_CUR)<0){
		fprintf(stderr, "fseek error\n");
		return 0;
	}

	if(fread(recordbuf, RECORD_SIZE, 1, fp)==0){
		fprintf(stderr, "fread error\n");
		return 0;
	}

	unpack(recordbuf, s);
	return 1;
}

void unpack(const char *recordbuf, STUDENT *s)
{
	char *ptr;

	ptr = strtok(recordbuf, "#");
	strcpy(s->id, ptr);

	ptr = strtok(NULL, "#");
	strcpy(s->name, ptr);
	
	ptr = strtok(NULL, "#");
	strcpy(s->dept, ptr);
	
	ptr = strtok(NULL, "#");
	strcpy(s->addr, ptr);

	ptr = strtok(NULL, "#");
	strcpy(s->email, ptr);
}

int writeRecord(FILE *fp, const STUDENT *s, int rrn)
{
	int *file_record_num = (int *)malloc(sizeof(int));
	char recordbuf[RECORD_SIZE+1];
	pack(recordbuf, s);

	fseek(fp, 0L, SEEK_SET);
	fread(file_record_num, 4, 1, fp);

	if(rrn<0 || (rrn!=0 && rrn>*file_record_num))
	{
		fprintf(stderr, "invalid index %d\n", rrn);
		printf("file_record_num: %d\n", *file_record_num);
		return 0;
	}

	rrn++;

	fseek(fp, 0L, SEEK_SET);
	fwrite(&rrn , 4, 1, fp);
	fseek(fp, 0L, SEEK_END);
	fwrite(recordbuf, RECORD_SIZE, 1, fp);

	free(file_record_num);
	return 1;
}

void pack(char *recordbuf, const STUDENT *s)
{
	sprintf(recordbuf, "%s#%s#%s#%s#%s#", s->id, s->name, s->dept, s->addr, s->email);
}

int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email)
{
	long filesize = 0;
	int record_num = 0;

	STUDENT student;
	strcpy(student.id, id);
	strcpy(student.name, name);
	strcpy(student.dept, dept);
	strcpy(student.addr, addr);
	strcpy(student.email, email);

	fseek(fp, 0L, SEEK_END);

	if((filesize=ftell(fp))<0){
		fprintf(stderr, "ftell error\n");
		return 0;
	}
	else if(filesize==0) {
		record_num = filesize;

		int head = -1;
		fseek(fp, 4L, SEEK_SET);
		fwrite(&head, 4L, 1, fp);
	}
	else {

		filesize -= HEADER_SIZE;
		record_num = filesize / RECORD_SIZE;
	}

	return (writeRecord(fp, &student, record_num));
}

void printRecord(const STUDENT *s)
{
	printf("%s | %s | %s | %s | %s\n", s->id, s->name, s->dept, s->addr, s->email);
}

int deleteRecord(FILE *fp, enum FIELD f, char *keyval)
{
	int rrn=1;
	int find;
	char *ptr;
	char buf[RECORD_SIZE+1];

	STUDENT student;
	fseek(fp, HEADER_SIZE, SEEK_SET);

	while(fread(buf, RECORD_SIZE, 1, fp)!=0)
	{
		find = f;
		
		ptr = strtok(buf, "#");
		while(find>0){
			ptr = strtok(NULL, "#");
			find--;
		}

		if(!strcmp(ptr, keyval)){

			fseek(fp, -RECORD_SIZE, SEEK_CUR);

			char head_s[5];
			char del = '*';
			long cur_fp = ftell(fp);

			fseek(fp, 4L, SEEK_SET);
			fread(head_s, 4, 1, fp);

			fseek(fp, 4L, SEEK_SET);
			fwrite(&rrn, 4, 1, fp);

			fseek(fp, cur_fp, SEEK_SET);
			fwrite(&del , 1, 1, fp);
			fwrite(head_s, 4, 1, fp);

			fseek(fp, cur_fp + RECORD_SIZE, SEEK_SET);
		}

		rrn++;
	}

	return 1;
}

int insertRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email)
{
	int *head_num = (int *)malloc(sizeof(int));
	char check;

	fseek(fp, 4L, SEEK_SET);
	fread(head_num, 4, 1, fp);

	if(*head_num==-1){
		if(appendRecord(fp, id, name, dept, addr, email)==0){
			fprintf(stderr, "Fail to append to record file\n");
			return 0;
		}
	}
	else{

		fseek(fp, 8L, SEEK_SET);
		fseek(fp, RECORD_SIZE*(*head_num-1), SEEK_CUR);

		fread(&check, 1, 1, fp);
		if(check!='*'){
			printf("invaild delete_record\n");
			return 0;
		}
		fread(head_num, 4, 1, fp);
		
		char recordbuf[RECORD_SIZE+1];
   		STUDENT s;
		strcpy(s.id, id);
		strcpy(s.name, name);
		strcpy(s.dept, dept);
		strcpy(s.addr, addr);
		strcpy(s.email, email);

		pack(recordbuf, &s);

		fseek(fp, -5L, SEEK_CUR);
		fwrite(recordbuf, RECORD_SIZE, 1, fp);

		fseek(fp, 4L, SEEK_SET);
		fwrite(head_num, 4, 1, fp);
	}

	return 1;
}
