#include "reservation.h"

void user_mode();
void manager_mode();
// 지점 관리 함수
void cafeManage();
// 지점 추가, 수정, 삭제
void addCafe();
void editCafeInfo();
void deleteCafe();
// 스터디룸 추가, 수정, 삭제 선택
void editRoomInfo(char *dirPath);
// 스터디룸 추가, 수정, 삭제
void addStudyRoom(char *dirPath);
void editStudyRoom(char *dirPath);
void deleteStudyRoom(char *dirPath);

bool check_ID(char *userID);
void user_mode();

// 지점과 룸이 존재하는지 확인
int isExistCafe(int num, char *path);
int isExistRoom(int num, char *dirPath, char *path);

// 스터디룸 조회, 예약, 수정
void print_file(char *dir);
void print_room(char *fullpath);
void study_room_check();

void input_reservation(char *usrID, Data *data, int mode);
void new_reservation(char *usrID);
void set_new_reservation_data(char *path, Data data);

void print_reservation(char *usrID);

void modify_reservation(char *usrID);

bool checkDate(char *date);
bool isTimeEmpty(char *roomPath, char *date, int start, int time);
bool isAcceptable(char *dirpath, int number);

void changeInfo(char *path, int start, int use);

int main(void)
{
	getDataDir();

	int mode;

	while(1){

		printf("\n---모드를 선택해주세요---\n");
		printf("0. 프로그램 종료\n");
		printf("1. 관리자 모드\n");
		printf("2. 사용자 모드\n");
		mode = getInput(">> ");
	
		if(mode==0){
			printf("\n프로그램을 종료합니다...\n");
			exit(0);
		}

		// 관리자 모드 프로그램 실행
		if(mode==1){
			manager_mode();
		}
		// 사용자 모드 프로그램 실행
		else if(mode==2){
			user_mode();
		}
		else{
			printf("잘못된 입력입니다.\n");
			continue;
		}
	}

	return 0;
}

void manager_mode()
{
	int mode, number, roomNumber;

	while(1){

		printf("\n---관리자 모드---\n");
		printf("0. 프로그램 종료\n");
		printf("1. 지점 관리\n");
		mode = getInput(">> ");

		if(mode==0) return;
		else if(mode==1) cafeManage();
		else{
			printf("잘못된 입력입니다.\n");
			continue;
		}

		break;
	}
}

void cafeManage()
{
	int option;

	while(1){

		printf("\n---지점 관리---\n");
		printf("0. 프로그램 종료\n");
		printf("1. 지점 추가\n");
		printf("2. 지점 정보 수정\n");
		printf("3. 지점 삭제\n");
		option = getInput(">> ");

		if(option==0) return;
		else if(option==1) addCafe();
		else if(option==2) editCafeInfo();
		else if(option==3) deleteCafe();
		else{
			printf("잘못된 입력입니다.\n");
			continue;
		}

		break;
	}
}

// 존재하지 않는 경우 1, 존재하는 경우 0을 리턴
int isExistCafe(int num, char *path)
{
	if(ManagerPath[strlen(ManagerPath)-1]=='/')
		sprintf(path, "%s%d", ManagerPath, num);
	else
		sprintf(path, "%s/%d", ManagerPath, num);

	// 존재하지 않는 경우
	if(access(path, F_OK)!=0)
		return 1;
	// 존재하는 경우
	else
		return 0;
}

void addCafe()
{
	char path[PATH_MAX];
	int num, option;

	while(1){

		num = getInput("추가할 지점 번호를 입력해주세요(1~6), 취소를 원할 시 0을 입력 >> ");

		if(num==0) return;
		if(num<1 || num>6) {
			printf("잘못된 입력입니다.\n");
			continue;
		}

  	 	option = isExistCafe(num, path);

		if(option==1){
			mkdir(path, 0755);
			printf("%d번 지점이 추가되었습니다.\n", num);
		}
		else if(option==0){
			printf("이미 존재하는 지점 번호입니다.\n");
			continue;
		}

		break;
	}
}

