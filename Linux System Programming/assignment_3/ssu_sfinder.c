#include "ssu_sfinder.h"
#include "ssu_list.h"
#include "ssu_trash.h"

void fsha1(char *filePath);
void dir_traverse(dirList *dirlist);
void find_duplicates(void);
void remove_no_duplicates(void);
void delete_prompt(void);
int sha1(char *target_path, char *hash_result);
void hash_func(char *path, char *hash);
void ssu_help();
void print_list(char opt[OPTMAX][STRMAX]);
void trash(char opt[OPTMAX][STRMAX]);
void get_trashfilelist(trashFile *head);
void restore(int idx);

int main(void)
{
	get_trash_path();
	get_log_path();

	while(1)
	{
		char input[STRMAX];
		char *argv[ARGMAX];
		char opt[OPTMAX][STRMAX];
		int argc = 0;
		int option = 0;
		int find_error;

		optind = 0;
		opterr = 0;

		printf("20202914> ");
		fgets(input, sizeof(input), stdin);
		input[strlen(input)-1] = '\0';

		argc = get_argv(input, argv);

		if(argc==0) continue;

		if(!strcmp(argv[0], "fsha1"))
		{
			find_error = 0;

			int extension_ok = 0, targetdir_ok = 0;
			minbsize = -2; maxbsize = -2; t_num = 1;
			memset(extension, 0, sizeof(extension));

			while((find_error==0) && (option = getopt(argc, argv, "e:l:h:d:t:"))!=-1)
			{
				switch(option)
				{
					case 'e':
						// argv[1] 인자에 *이 존재하는지 확인, 존재하지 않으면 에러
						if(strchr(optarg, '*')==NULL){
							printf("ERROR: [FILE_EXTENSION] should be '*' or '*.extension\n");
							find_error = 1;
							break;
						}
						// 존재하고, *이 아닌 경우
						if(strcmp(optarg, "*")){
							// 확장자 확인, 잘못된 경우 에러
							if(get_extension(optarg)==NULL){
								printf("ERROR: [FILE_EXTENSION] should be '*' or '*.extension\n");
								find_error = 1;
								break;
							}
							else
								strcpy(extension, get_extension(optarg));

							if(strlen(extension)==0){
								printf("ERROR: There should be extension\n");
								find_error = 1;
								break;
							}
						}

						extension_ok = 1;
						break;

					case 'l':
						minbsize = get_size(optarg);
						// ~가 들어온 경우 mibsize = 0 으로 설정, 에러인 경우 처리
						if(minbsize == -1) minbsize = 0;
						if(minbsize == SIZE_ERROR){
							printf("ERROR: Size wrong -min size : %s\n", opt[1]);
							find_error = 1;
							break;
						}
						break;

					case 'h':
						maxbsize = get_size(optarg);
						// 에러인 경우
						if(maxbsize == SIZE_ERROR){
							printf("ERROR: Size wrong -max size : %s\n", opt[2]);
							find_error = 1;
							break;
						}
						// maxbsize 옵션에 ~가 들어오지 않았는데 maxbsize가 mibsize보다 작은 경우
						if(maxbsize != -1 && minbsize > maxbsize){
							printf("ERROR: [MAXSIZE] should be bigger than [MINSIZE]\n");
							find_error = 1;
							break;
						}
						break;

					case 'd':
						memset(target_dir, 0, sizeof(target_dir));
						// 들어온 주소에 홈 디렉토리 ~가 존재하는 경우
						if(strchr(optarg, '~') != NULL)
							get_path_from_home(optarg, target_dir);
						// 그 외의 경우
						else{
							if(realpath(optarg, target_dir)==NULL){
									printf("ERROR: [TARGET_DIRECTORY] should exist\n");
									find_error = 1;
									break;
							}
						}
						// target_dir이 존재하는지 확인
						if(access(target_dir, F_OK) == -1){
							printf("ERROR: %s directory doesn't exist\n", target_dir);
							find_error = 1;
							break;
						}
						// directory인지 확인
						if(!is_dir(target_dir)){
							printf("ERROR: [TARGET_DIRECTOYRY] should be a directory\n");
							find_error = 1;
							break;
						}

						targetdir_ok = 1;
						break;

					case 't':
						t_num = atoi(optarg);

						if(t_num < 1 || t_num >5){
							printf("ERROR: 1 <= [THREAD_NUM] <= 5\n");
							find_error = 1;
							break;
						}

						break;
						
					default:
						printf("ERROR: This option is not supported\n");
						find_error = 1;
				}
			}

			if(find_error==1) continue;

			if(extension_ok==0 || targetdir_ok==0 || minbsize==-2 || maxbsize==-2){
				printf("ERROR: -e, -l, -h, -d options are necessary!\n");
				continue;
			}

			fsha1(target_dir);
		}
		else if(!strcmp(argv[0], "list")){

			find_error = 0;

			// 기본 설정
			strcpy(opt[0], "fileset");
			strcpy(opt[1], "size");
			strcpy(opt[2], "1");

			while((option = getopt(argc, argv, "l:c:o:"))!=-1)
			{
				switch(option)
				{
					case 'l':
						strcpy(opt[0], optarg);

						if(strcmp(opt[0], "fileset") && strcmp(opt[0], "filelist")){
							printf("ERROR: [LISY_TYPE] - \"fileset\" or \"filelist\"\n");
							find_error = 1;
						}
						break;

					case 'c':
						strcpy(opt[1], optarg);

						if(strcmp(opt[1], "filename") && strcmp(opt[1], "size")
								&& strcmp(opt[1], "uid") && strcmp(opt[1], "gid")
								&& strcmp(opt[1], "mode")){
							printf("ERROR: [CATEGORY] - \"filename\" or \"size\" or \"uid\" or \"gid\" or \"mode\"\n");
							find_error = 1;
						}
						break;

					case 'o':
						strcpy(opt[2], optarg);

						if(strcmp(opt[2], "-1") && strcmp(opt[2], "1")){
							printf("ERROR: [ORDER] - \"-1\" or \"1\"\n");
							find_error = 1;
						}
						break;
					default:
						find_error = 2;
				}
			}

			// 지원하지 않는 옵션이 들어온 경우
			if(find_error == 2) {
				printf("ERROR: use only [l/c/o] options\n");
				continue;
			}

			if(find_error == 0)
				print_list(opt);
		}
		else if(!strcmp(argv[0], "trash")){

			find_error = 0;

			strcpy(opt[0], "filename");
			strcpy(opt[1], "1");

			while((option = getopt(argc, argv, "c:o:"))!=-1)
			{
				switch(option)
				{
					case 'c':
						strcpy(opt[0], optarg);

						if(strcmp(opt[0], "filename") && strcmp(opt[0], "size")
								&& strcmp(opt[0], "date") && strcmp(opt[0], "time")){
							printf("ERROR: [CATEGORY] - \"filename\" or \"size\" or \"date\" or \"time\"\n");
							find_error = 1;
						}
						break;

					case 'o':
						strcpy(opt[1], optarg);

						if(strcmp(opt[1], "-1") && strcmp(opt[1], "1")){
							printf("ERROR: [ORDER] - \"-1\" or \"1\"\n");
							find_error = 1;
						}
						break;

					default:
						find_error = 2;
				}
			}

			// 지원하지 않는 옵션이 들어온 경우
			if(find_error==2){
				printf("ERROR: use only [c/o] options\n");
				continue;
			}

			if(find_error==0)
				trash(opt);
		}
		else if(!strcmp(argv[0], "restore")){

			int r_idx = 0;

			if(trash_list==NULL){
				printf("Trash bin is empty\n");
				continue;
			}

			// 옵션이 들어오지 않은 경우
			if(argv[1]!=NULL){
				r_idx = atoi(argv[1]);
			}
			else {
				printf("ERROR: Please enter an index\n");
				continue;
			}

			if(r_idx < 1 || r_idx > trashfile_size(trash_list)){
				printf("ERROR: Invalid index\n");
				continue;
			}
			
			restore(r_idx);
		}
		else if(!strcmp(argv[0], "exit"))
			break;
		else
			ssu_help();
	}

	printf("Prompt End\n");
	return 0;
}

