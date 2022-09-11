typedef struct trashFile {
	struct stat statbuf;
	char hash[HASHMAX];
	char path[PATHMAX];
	char d_date[15];
	char d_time[15];
	struct trashFile *next;
} trashFile;

// 휴지통에 존재하는 파일들에 대한 정보를 담는 리스트
trashFile *trash_list;

void swap_trashfile(trashFile *f1, trashFile *f2);
void sort_trashfile_with_path(trashFile *head, int n);
void sort_trashfile_with_date(trashFile *head, int n);
void sort_trashfile_with_time(trashFile *head, int n);
void sort_trashfile_with_size(trashFile *head, int n);
void trashfile_add(trashFile *head, char *argv[ARGMAX]);
void trashfile_delete(trashFile *head, int n, char *deleted_path);
void trashfile_print(trashFile *head);
int trashfile_size(trashFile *head);

// f1과 f2를 swap
void swap_trashfile(trashFile *f1, trashFile *f2)
{
	struct stat statbuf;
	char hash[HASHMAX];
	char path[PATHMAX];
	char d_date[15];
	char d_time[15];

	statbuf = f1->statbuf;
	strcpy(hash, f1->hash);
	strcpy(path, f1->path);
	strcpy(d_date, f1->d_date);
	strcpy(d_time, f1->d_time);

	f1->statbuf = f2->statbuf;
	strcpy(f1->hash, f2->hash);
	strcpy(f1->path, f2->path);
	strcpy(f1->d_date, f2->d_date);
	strcpy(f1->d_time, f2->d_time);

	f2->statbuf = statbuf;
	strcpy(f2->hash, hash);
	strcpy(f2->path, path);
	strcpy(f2->d_date, d_date);
	strcpy(f2->d_time, d_time);
}

// path를 기준으로 trashfile을 정렬한다
void sort_trashfile_with_path(trashFile *head, int n)
{
	int size = trashfile_size(head);
	trashFile *cur;
	trashFile *before;

	for(int j=0; j<size ; j++)
	{
		cur = head->next;
		before = head;

		if(cur==NULL) break;

		for(int k=0; k<size-1-j; k++)
		{
			if(n*strcmp(cur->path, cur->next->path)>0)
				swap_trashfile(cur, cur->next);

			before = cur;
			cur = cur->next;

			if(cur==NULL) break;
		}
	}
}

// d_date를 기준으로 trashfile을 정렬한다
void sort_trashfile_with_date(trashFile *head, int n)
{
	int size = trashfile_size(head);
	trashFile *cur;
	trashFile *before;

	for(int j=0; j<size ; j++)
	{
		cur = head->next;
		before = head;

		if(cur==NULL) break;
		
		for(int k=0; k<size-1-j; k++)
		{
			if(n*strcmp(cur->d_date, cur->next->d_date)>0)
				swap_trashfile(cur, cur->next);

			before = cur;
			cur = cur->next;

			if(cur==NULL) break;
		}
	}
}

// time을 기준으로 trashfile을 정렬한다
void sort_trashfile_with_time(trashFile *head, int n)
{
	int size = trashfile_size(head);
	trashFile *cur;
	trashFile *before;

	for(int j=0; j<size ; j++)
	{
		cur = head->next;
		before = head;

		if(cur==NULL) break;

		for(int k=0; k<size-1-j; k++)
		{
			if(n*strcmp(cur->d_time, cur->next->d_time)>0)
				swap_trashfile(cur, cur->next);

			before = cur;
			cur = cur->next;
		
			if(cur==NULL) break;
		}
	}
}

// size를 기준으로 trashfile을 정렬한다
void sort_trashfile_with_size(trashFile *head, int n)
{
	trashFile *before;
	trashFile *cur;
	trashFile *end = NULL;

	int swapped;

    do
    {
        swapped = 0;
        cur = head->next;

        while(cur->next!=end && cur!=end){

            long long file1_size = cur->statbuf.st_size;
            long long file2_size = cur->next->statbuf.st_size;

            if(n*file1_size > n*file2_size){
                swap_trashfile(cur, cur->next);
                swapped = 1;
            }

            cur = cur->next;
        }
        end = cur;
    } while(swapped);
}

// trashfile에 정보를 추가
void trashfile_add(trashFile *head, char *argv[ARGMAX])
{
	trashFile *newfile = (trashFile *)malloc(sizeof(trashFile));
	memset(newfile, 0, sizeof(trashFile));

	struct stat statbuf;
	if(lstat(argv[2], &statbuf)<0){
		fprintf(stderr, "ERROR: lstat error for %s\n", argv[2]);
		return;
	}

	newfile->statbuf = statbuf;
	strcpy(newfile->hash, argv[0]);
	strcpy(newfile->path, argv[1]);
	strcpy(newfile->d_date, argv[3]);
	strcpy(newfile->d_time, argv[4]);
	newfile->next = NULL;

	if(head->next == NULL){
		head->next = newfile;
	}
	else
	{
		trashFile *cur = head->next;

		while(cur->next!=NULL)
			cur = cur->next;

		cur->next = newfile;
	}
}

// trashfilelist에서 삭제
void trashfile_delete(trashFile *head, int n, char *deleted_path)
{
	trashFile *tmp = head;
	trashFile *deleted = head->next;

	while(--n) {
		tmp = deleted;
		deleted = deleted->next;
	}
	
	tmp->next = tmp->next->next;
	strcpy(deleted_path, deleted->path);
	free(deleted);
}

// trashfile에 존재하는 전체 파일 목록들의 정보를 출력한다
void trashfile_print(trashFile *head)
{
	int idx = 1;
	trashFile *cur = head->next;

	if(cur!=NULL)
		printf("      %-100s %-10s %-20s %-20s\n", "FILENAME", "SIZE", "DELETION DATE", "DELETION TIME");

	while(cur!=NULL){
		printf("[%3d] %-100s %-10ld %-20s %-20s\n", idx, cur->path, cur->statbuf.st_size, cur->d_date, cur->d_time);
		cur = cur->next; idx++;
	}
	printf("\n");
}

// trashfile에 존재하는 데이터의 수를 리턴한다
int trashfile_size(trashFile *head)
{
	trashFile *cur = head->next;
	int size = 0;
	
	while (cur != NULL){
		size++;
		cur = cur->next;
	}
	
	return size;
}