void editCafeInfo()
{
	char path[PATH_MAX];
	int num, option;

	while(1){

		num = getInput("수정할 지점 번호를 입력해주세요, 취소를 원할 시 0을 입력 >> ");

		if(num==0) return;

   	 	option = isExistCafe(num, path);

		if(option==1){
			printf("존재하지 않는 지점 번호입니다.\n");
			continue;
		}
		else if(option==0){
			editRoomInfo(path);
		}

		break;
	}
}

void deleteCafe()
{
	char path[PATH_MAX];
	int num, option;

	while(1){

		num = getInput("삭제할 지점 번호를 입력해주세요(1~6), 취소를 원할 시 0을 입력 >> ");

		if(num==0) return;

		option = isExistCafe(num, path);

		if(option==1){
			printf("존재하지 않는 지점 번호입니다.\n");
			continue;
		}
		else if(option==0){
			remove_file(path);
			rmdir(path);
			printf("%d번 지점이 삭제되었습니다.\n", num);
		}

		break;
	}
}

void editRoomInfo(char *dirPath)
{
	int option;

	while(1){

		printf("\n---스터디 공간 관리---\n");
		printf("0. 프로그램 종료\n");
		printf("1. 스터디 공간 추가\n");
		printf("2. 스터디 공간 수정\n");
		printf("3. 스터디 공간 삭제\n");
	   	option = getInput(">> ");

		if(option==0) return;
		else if(option==1) addStudyRoom(dirPath);
		else if(option==2) editStudyRoom(dirPath);
		else if(option==3) deleteStudyRoom(dirPath);
		else {
			printf("잘못된 입력입니다.\n");
			continue;
		}

		break;
	}
}

int isExistRoom(int num, char *dirPath, char *path)
{
	if(num==0) return -1;
	if(num<1 || num>5){
		printf("잘못된 입력입니다.\n");
		return 2;
	}

	if(dirPath[strlen(dirPath)-1]=='/')
		sprintf(path, "%s%d", dirPath, num);
	else
		sprintf(path, "%s/%d", dirPath, num);

	if(access(path, F_OK)!=0)
		return 1;
	else
		return 0;
}

void addStudyRoom(char *dirPath)
{
	char path[PATH_MAX];
	int num, option;

	while(1){
        num = getInput("추가할 스터디룸의 번호를 입력하세요(1~5), 취소를 원할 시 0을 입력 >> ");

		if(num==0) return;
		if(num<1 || num>5){
			printf("잘못된 입력입니다.\n");   
			continue;
		}
		
		option = isExistRoom(num, dirPath, path);

		if(option==1){

			// 룸 관리 폴더 생성
			mkdir(path, 0755);

			// 이름, 위치하는 층, 책상 개수, 보유 컴퓨터 개수, 허용 인원 순으로 기록
			Roominfo addRoom;
			char roomName[10];
			int fd, maxNumber;

			sprintf(roomName, "%s%d", "room", num);
			strcpy(addRoom.roomName, roomName);
			addRoom.floor = 1;
			addRoom.desk = 10;
			addRoom.computer = 3;

			while(1){

				addRoom.people = getInput("스터디 공간의 허용 인원을 입력하세요(1~10), 취소를 원할 시 0을 입력 >> ");

				if(addRoom.people == 0) {
					rmdir(path);
					return;
				}

				if(addRoom.people<1 || addRoom.people>10){
					printf("잘못된 입력입니다.\n");
					continue;
				}
				
				break;
			}

			char filename[PATH_MAX];
			getPath(dirPath, ROOMINFO, filename);

			if((fd=open(filename, O_CREAT|O_WRONLY|O_APPEND, 0644))<0){
				fprintf(stderr, "open error for %s\n", path);
				break;
			}

			if(write(fd, (char*)&addRoom, sizeof(addRoom))<0){
				fprintf(stderr, "write error\n");
				break;
			}

			printf("%d번 스터디룸이 추가되었습니다.\n", num);
			close(fd);
		}
		else if(option==0){
			printf("이미 존재하는 룸입니다.\n");
			continue;
		}
		else
			continue;
		
		break;
	}
}