void fsha1(char *filePath)
{
	char target_dir[PATHMAX];
	dirList *dirlist = (dirList *)malloc(sizeof(dirList));
	dirlist -> next = NULL;
	dups_list_h = (fileList *)malloc(sizeof(fileList));
	dups_list_h -> next = NULL;

	strcpy(target_dir, filePath);
	// 중복 파일을 관리하는 디렉터리를 획득
	get_same_size_files_dir();

	struct timeval begin_t, end_t;
	gettimeofday(&begin_t, NULL);

	dirlist_append(dirlist, target_dir);
	
	// 디렉토리에서 재귀적으로 중복 파일 탐색
	dir_traverse(dirlist);
	// 중복 파일 리스트 생성
	find_duplicates();
	// 중복 파일이 아닌 파일들 삭제
	remove_no_duplicates();

	gettimeofday(&end_t, NULL);

	end_t.tv_sec -= begin_t.tv_sec;
	if(end_t.tv_usec < begin_t.tv_usec){
		end_t.tv_sec--;
		end_t.tv_usec += 1000000;
	}
	end_t.tv_usec -= begin_t.tv_usec;

	// 중복 파일 리스트가 존재하지 않는 경우
	if(dups_list_h->next == NULL)
		printf("No duplicates in %s\n", target_dir);
	else
		print_filelist(dups_list_h);
	
	printf("Searching time: %ld:%06ld(sec:usec)\n\n", end_t.tv_sec, end_t.tv_usec);

	trash_list = (trashFile *)malloc(sizeof(trashFile));
	trash_list -> next = NULL;

	// 파일 삭제 여부를 정하는 prompt 생성
	delete_prompt();
	// trashfilelist를 생성
	get_trashfilelist(trash_list);
}

