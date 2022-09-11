void swap_fileset(fileList *f1, fileList *f2);
void sort_file_with_size(fileList *head, int n);
void sort_file_with_mode(fileList *head, int n);
void sort_file_with_uid(fileList *head, int n);
void sort_file_with_gid(fileList *head, int n);
void sort_file_with_path(fileList *head, int n);
void sort_fileset_with_uid(fileList *head, int n);
void sort_fileset_with_gid(fileList *head, int n);
void sort_fileset_with_mode(fileList *head, int n);
void sort_fileset_with_path(fileList *head, int n);
void swap_fileInfo(fileInfo *f1, fileInfo *f2);

// filelist에서 f1과 f2를 swap
void swap_fileset(fileList *f1, fileList *f2)
{
	long long filesize;
	char hash[HASHMAX];
	fileInfo *fileInfoList;

	filesize = f1->filesize;
	strcpy(hash, f1->hash);
	fileInfoList = f1->fileInfoList;

	f1->filesize = f2->filesize;
	strcpy(f1->hash, f2->hash);
	f1->fileInfoList = f2->fileInfoList;

	f2->filesize = filesize;
	strcpy(f2->hash, hash);
	f2->fileInfoList = fileInfoList;
}

// sorting 함수의 n이 1인 경우 오름차순, -1인 경우 내림차순으로 정렬
// size를 기준으로 정렬
void sort_file_with_size(fileList *head, int n)
{
	fileList *cur;
	fileList *end = NULL;

	int swapped;

	do
	{
		swapped = 0;
		cur = head->next;

		while(cur->next!=end && cur!=end){

			long long file1_size = cur->filesize;
			long long file2_size = cur->next->filesize;

			if(n*file1_size > n*file2_size){
				swap_fileset(cur, cur->next);
				swapped = 1;
			}

			cur = cur->next;
		}
		end = cur;
	} while(swapped);
}

void sort_fileset_with_uid(fileList *head, int n)
{
	sort_file_with_uid(head, n);
	
	fileList *cur;
	fileList *end = NULL;
	int swapped;

	do
	{
		swapped = 0;
		cur = head->next;

		while(cur->next!=end && cur!=end){

			struct stat statbuf1;
			struct stat statbuf2;

			lstat(cur->fileInfoList->next->path, &statbuf1);
			lstat(cur->next->fileInfoList->next->path, &statbuf2);

			if(n*statbuf1.st_uid > n*statbuf2.st_uid){
				swap_fileset(cur, cur->next);
				swapped = 1;
			}

			cur = cur->next;
		}
		end = cur;
	} while(swapped);
}

// uid를 기준으로 정렬
void sort_file_with_uid(fileList *head, int n)
{
	int set_size = filelist_size(head);
	int list_size;
	fileList *set_cur = head;
	fileInfo *list_cur;
	fileInfo *list_before;

	for(int i=0; i<set_size; i++)
	{
		set_cur = set_cur -> next;
		list_size = fileinfolist_size(set_cur->fileInfoList);

		if(set_cur==NULL) break;

		for(int j=0; j<list_size ; j++)
		{
			list_cur = set_cur -> fileInfoList -> next;
			list_before = set_cur -> fileInfoList;

			if(list_cur==NULL) break;

			for(int k=0; k<list_size-1-j; k++)
			{
				struct stat statbuf1;
				struct stat statbuf2;

				lstat(list_cur->path, &statbuf1);
				lstat(list_cur->next->path, &statbuf2);

				if(n*(statbuf1.st_uid) > n*(statbuf2.st_uid))
					swap_fileInfo(list_cur, list_cur->next);

				list_before = list_cur;
				list_cur = list_cur->next;

				if(list_cur==NULL) break;
			}
		}
	}
}

void sort_fileset_with_gid(fileList *head, int n)
{
	sort_file_with_gid(head, n);

	fileList *cur;
	fileList *end = NULL;
	int swapped;

	do
	{
		swapped = 0;
		cur = head->next;

		while(cur->next!=end && cur!=end){

			struct stat statbuf1;
			struct stat statbuf2;

			lstat(cur->fileInfoList->next->path, &statbuf1);
			lstat(cur->next->fileInfoList->next->path, &statbuf2);

			if(n*statbuf1.st_gid > n*statbuf2.st_gid){
				swap_fileset(cur, cur->next);
				swapped = 1;
			}

			cur = cur->next;
		}
		end = cur;
	} while(swapped);
}