void editStudyRoom(char *dirPath)
{
	while(1){

		char path[PATH_MAX];
		int num, option;

	    num = getInput("수정할 스터디룸의 번호를 입력하세요, 취소를 원할 시 0을 입력 >> ");

       	if(num==0) return;

		option = isExistRoom(num, dirPath, path);

		if(option==1){
			printf("존재하지 않는 스터디룸입니다.\n");
			continue;
		}
		else if(option==0){

			int fd;

			// 이름, 위치하는 층, 책상 개수, 보유 컴퓨터 개수, 허용 인원 기록
			int maxNumber;
			char roomInfo[INPUT_MAX];
			char roomName[10];
			sprintf(roomName, "%s%d", "공간", num);

			while(1){

				maxNumber = getInput("스터디 공간의 허용 인원을 입력하세요(1~10), 취소를 원할 시 0을 입력 >> ");

				if(maxNumber == 0)
					return;

				if(maxNumber<1 || maxNumber>10){
					printf("잘못된 입력입니다.\n");
					continue;
				}
				
				break;
			}

			if((fd=open(path, O_WRONLY))<0){
				fprintf(stderr, "open error for %s\n", path);
				break;
			}

			sprintf(roomInfo, "%-10s %-5d %-5d %-6d %-5d\n", roomName, 1, 10, 3, maxNumber);

			if(write(fd, (char*)&roomInfo, strlen(roomInfo))<0){
				fprintf(stderr, "write error\n");
				break;
			}

			printf("%d번 스터디룸이 수정되었습니다.\n", num);
			close(fd);
		}
		break;
	}
}

void deleteStudyRoom(char *dirPath)
{
	while(1){

		char path[PATH_MAX];
		int num, option;

		num = getInput("삭제할 스터디룸 번호를 입력하세요, 취소를 원할 시 0을 입력 >> ");
		
		if(num==0) return;

        option = isExistRoom(num, dirPath, path);

		if(option==1){
			printf("존재하지 않는 스터디룸입니다.\n\n");
			continue;
		}
		else if(option==0){
			remove_file(path);
	        rmdir(path);
			printf("%d번 스터디룸이 삭제되었습니다.\n", num);

			int fd, n;
			char filename[10];
			Roominfo cur;

			getPath(dirPath, ROOMINFO, path);
			sprintf(filename, "%s%d", "room", num);

			if((fd=open(path, O_RDWR))<0){
				fprintf(stderr, "open error for %s\n", path);
				exit(1);
			}

			while((n=read(fd, (char*)&cur, sizeof(Roominfo)))>0){
				if(!strcmp(cur.roomName, filename)){
					strcpy(cur.roomName, "empty");
					lseek(fd, -n, SEEK_CUR);
					write(fd, (char*)&cur, n);


					break;
				}
			}

			close(fd);
		}
		else
			continue;

		break;
	}
}

bool check_ID(char *userID)
{
	int size = strlen(userID);

	if(!(size<=ID_MAX && size>=ID_MIN)){
		printf("잘못된 아이디 길이입니다.\n");
		return false;
	}

	for(int i=0; i<size; i++){
		
		char c = userID[i];

		if(!(('A'<=c && c<='Z')||('a'<=c && c<='z')||('0'<=c && c<='9'))){
			printf("잘못된 아이디 형식입니다.\n");
			return false;
		}
	}

	return true;
}

void user_mode()
{
	int option;
	char userID[INPUT_MAX];

	while(1){

		printf("\n사용자 아이디를 입력해주세요.\n");
		printf("(영문자 및 숫자 조합 최소 5글자, 최대 10글자)\n");
		printf("(취소를 원하는 경우 0 입력)\n");
		printf(">> ");
		fgets(userID, INPUT_MAX, stdin);
		userID[strlen(userID)-1] = '\0';

		if(userID[0]=='\0') continue;

		if(!strcmp(userID, "0"))
			return;

		if(!check_ID(userID)) continue;
		else break;
	}

	while(1){

		printf("\n---사용자 모드---\n");
		printf("0. 프로그램 종료\n");
		printf("1. 스터디 공간 조회\n");
		printf("2. 신규 예약\n");
		printf("3. 예약 조회\n");
		printf("4. 예약 수정\n");
		option = getInput(">> ");

		if(option==0) return;

		if(option==1){
			study_room_check();
		}
		else if(option==2){
			new_reservation(userID);
		}
		else if(option==3){
			print_reservation(userID);
		}
		else if(option==4){
			modify_reservation(userID);
		}
		else{
			printf("잘못된 옵션입니다.\n");
			continue;
		}

		break;
	}
}