void dir_traverse(dirList *dirlist)
{
	dirList *cur = dirlist->next;
	dirList *subdirs = (dirList *)malloc(sizeof(dirList));

	memset(subdirs, 0, sizeof(dirList));
	
	while(cur!=NULL)
	{
		struct dirent **namelist;
		int listcnt;
		
		// 현재 path로부터 존재하는 하위 파일들을 받아온다
		listcnt = get_dirlist(cur->dirpath, &namelist);

		for(int i=0; i<listcnt; i++){
			
			char fullpath[PATHMAX] = {0, };
			struct stat statbuf;
			int file_mode;
			long long filesize;

			if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
				continue;

			get_fullpath(cur->dirpath, namelist[i]->d_name, fullpath);

			// 제외
			if(!strcmp(fullpath, "/proc") || !strcmp(fullpath, "/run") || !strcmp(fullpath, "/sys")
					|| !strcmp(fullpath, trash_path) || !strcmp(fullpath, trash_info))
				continue;

			file_mode = get_file_mode(fullpath, &statbuf);

			// filesize가 0인 것은 제외
			if((filesize = (long long)statbuf.st_size)==0)
				continue;

			// DIRECTORY인 경우 subdirs에 추가한다
			if(file_mode == DIRECTORY)
				dirlist_append(subdirs, fullpath);
			
			// REGFILE인 경우 
			else if(file_mode == REGFILE){

				// filesize에 따라 탐색
				if((filesize < minbsize) || (maxbsize!=-1 && filesize>maxbsize))
					continue;

				FILE *fp;
				char filename[PATHMAX*2];
				char *path_extension;
				char hash[HASHMAX];

				// 중복 파일을 관리하는 디렉토리에 filesize 이름으로 파일을 생성하기 위한 filename
				sprintf(filename, "%s/%lld", same_size_files_dir, filesize);

				memset(hash, 0, HASHMAX);
				hash_func(fullpath, hash);

				path_extension = get_extension(fullpath);

				if(strlen(extension)!=0){
					if(path_extension==NULL||strcmp(extension, path_extension))
						continue;
				}

				if((fp=fopen(filename, "a"))==NULL){
					printf("ERROR: fopen error for %s\n", filename);
					continue;
				}

				// hash와 fullpath 기록
				fprintf(fp, "%s %s\n", hash, fullpath);
				fclose(fp);
			}
		}

		cur = cur -> next;
	}

	// dirlist 삭제
	dirlist_delete_all(dirlist);

	// subdirs에 존재하는 디렉터리가 있으면 탐색 시작
	if(subdirs->next != NULL)
		dir_traverse(subdirs);
}