// gid를 기준으로 정렬
void sort_file_with_gid(fileList *head, int n)
{
	int set_size = filelist_size(head);
	int list_size;
	fileList *set_cur = head;
	fileInfo *list_cur;
	fileInfo *list_before;

	for(int i=0; i<set_size; i++)
	{
		set_cur = set_cur -> next;
		list_size = fileinfolist_size(set_cur->fileInfoList);

		if(set_cur==NULL) break;

		for(int j=0; j<list_size ; j++)
		{
			list_cur = set_cur -> fileInfoList -> next;
			list_before = set_cur -> fileInfoList;

			if(list_cur==NULL) break;

			for(int k=0; k<list_size-1-j; k++)
			{
				struct stat statbuf1;
				struct stat statbuf2;

				lstat(list_cur->path, &statbuf1);
				lstat(list_cur->next->path, &statbuf2);

				if(n*(statbuf1.st_gid) > n*(statbuf2.st_gid))
					swap_fileInfo(list_cur, list_cur->next);

				list_before = list_cur;
				list_cur = list_cur->next;

				if(list_cur==NULL) break;
			}
		}
	}
}

void sort_fileset_with_mode(fileList *head, int n)
{
	// 각각의 fileset마다 정렬해준 후에 fileset의 첫 데이터에 따라 정렬
	sort_file_with_mode(head, n);

	fileList *cur;
	fileList *end = NULL;
	int swapped;

	do
	{
		swapped = 0;
		cur = head->next;

		while(cur->next!=end && cur!=end){

			struct stat statbuf1;
			struct stat statbuf2;

			lstat(cur->fileInfoList->next->path, &statbuf1);
			lstat(cur->next->fileInfoList->next->path, &statbuf2);

			if(n*statbuf1.st_mode > n*statbuf2.st_mode){
				swap_fileset(cur, cur->next);
				swapped = 1;
			}

			cur = cur->next;
		}
		end = cur;
	} while(swapped);
}

// mode를 기준으로 정렬
void sort_file_with_mode(fileList *head, int n)
{
	int set_size = filelist_size(head);
	int list_size;
	fileList *set_cur = head;
	fileInfo *list_cur;
	fileInfo *list_before;

	for(int i=0; i<set_size; i++)
	{
		set_cur = set_cur -> next;
		list_size = fileinfolist_size(set_cur->fileInfoList);

		if(set_cur==NULL) break;

		for(int j=0; j<list_size ; j++)
		{
			list_cur = set_cur -> fileInfoList -> next;
			list_before = set_cur -> fileInfoList;

			if(list_cur==NULL) break;

			for(int k=0; k<list_size-1-j; k++)
			{
				struct stat statbuf1;
				struct stat statbuf2;

				lstat(list_cur->path, &statbuf1);
				lstat(list_cur->next->path, &statbuf2);

				if(n*(statbuf1.st_mode) > n*(statbuf2.st_mode))
					swap_fileInfo(list_cur, list_cur->next);

				list_before = list_cur;
				list_cur = list_cur->next;

				if(list_cur==NULL) break;
			}
		}
	}
}

void sort_fileset_with_path(fileList *head, int n)
{
	sort_file_with_path(head, n);

	fileList *cur;
	fileList *end = NULL;
	int swapped;

	do
	{
		swapped = 0;
		cur = head->next;

		while(cur->next!=end && cur!=end){

			if(n*strcmp(cur->fileInfoList->next->path, cur->next->fileInfoList->next->path)>0){
				swap_fileset(cur, cur->next);
				swapped = 1;
			}

			cur = cur->next;
		}
		end = cur;
	} while(swapped);
}

// path를 기준으로 정렬
void sort_file_with_path(fileList *head, int n)
{
	int set_size = filelist_size(head);
	int list_size;
	fileList *set_cur = head;
	fileInfo *list_cur;
	fileInfo *list_before;

	for(int i=0; i<set_size; i++)
	{
		set_cur = set_cur -> next;
		list_size = fileinfolist_size(set_cur->fileInfoList);

		if(set_cur==NULL) break;

		for(int j=0; j<list_size ; j++)
		{
			list_cur = set_cur -> fileInfoList -> next;
			list_before = set_cur -> fileInfoList;

			if(list_cur==NULL) break;

			for(int k=0; k<list_size-1-j; k++)
			{
				if(n*strcmp(list_cur->path, list_cur->next->path)>0)
					swap_fileInfo(list_cur->next, list_cur);

				list_before = list_cur;
				list_cur = list_cur->next;

				if(list_cur==NULL) break;
			}
		}
	}
}

// fileinfo에서 f1과 f2를 swap
void swap_fileInfo(fileInfo *f1, fileInfo *f2)
{
	char path[PATHMAX];
	struct stat statbuf;

	strcpy(path, f1->path);
	statbuf = f1->statbuf;

	strcpy(f1->path, f2->path);
	f1->statbuf = f2->statbuf;

	strcpy(f2->path, path);
	f2->statbuf = statbuf;
}