void print_file(char *dir)
{
	char fullpath[PATH_MAX] = {0, };
	getPath(dir, "roominfo", fullpath);

	printf("\n--- 스터디룸 조회 ---\n");
	printf("%-10s %-7s %-7s %-7s %-7s\n", "이름", "층수", "책상", "컴퓨터", "허용인원");
	print_room(fullpath);
}

void print_room(char *fullpath)
{
	struct roominfo tmp;
	int fd, n;
	char buf[INPUT_MAX];

	if((fd=open(fullpath, O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", fullpath);
		return;
	}

	while((n=read(fd, (char *)&tmp, sizeof(struct roominfo)))>0){
		
		if(!strcmp(tmp.roomName, "empty")) continue;

		printf("%-8s %-5d %-5d %-6d %-5d\n", tmp.roomName, tmp.floor, tmp.desk, tmp.computer, tmp.people);
	}

	close(fd);
}

void study_room_check()
{
	while(1){

		int num, option;
		char path[PATH_MAX];

		num = getInput("조회를 원하는 지점의 번호를 입력해주세요 >> ");
		
		if(num==0) return;

		option = isExistCafe(num, path);

		if(option==1){
			printf("존재하지 않는 지점입니다.\n");
			continue;
		}
		else if(option==0){
			print_file(path);
		}
		else
			continue;

		break;
	}
}

bool checkDate(char *date)
{
	int d1, d2;
	time_t timer;
	struct tm* t;

	timer = time(NULL);
	t = localtime(&timer);

	t->tm_year -= 100;
	t->tm_mon += 1;

	d1 = atoi(date);
	d2 = t->tm_year*10000 + t->tm_mon*100 + t->tm_mday;

	if(d1>d2) {
		return true;
	}
	else {
		printf("올바른 날짜를 입력해주세요.\n");
		printf("(%d 이후로 가능)\n", d2);
	   	return false;
	}
}

bool isTimeEmpty(char *roomPath, char *date, int start, int time)
{
	char path[PATH_MAX];
	char buf[INPUT_MAX];
	FILE *fp;

	if(start<8 || start>22 || start+time>22) return false;

	getPath(roomPath, date, path);

	// 다른 예약건이 해당 날짜에 존재하는 경우
	if(access(path, F_OK)==0){

		if((fp = fopen(path, "r"))==NULL){
			fprintf(stderr, "fopen error for %s\n", path);
			return false;
		}
	
		fgets(buf, INPUT_MAX, fp);
	
		for(int i=start; i<start+time; i++){
			if(buf[i-8]=='X')
				return false;
		}
	}
	// 예약건이 존재하지 않는 경우
	else
		return true;

	fclose(fp);
	return true;
}

void changeInfo(char *path, int start, int use)
{
	int fd;
	char infoPath[PATH_MAX];
	char check[20];

	if(access(path, F_OK)==0){

		if((fd = open(path, O_RDWR))<0){
			fprintf(stderr, "open error for %s\n", path);
			exit(1);
		}

		read(fd, check, 15);

	}
	else{
	
		if((fd = open(path, O_WRONLY|O_CREAT, 0644))<0){
			fprintf(stderr, "open error for %s\n", path);
			exit(1);
		}

		strcpy(check, "OOOOOOOOOOOOOOO");
	}

	for(int i=start; i<start+use; i++)
		check[i-8] = 'X';

	write(fd, check, 15);
	close(fd);
}

bool isAcceptable(char *dirpath, int number)
{
	Roominfo cur;
	int fd, max;
	char path[PATH_MAX];

	getPath(dirpath, ROOMINFO, path);

	if((fd=open(path, O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", path);
		return false;
	}

	for(int i=0; i<number; i++)
		read(fd, (char *)&cur, sizeof(Roominfo));

	max = cur.people;

	if(number>max || number<1){
		printf("최대 수용 가능 인원수는 %d명 입니다.\n", max);
		return false;
	}

	close(fd);
	return true;
}

void new_reservation(char *usrID)
{
	// 저장할 데이터
	Data data;
	strcpy(data.userID, usrID);
	strcpy(data.name, "홍길동");
	strcpy(data.phone, "010-1234-5678");

	input_reservation(usrID, &data, 1);
}

void set_new_reservation_data(char *path, Data data)
{
	int fd;

	if(access(path, F_OK)==0){

		if((fd=open(path, O_WRONLY|O_APPEND))<0){
			fprintf(stderr, "open error for %s\n", path);
			exit(1);
		}
	}
	else{

		if((fd=open(path, O_WRONLY|O_CREAT, 0644))<0){
			fprintf(stderr, "open error for %s\n", path);
			exit(1);
		}
	}

	write(fd, (char*)&data, sizeof(data));
	close(fd);
}

void print_reservation(char *usrID)
{
	Data cur;
	int idx=0, start, end;
	char path[PATH_MAX];
	getPath(UserPath, usrID, path);

	if(access(path, F_OK)==0){

		int fd, n;

		printf("\n--- %s 예약 조회 ---\n", usrID);

		if((fd=open(path, O_RDONLY))<0){
			fprintf(stderr, "open() error for %s\n", path);
			exit(1);
		}

		printf("%-4s %-8s %-8s %-8s %-8s %-6s %-8s\n",
				"번호", "날짜", "시작시간", "종료시간", "지점번호", "룸번호", "예약인원");

		while((n=read(fd, (char*)&cur, sizeof(Data)))>0){
			
			start=0, end=0;
			for(int i=0; i<15; i++){
				if(start==0 && cur.check[i]=='O')
					start = i+8;
				if(start!=0 && end==0 && cur.check[i]=='X'){
					end = i+8;
					break;
				}
			}


			printf("%-4d %-6s %-8d %-8d %-8d %-6d %-8d\n",
					++idx, cur.date, start, end, cur.cafe, cur.room, cur.people);
		}

		close(fd);
	}
	else
		printf("예약이 존재하지 않습니다.\n");

}

void modify_reservation(char *usrID)
{
	Data cur, origin_data, new_data;
	char path[PATH_MAX];
	char managerPath[PATH_MAX];
	int fd, size, num;

	while(1){

		num = getInput("수정을 원하는 예약 번호를 입력해주세요, 취소를 원할 시 0을 입력 >> ");
		if(num==0) return;

		getPath(UserPath, usrID, path);

		if(access(path, F_OK)==0){

			if((fd=open(path, O_RDWR))<0){
				fprintf(stderr, "open error for %s\n", path);
				exit(1);
			}

			while(num>0){

				size = read(fd, (char *)&origin_data, sizeof(Data));
				num--;

				if(size<1){
					printf("예약이 존재하지 않습니다.\n");
					return;
				}
			}
		}
		else{
			printf("예약이 존재하지 않습니다.\n");
			continue;
		}
		
		break;
	}


	// 유저 예약 정보 변경
	input_reservation(usrID, &new_data, 0);
	lseek(fd, -size, SEEK_CUR);
	write(fd, (char *)&new_data, sizeof(Data));
	close(fd);

	// 기존 예약 정보 변경
	sprintf(managerPath, "%s/%d/%d/%s", ManagerPath, origin_data.cafe, origin_data.room, origin_data.date);

	if((fd=open(managerPath, O_RDWR))<0){
		fprintf(stderr, "open error for %s\n", managerPath);
		exit(1);
	}

	char check[20];
	read(fd, check, 15);

	for(int i=0; i<15; i++)
		if(origin_data.check[i]=='O')
			check[i] = 'O';

	lseek(fd, 0, SEEK_SET);
	write(fd, check, 15);

	while((size=read(fd, (char *)&cur, sizeof(Data)))>0){

		if(cur.cafe!=-1 && !strcmp(cur.check, origin_data.check)){
			lseek(fd, -size, SEEK_CUR);
			cur.cafe = -1;
			cur.room = -1;
			cur.people = -1;
			write(fd, (char *)&cur, sizeof(Data));
		}
	}
	close(fd);

	// 새로운 예약 정보 추가
	sprintf(managerPath, "%s/%d/%d/%s", ManagerPath, new_data.cafe, new_data.room, new_data.date);

	if(access(managerPath, F_OK)==0){
		if((fd=open(managerPath, O_WRONLY|O_APPEND))<0){
			fprintf(stderr, "open error for %s\n", managerPath);
			exit(1);
		}

		read(fd, check, 15);
	}
	else{
		if((fd=open(managerPath, O_WRONLY|O_CREAT, 0644))<0){
			fprintf(stderr, "open error for %s\n", managerPath);
			exit(1);
		}

		strcpy(check, "OOOOOOOOOOOOOOO");
	}

	for(int i=0; i<15; i++)
		if(new_data.check[i]=='O')
			check[i] = 'X';

	lseek(fd, 0, SEEK_SET);
	write(fd, check, 15);
	close(fd);
}

void input_reservation(char *usrID, Data *data, int mode)
{
	char managerPath[PATH_MAX];
	char userPath[PATH_MAX];
	char dirPath[PATH_MAX]; // [/DB/Manager/지점번호 경로]
	char roomPath[PATH_MAX]; // [/DB/Manager/지점번호/룸번호 경로]
	int option;
	int cafeNumber, roomNumber, start, use;

    strcpy(data->userID, usrID);
    strcpy(data->name, "홍길동");
    strcpy(data->phone, "010-1234-5678");

	// 지점 번호 입력
	while(1){

		cafeNumber = getInput("원하는 지점 번호를 입력해주세요, 취소를 원할 시 0을 입력 >> ");
		data->cafe = cafeNumber;

		if(cafeNumber==0) return;

		option = isExistCafe(cafeNumber, dirPath);

		if(option==1){
			printf("존재하지 않는 지점입니다.\n");
		}

		else if(option==0){
			break;
		}
	}

	// 룸 번호 입력
	while(1){

		roomNumber = getInput("원하는 스터디 예약 공간 번호를 입력해주세요, 취소를 원할 시 0을 입력 >> ");
		data->room = roomNumber;

		if(roomNumber==0) return;

		option = isExistRoom(roomNumber, dirPath, roomPath);

		if(option==1){
			printf("존재하지 않는 스터디룸입니다.\n");
		}
		else if(option==0)
			break;
		else
			continue;
	}

	while(1){
		// 예약 일자 입력
		printf("예약 일자 입력(YYMMDD), 취소를 원할 시 0을 입력 >> ");
		scanf("%s", data->date);
		getchar();

		if(!strcmp(data->date, "0")) return;
		if(!checkDate(data->date)){
			continue;
		}

		// 예약 시작 시간 입력
		printf("예약 시작 시간 입력, 취소를 원할 시 0을 입력 >> ");
		scanf("%d", &start);
		getchar();

		if(start==0) return;
		if(!isTimeEmpty(roomPath, data->date, start, 1)){
			printf("예약이 불가능합니다. 다른 시간대를 선택해주세요.\n");
			print_room(roomPath);
			continue;
		}

		// 사용 예정 시간 입력
		printf("사용 예정 시간 입력, 취소를 원할 시 0을 입력 >> ");
		scanf("%d", &use);
		getchar();
		
		if(use == 0) break;
		if(!isTimeEmpty(roomPath, data->date, start, use)){
			printf("예약이 불가능합니다. 다른 시간대를 선택해주세요.\n");
			print_room(roomPath);
			continue;
		}

		printf("사용 인원 입력, 취소를 원할 시 0을 입력 >> ");
		scanf("%d", &data->people);
		getchar();

		if(data->people == 0) break;
		if(!isAcceptable(dirPath, data->people)){
			continue;
		}

		break;
	}

	char check[20] = "XXXXXXXXXXXXXXX";

	for(int i=start; i<start+use; i++)
		check[i-8] = 'O';

	strcpy(data->check, check);

	if(mode==1){
		getPath(roomPath, data->date, managerPath);
		getPath(UserPath, usrID, userPath);

		changeInfo(managerPath, start, use);
	
		set_new_reservation_data(managerPath, *data);
		set_new_reservation_data(userPath, *data);

		printf("%d번 지점, %d번 룸에 예약되었습니다.\n", cafeNumber, roomNumber);
	}
}