// 중복 파일 리스트 생성
void find_duplicates(void)
{
	struct dirent **namelist;
	int listcnt;
	char hash[HASHMAX];
	FILE *fp;

	listcnt = get_dirlist(same_size_files_dir, &namelist);

	for(int i=0; i<listcnt; i++){
		char filename[PATHMAX*2];
		long long filesize;
		char filepath[PATHMAX];
		char hash[HASHMAX];
		char line[STRMAX];

		if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
			continue;

		// 탐색한 파일 목록 가져오기
		filesize = atoll(namelist[i]->d_name);
		sprintf(filename, "%s/%s", same_size_files_dir, namelist[i]->d_name);

		if((fp=fopen(filename, "r"))==NULL){
			printf("ERROR: fopen error for %s\n", filename);
			continue;
		}

		while(fgets(line, sizeof(line), fp)!=NULL)
		{	
			int idx;

			strncpy(hash, line, HASHMAX);
			hash[HASHMAX-1] = '\0';

			strcpy(filepath, line+HASHMAX);
			filepath[strlen(filepath)-1] = '\0';

			// filelist에 hash와 동일한 값을 가지는 file이 존재하지 않는 경우
			if((idx=filelist_search(dups_list_h, hash))==0)
				filelist_append(dups_list_h, filesize, filepath, hash);

			// 존재하는 경우 filelist의 fileInfoList에 filepath에 대한 정보를 저장
			else{
				fileList *filelist_cur = dups_list_h;

				while(idx--){
					filelist_cur = filelist_cur->next;
				}
				fileinfo_append(filelist_cur->fileInfoList, filepath);
			}
		}
		fclose(fp);
	}
}

// 중복 파일이 1개 이하인 경우 삭제
void remove_no_duplicates(void)
{
	fileList *filelist_cur = dups_list_h -> next;

	while(filelist_cur != NULL){

		fileInfo *fileinfo_cur = filelist_cur -> fileInfoList;

		if(fileinfolist_size(fileinfo_cur)<2)
			filelist_delete_node(dups_list_h, filelist_cur->hash);

		filelist_cur = filelist_cur->next;
	}
}

void delete_prompt(void)
{
	while(filelist_size(dups_list_h)>0)
	{
		FILE *fp;
		char filename[PATHMAX*2];
		char input[STRMAX];
		char last_filepath[PATHMAX];
		char modifiedtime[STRMAX];
		char *argv[ARGMAX];
		int argc = 0;
		int tmp_num, set_idx, list_idx;
		int opt, option, check_err;
		time_t mtime = 0;
		fileList *target_filelist_p;
		fileInfo *target_infolist_p;

		optind = 0;
		opterr = 0;

		printf(">> ");
		fgets(input, sizeof(input), stdin);
		input[strlen(input)-1] = '\0';

		argc = get_argv(input, argv);

		if(argc==0) continue;

		if(!strcmp(argv[0], "exit")){
			printf(">> Back to Prompt\n");
			break;
		}

		if(!strcmp(argv[0], "delete")){

			opt = 0;
			check_err = 0;
			set_idx = -1;
			list_idx = -1;

			while((option = getopt(argc, argv, "l:d:ift"))!=-1)
			{
				switch(option)
				{
					case 'l':
						set_idx = atoi(optarg);
						break;

					case 'd':
						list_idx = atoi(optarg);
						if(opt==0) opt = 'd';
						else {
							check_err = 1;
							break;
						}
						break;

					case 'i':
						if(opt==0) opt = 'i';
						else {
							check_err = 1;
							break;
						}
						break;

					case 'f':
						if(opt==0) opt = 'f';
						else {
							check_err = 1;
							break;
						}
						break;

					case 't':
						if(opt==0) opt = 't';
						else {
							check_err = 1;
							break;
						}
						break;

					default:
						check_err = 2;
				}
			}

			// 에러 처리
			if(check_err==1){
				printf("ERROR: use l option only plus one [d/i/f/t] option\n");
				continue;
			}

			if(check_err==2){
				printf("ERROR: use only [l/d/i/f/t] options\n");
				continue;
			}

			if(opt==0){
				printf("ERROR: use [d/i/f/t] option\n");
				continue;
			}

			// set_idx 입력 에러
			if(set_idx==-1 || filelist_size(dups_list_h)<set_idx || set_idx<1){
				printf("ERROR: set_idx error\n");
				continue;
			}

			target_filelist_p = dups_list_h -> next;
			tmp_num = set_idx;

			// 지정한 set_idx 찾기
			while(--tmp_num){
				if(target_filelist_p->next != NULL)
					target_filelist_p = target_filelist_p->next;
				else {
					printf("ERROR: set_idx that does not exist\n");
					continue;
				}
			}

			target_infolist_p = target_filelist_p->fileInfoList;
			
			// list_idx 입력 에러
			if(list_idx!=-1 && (fileinfolist_size(target_infolist_p)<list_idx || list_idx<1)){
				printf("ERROR: list_idx error\n");
				continue;
			}

			// 가장 최근에 수정된 파일, 수정시간 가져오기
			mtime = get_recent_mtime(target_infolist_p, last_filepath);
			get_time(localtime(&mtime), modifiedtime);

			if(opt=='d')
			{
				fileInfo *deleted;
				tmp_num = list_idx;

				deleted = target_infolist_p -> next;
				
				// 지정한 list_idx 찾기
				while(--tmp_num){
					if(deleted->next != NULL)
						deleted = deleted->next;
					else {
						printf("ERROR: list_idx that does not exist\n");
						continue;
					}
				}

				//삭제
				remove(deleted->path);
				log_msg(1, deleted->path);
				fileinfo_delete_node(target_infolist_p, deleted->path);
				printf("\"%s\" has been deleted in #%d\n\n", deleted->path, list_idx);

                // 중복 파일이 2개 미만인 경우 중복 파일 리스트에서 삭제
                if(fileinfolist_size(target_infolist_p)<2)
                    filelist_delete_node(dups_list_h, target_filelist_p->hash);
			}
			// 선택한 세트의 중복 파일 리스트에 있는 파일의 절대 경로를 보여주며 삭제 여부 확인
			else if(opt=='i')
			{
				char ans[STRMAX];
				fileInfo *cur = target_infolist_p->next;
				fileInfo *deleted_list = (fileInfo *)malloc(sizeof(fileInfo));
				fileInfo *tmp;
				int listcnt = fileinfolist_size(target_infolist_p);

				while(cur!=NULL && listcnt--){
					printf("Delete \"%s\"? [y/n] ", cur->path);
					memset(ans, 0, sizeof(ans));
					fgets(ans, sizeof(ans), stdin);

					// 삭제
					if(!strcmp(ans, "y\n") || !strcmp(ans, "Y\n")){
						remove(cur->path);
						log_msg(1, cur->path);
						cur = fileinfo_delete_node(target_infolist_p, cur->path);
					}
					else if(!strcmp(ans, "n\n") || !strcmp(ans, "N\n"))
						cur = cur->next;
					else{
						printf("ERROR: Answer should be 'y/Y' or 'n/N'\n");
						break;
					}
				}
				printf("\n");

				// 중복 파일이 2개 미만인 경우 중복 파일 리스트에서 삭제
				if(fileinfolist_size(target_infolist_p)<2)
					filelist_delete_node(dups_list_h, target_filelist_p->hash);
			}
			// 가장 최근에 수정한 파일을 남겨두고 나머지 중복 파일 삭제
			else if(opt=='f'){

				fileInfo *tmp;
				fileInfo *deleted = target_infolist_p->next;

				while(deleted!=NULL){
					tmp = deleted->next;

					if(!strcmp(deleted->path, last_filepath)){
						deleted = tmp;
						continue;
					}

					remove(deleted->path);
					log_msg(1, deleted->path);
					free(deleted);
					deleted = tmp;
				}

				filelist_delete_node(dups_list_h, target_filelist_p->hash);
				printf("Left file in #%d : %s (%s)\n\n", set_idx, last_filepath, modifiedtime);
			}
			// 가장 최근에 수정한 파일을 남겨두고 나머지 중복 파일 휴지통 이동
			else if(opt=='t'){

				fileInfo *tmp;
				fileInfo *deleted = target_infolist_p->next;
				char _trash[PATHMAX];
				char filename[PATHMAX];

				while(deleted!=NULL)
				{
					tmp=deleted->next;

					if(!strcmp(deleted->path, last_filepath)){
						deleted = tmp;
						continue;
					}

					memset(_trash, 0, sizeof(_trash));
					memset(filename, 0, sizeof(filename));

					// 휴지통으로 보내기 위한 경로
					sprintf(_trash, "%s/%s", trash_path, strrchr(deleted->path, '/')+1);

					// 동일한 이름의 파일이 휴지통에 존재하는 경우
					if(access(_trash, F_OK)==0){
						get_new_file_name(trash_path, deleted->path, filename);
						strncpy(strrchr(_trash, '/')+1, filename, strlen(filename));
					}
					else
						strcpy(filename, strrchr(deleted->path, '/')+1);

	                // 휴지통으로 보낸 파일을 관리
					sprintf(filename, "%s/%lld", trash_info, target_filelist_p->filesize);
	
	                if((fp=fopen(filename, "a"))==NULL){
	                        printf("ERROR: fopen error for %s\n", filename);
	                        return;
	                }

					// 휴지통으로 이동
					if(rename(deleted->path, _trash)==-1){
						printf("ERROR: Fail to move duplicates to Trash\n");
						break;
					}

					// 삭제 파일에 대한 정보 기록
					fprintf(fp, "%s %s %s %s\n",
							target_filelist_p->hash, deleted->path, _trash, log_msg(0, deleted->path));
					fclose(fp);
					free(deleted);
					deleted = tmp;
				}

				// hash 값을 가지는 파일 리스트 삭제
				filelist_delete_node(dups_list_h, target_filelist_p->hash);
				printf("All files in #%d have moved to Trash except \"%s\" (%s)\n\n", set_idx, last_filepath, modifiedtime);
			}

			if(dups_list_h->next == NULL) break;
			else print_filelist(dups_list_h);
		}
	}
}

int sha1(char *target_path, char *hash_result)
{
	FILE *fp;
	unsigned char hash[SHA_DIGEST_LENGTH];
	unsigned char buffer[SHRT_MAX];
	int bytes = 0;
	SHA_CTX sha1;

	if((fp=fopen(target_path, "rb"))==NULL){
		printf("ERROR: fopen error for %s\n", target_path);
		return 1;
	}

	SHA1_Init(&sha1);

	while((bytes=fread(buffer, 1, SHRT_MAX, fp))!=0)
		SHA1_Update(&sha1, buffer, bytes);

	SHA1_Final(hash, &sha1);

	for(int i=0; i<SHA_DIGEST_LENGTH; i++)
		sprintf(hash_result + (i*2), "%02x", hash[i]);
	hash_result[HASHMAX-1] = 0;

	fclose(fp);
	return 0;
}

void hash_func(char *path, char *hash)
{
	sha1(path, hash);
}

void ssu_help()
{
	printf("Usage:\n");
	printf(" > fsha1 -e [FILE_EXTENSION] -l [MINSIZE] -h [MAXSIZE] -d [TARGET_DIRECTORY] -t [THREAD_NUM]\n");
	printf("     >> delete -l [SET_INDEX] -d [OPTARG] -i -f -t\n");
	printf(" > trash -c [CATEGORY] -o [ORDER]\n");
	printf(" > restore [RESTORE_INDEX]\n");
	printf(" > help\n");
	printf(" > exit\n");
	printf("\n");
}

void print_list(char opt[OPTMAX][STRMAX])
{
	int n = atoi(opt[2]);

	if(dups_list_h==NULL){
		printf("List is empty\n");
		return;
	}
	
	if(!strcmp(opt[0], "fileset")){
		if(!strcmp(opt[1], "size")) sort_file_with_size(dups_list_h, n);
		else if(!strcmp(opt[1], "uid")) sort_fileset_with_uid(dups_list_h, n);
		else if(!strcmp(opt[1], "gid")) sort_fileset_with_gid(dups_list_h, n);
		else if(!strcmp(opt[1], "mode")) sort_fileset_with_mode(dups_list_h, n);
		else if(!strcmp(opt[1], "filename")) sort_fileset_with_path(dups_list_h, n);
	}
	else if(!strcmp(opt[0], "filelist")){
		if(!strcmp(opt[1], "size")) sort_file_with_size(dups_list_h, n);
		else if(!strcmp(opt[1], "uid")) sort_file_with_uid(dups_list_h, n);
		else if(!strcmp(opt[1], "gid")) sort_file_with_gid(dups_list_h, n);
		else if(!strcmp(opt[1], "mode")) sort_file_with_mode(dups_list_h, n);
		else if(!strcmp(opt[1], "filename")) sort_file_with_path(dups_list_h, n);
	}

	print_filelist(dups_list_h);
}

void trash(char opt[OPTMAX][STRMAX])
{
	if(trash_list==NULL || trash_list->next == NULL) {
		printf("Trash bin is empty\n");
		return;
	}

	if(opt==NULL){
		trashfile_print(trash_list);
		return;
	}

	int n = atoi(opt[1]);

	if(!strcmp(opt[0], "filename")) sort_trashfile_with_path(trash_list, n);
	else if(!strcmp(opt[0], "size")) sort_trashfile_with_size(trash_list, n);
	else if(!strcmp(opt[0], "date")) sort_trashfile_with_date(trash_list, n);
	else if(!strcmp(opt[0], "time")) sort_trashfile_with_time(trash_list, n);

	// trashfile list 출력
	trashfile_print(trash_list);
}

void get_trashfilelist(trashFile *head)
{
	struct dirent **namelist;
	int listcnt;
	FILE *fp;

	listcnt = get_dirlist(trash_info, &namelist);

	for(int i=0; i<listcnt; i++)
	{
		char line[STRMAX];
		char filename[PATHMAX*2];
		long long filesize;

		int argc = 0;
		char *argv[ARGMAX];

		if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
			continue;

		filesize = atoll(namelist[i]->d_name);
		sprintf(filename, "%s/%s", trash_info, namelist[i]->d_name);

		if((fp=fopen(filename, "r"))==NULL){
			printf("ERROR: fopen error for %s\n", filename);
			continue;
		}

		while(fgets(line, sizeof(line), fp)!=NULL){

			if(line[0] == '*' || line[HASHMAX] == '*' || line[0]==NULL) continue;

			line[strlen(line)-1] = '\0';
			argc = get_argv(line, argv);
			
			if(argc!=5) {
				printf("ERROR: 잘못된 파일 정보입니다.\n");
				break;
			}

			trashfile_add(trash_list, argv);
		}

		fclose(fp);
	}
}

void restore(int idx)
{
	char tt_path[PATHMAX];
	char restored_path[PATHMAX];
	char restored_dir[PATHMAX];

	//trashfile list에서 제거 
	trashfile_delete(trash_list, idx, restored_path);

	// 복원된 파일 삭제 파일 데이터에서 제거
	remove_fileinfo(restored_path, tt_path);

	char filename[PATHMAX];

	// 파일이 복원될 디렉터리의 경로
	strcpy(restored_dir, restored_path);
	get_restored_path(restored_dir);

	if(access(restored_path, F_OK)==0){
    	get_new_file_name(restored_dir, restored_path, filename);
	   	strncpy(strrchr(restored_path, '/')+1, filename, strlen(filename));
    }
    else
        strcpy(filename, strrchr(restored_path, '/')+1);

	sprintf(restored_path, "%s/%s", restored_dir, filename);

	//파일 복원하기
	if(rename(tt_path, restored_path)==-1){
		printf("ERROR: Fail to restore file\n");
		return;
	}

	printf("[RESTORE] success for %s\n", restored_path);
	// 로그 작성
	log_msg(2, restored_path);
	
	if(trash_list->next==NULL) return;

	trash(NULL);
}

void get_restored_path(char *path)
{
	char *ptr = strrchr(path, '/');
	
	if(ptr!=NULL)
	{
		*ptr = '\0';
	}
}

void remove_fileinfo(char *restored_path, char *tt_path)
{
	// 파일 레코드에서 복원 파일 제외
	struct dirent **namelist;
	int listcnt;
	FILE *fp;

	listcnt = get_dirlist(trash_info, &namelist);

	for(int i=0; i<listcnt; i++)
	{
		char line[STRMAX];
		char filename[PATHMAX*2];
		long long filesize;

		int cur=0;
		int argc = 0;
		char *argv[ARGMAX];

		if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
			continue;

		filesize = atoll(namelist[i]->d_name);
		sprintf(filename, "%s/%s", trash_info, namelist[i]->d_name);

		if((fp=fopen(filename, "r+"))==NULL){
			printf("ERROR: fopen error for %s\n", filename);
			continue;
		}

		while(fgets(line, sizeof(line), fp)!=NULL){

			line[strlen(line)-1] = '\0';
			argc = get_argv(line, argv);
	
			if(argc!=5) {
				printf("ERROR: 잘못된 파일 정보입니다.\n");
				break;
			}

			// 삭제된 파일의 레코드 앞에 * 표시
			if(!strcmp(argv[1], restored_path)){
				strcpy(tt_path, argv[2]);
				fseek(fp, cur, SEEK_SET);
				fputc('*', fp);
				fseek(fp, cur+HASHMAX, SEEK_SET);
				fputc('*', fp);
				fclose(fp);
				return;
			}

			cur = ftell(fp);
		}
		fclose(fp);
	}
}
