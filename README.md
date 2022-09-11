# ssu-project
숭실대학교 컴퓨터학부 2022-1학기 전공 과제 정리

---

## 리눅스시스템프로그래밍

### 설계과제1
#### 과제 개요
파일/디렉토리의 이름과 경로를 받아 경로로부터 **파일/디렉토리를 탐색하고 이들을 비교**하는 프로그램. 조건에 맞는 파일/디렉토리를 출력하면 사용자는 출력된 파일/디렉토리 중 하나를 골라 원본 파일과의 비교를 진행할 수 있다. 비교를 수행하면 추가된 내용, 삭제된 내용, 수정된 내용이 출력되며, 파일의 내용이 같을 경우에는 아무 출력도 없다.

#### 명령어
```
find [FILENAME] [PATH]
>> [INDEX] [OPTION ...]
exit
help
```

#### 주요 명령어 설명
|명령어|옵션|설명|
|---|---|---|
|`find`||`[PATH]`로부터 탐색을 시작해 `[FILENAME]`과 이름과 크기가 동일한 모든 파일(디렉토리)을 리스트로 출력|
|`find`|`[FILENAME]`|특정 디렉토리 내에 존재하는 파일(디렉토리), 절대경로와 상대경로 모두 가능 (필수 입력)|
|`find`|`[PATH]`|탐색을 시작할 디렉토리 경로, 절대경로와 상대경로 모두 가능 (필수 입력)|
|>>|`[INDEX]` `[OPTION]`|0번째 파일과 `[INDEX]`번째 파일을 `[OPTION]`에 따라 비교한다|
|`[OPTION]`|q|두 파일 내용이 다른 경우 차이점 출력 없이 알림|
|`[OPTION]`|s|두 파일 내용이 동일한 경우 알림|
|`[OPTION]`|i|대소문자 구분 없이 비교 *(오류 존재)*|
|`[OPTION]`|r|디렉토리 비교 시 하위 디렉토리를 전부 재귀적으로 탐색해 모든 파일 비교 *(오류 존재)*|
|| | (각 옵션은 중복 사용이 가능하다.)|
|`exit`||프로그램 실행 시간을 출력한 뒤 종료한다.|
|`help`||프로그램 명령어 사용법을 출력한다.|

<br>

### 설계과제2
#### 과제 개요
사용자로부터 명령어를 입력받으면 **입력된 조건에 따라 시스템 내 존재하는 중복 파일을 탐색하고 삭제**하는 프로그램. 중복 파일은 해시값이 동일한 파일을 뜻하며, 해시값이 같은 파일을 세트로 하여 중복 파일 세트를 화면상에 출력한다. 출력된 목록에서 사용자는 옵션에 따라 파일을 선택적으로 삭제할 수 있다.
 
 #### 명령어
 ```
 fmd5 [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]
 fsha1 [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]
 >> [SET_INDEX] [OPTION ...]
 exit
 help
 ```
 
 #### 주요 명령어 설명
|명령어|옵션|설명|
|---|---|---|
|`fmd5`||MD5 해시값 사용 *(대용량 파일 탐색 오류)*|
|`fsha1`||SHA1 해시값 사용 *(대용량 파일 탐색 오류)*|
|`fmd5`, `fsha1`||`[TARGET_DIRECTORY]` 아래 모든 디렉토리에서 `[FILE_EXTENSION]`을 가지는 `[MINSIZE]`이상 `[MAXSIZE]`이하 중복 파일을 찾아 출력|
|`fmd5`, `fsha1`|`[FILE_EXTENSION]`|`*` 입력 시, 모든 정규 파일을 대상으로 탐색. `*.(확장자명)` 입력 시, (확장자명)을 가진 정규 파일을 대상으로 탐색 (필수 입력)|
|`fmd5`, `fsha1`|`[MINSIZE]`|탐색할 파일의 최소 크기를 설정. KB, MB, GB 단위 설정 가능하며 기본은 바이트 단위. `~` 입력 시, `[MAXSIZE]` 이하인 모든 파일에 대해 탐색 (필수 입력)|
|`fmd5`, `fsha1`|`[MAXSIZE]`|탐색할 파일의 최대 크기를 설정. KB, MB, GB 단위 설정 가능하며 기본은 바이트 단위. `~` 입력 시, `[MINSIZE]` 이상인 모든 파일에 대해 탐색 (필수 입력)|
|`fmd5`, `fsha1`|`[TARGET_DIRECTORY]`|탐색할 디렉토리 경로 설정. 절대경로와 `~(홈 디렉토리)`를 포함한 상대경로 모두 가능 (필수 입력)|
|>>|`[SET_INDEX]` `[OPTION ...]`|`[SET_INDEX]`에 해당하는 중복 파일 리스트에서 `[OPTION]`에 따라 중복 파일 삭제|
|`[OPTION]`|`d [LIST_IDX]`|선택한 세트에서 `[LIST_IDX]`에 해당하는 파일 삭제|
|`[OPTION]`|`i`|선택한 세트에 존재하는 파일의 절대 경로를 보여주며 삭제 여부 확인 후 파일 삭제 또는 유지 *(오류 존재)*|
|`[OPTION]`|`f`|가장 최근 수정한 파일을 남기고 전부 삭제|
|`[OPTION]`|`t`|가장 최근 수정한 파일을 남기고 전부 휴지통으로 이동|

<br>

### 설계과제3
사용자로부터 명령어를 입력받으면 **입력된 조건에 따라 시스템 내 존재하는 중복 파일을 탐색하고 관리**하는 프로그램. 탐색이 끝난 후에는 중복 파일 리스트 혹은 삭제 파일 리스트를 사용자가 입력한 카테고리에 맞추어 출력할 수 있다. 또한 삭제 파일 리스트에서 관리되고 있는 파일 및 디렉토리를 복원하는 것이 가능하다.

#### 명령어
```
fsha1 -e [FILE_EXTENSION] -l [MINSIZE] -h [MAXSIZE] -d [TARGET_DIRECTORY] -t [THREAD_NUM]
>> delete -l [SET_IDX] -d [LIST_IDX] -i -f -t
list -l [LIST_TYPE] -c [CATEGORY] -p [ORDER]
trash -c [CATEGORY] -o [ORDER]
restore [RESTORE_INDEX]
exit
help
```

 #### 주요 명령어 설명
|명령어|옵션|설명|
|---|---|---|
|`fsha1`||SHA1 해시값 사용|
|`fsha1`||옵션 설계과제2와 동일|
|`fsha1`|`[THREAD_NUM]`|탐색 과정에서 사용할 최대 쓰레드 개수 설정. 기본 1개, 최대 5개. *(미구현)*|
|>>|`delete`|`[SET_IDX]`에 해당하는 중복 파일 리스트에서 옵션에 따라 중복 파일 삭제|
||`[SET_IDX]`|중복 파일 리스트에서 삭제하고자 하는 세트 번호 (필수 입력)|
||`-d [LIST_IDX]`|선택한 세트에서 `LIST_IDX`에 해당하는 파일 삭제|
||`-i`|선택한 세트에 존재하는 파일의 절대 경로를 보여주며 삭제 여부 확인 후 파일 삭제 또는 유지|
||`-f`|가장 최근 수정한 파일을 남기고 전부 삭제|
||`-t`|가장 최근 수정한 파일을 남기고 전부 휴지통으로 이동
|`list`||중복 파일을 관리하는 연결 리스트를 출력|
|`list`|`[LIST_TYPE]`|옵션에 따라 세트 또는 리스트를 정렬. `fileset(세트)`, `filelist(리스트)`|
|`list`|`[CATEGORY]`|정렬할 카테고리를 설정. `filename(절대 경로)`, `size(크기)`, `uid(사용자ID)`, `gid(그룹ID)`, `mode(모드)`|
|`list`|`[ORDER]`|정렬할 기준을 설정. `-1(내림차순)`, `1(오름차순)`|
|`trash`||현재 관리하는 쓰레기통에 존재하는 파일 및 디렉토리 출력|
|`trash`|`[CATEGORY]`|정렬할 카테고리를 설정. `filename(절대 경로)`, `size(크기)`, `date(날짜)`, `time(시간)`|
|`trash`|`[ORDER]`|정렬할 기준을 설정. `-1(내림차순)`, `1(오름차순)`|
|`restore`|`[RESTORE_INDEX]`|쓰레기통에서 `[RESTORE_INDEX]`에 해당하는 파일을 복원|


<br>

### 개발 환경
```
Linux Ubuntu-20.04.4-desktop
gcc version 9.4.0
```

---

## 파일처리

### 과제1
#### 과제 개요
파일 I/O 연산 프로그램을 구현한다.

#### 사용법
``` bash
// copy.c
a.out "origin_file_name" "dest_file_name"
// read.c
a.out "offset" "byte" "file_name"
// merge.c
a.out "file_name1" "file_name2" "file_name3"
// overwrite.c
a.out "offset" "data" "file_name"
// insert.c
a.out "offset" "data" "file_name"
// delete.c
a.out "offset" "byte" "file_name"
```

|소스코드|설명|
|---|---|
|`copy.c`|존재하는 원본 파일로부터 10바이트 단위로 데이터를 읽어 **새로운 복사본 파일을 생성**하는 프로그램|
|`read.c`|존재하는 원본 파일로부터 `offset`을 기준으로 `byte`의 크기만큼 **데이터를 읽어 화면에 출력**하는 프로그램. `byte`가 음수일 경우 왼쪽의 데이터를, 양수일 경우 오른쪽의 데이터를 읽는다.|
|`merge.c`|존재하는 **두 개의 파일을 병합해 새로운 파일을 생성**하는 프로그램. 생성된 파일은 두 개의 파일 크기의 합과 일치해야 한다.|
|`overwrite.c`|존재하는 파일에서 주어진 `offset`부터 시작해 **`data`를 덮어쓰는 프로그램**. EOF를 만나도 중단하지 않고 쓰기를 진행한다.|
|`insert.c`|존재하는 파일에서 `offset`과 `offset+1` 사이에 **데이터를 삽입**하는 프로그램. `offset`이 파일의 맨 마지막일 경우 append로 처리한다.|
|`delete.c`|존재하는 파일에서 `offset`을 기준으로 `byte` 크기만큼 **데이터를 삭제**하는 프로그램. `byte`가 음수일 경우 왼쪽의 데이터를, 양수일 경우 오른쪽의 데이터를 삭제한다. 지정한 `byte`만큼의 데이터가 존재하지 않을 경우 존재하는 데이터만 삭제한다. 삭제 후에는 삭제 데이터를 기준으로 전후 데이터가 병합되어야 한다.|

<br>

### 과제2
#### 과제 개요
순차읽기 및 랜덤읽기의 디스크 I/O 비용을 측정한다.

#### 사용법
``` bash
// create_records.c
a.out "#record" "record_file_name"
// seq_read.c
a.out "record_file_name"
// rand_read.c
a.out "record_file_name"
```

|소스코드|설명|
|---|---|
|`create_records.c`|입력된 `file_name` 파일에 학생 레코드를 `#record` 수만큼 반복적으로 저장한다. 학생 레코드의 크기는 200바이트이며 의미없는 데이터로 채워도 상관없다.|
|`seq_read.c`|`file_name`에 저장된 모든 레코드를 **순차적**으로 **하나씩** 프로그램 상으로 읽어 들이며 걸리는 시간을 출력한다.|
|`rand_read.c`|`file_name`에 저장된 모든 레코드를 **랜덤**하게 **하나씩** 프로그램 상으로 읽어 들이며 걸리는 시간을 출력한다. *(오류 존재)*|

<br>

### 과제3
#### 과제 개요
Flash Memory에서의 Block Mapping FTL을 구현한다.

|소스코드|설명|
|---|---|
|`blkmap.h`|flash memory를 구성하는 block, page, sector, spare area 등의 상수 정의와 address mapping table에 대한 구조체 정의|
|`devicedriver.c`|flash memory에 page 단위로 데이터를 읽고 쓰기 위한 read(), write()함수와 block 데이터를 소거하는 erase()함수 정의|
|`ftl.c`|FTL 기법을 따르는 ftl_open(), ftl_write(), ftl_read() 함수 구현|
|`main.c`|file system의 역할을 수행|

<br>

### 과제4
#### 과제 개요
레코드를 저장하고 검색한다.

#### 사용법
``` bash
// 레코드 추가
a.out -a record_file_name "field_value1" "field_value2" "field_value3" "field_value4" "field_value5"
// 레코드 검색
a.out -s record_file_name "field_name=field_value"
```

|소스코드|설명|
|---|---|
|`student.c`|레코드 구분은 **fixed-length record 방식**을 사용하고, 필드 구분은 **delimiter 방식**을 사용한다. 헤더 레코드가 존재하며 전체 레코드 수를 저장하는 4 바이트 공간과 예약 공간 4 바이트를 갖는다. `-a` 옵션을 사용함과 함께 사용자로부터 학생 레코드에 대한 입력을 받으면 **레코드를 추가**할 수 있다. `-s` 옵션과 함께 레코드 파일명, 필드명, 필드값을 입력받으면 레코드 파일로부터 **입력한 키와 일치하는 레코드를 찾아 전부 출력**한다.|
|`student.h`|STUDENT 구조체 정의|

<br>

### 과제5
#### 과제 개요
과제4의 프로그램에 레코드 삭제와 삽입 기능을 추가한다.

#### 사용법
``` bash
// 레코드 삭제
a.out -d record_file_name "ID=field_value"
// 레코드 삽입
a.out -i record_file_name "field_value1" "field_value2" "field_value3" "field_value4" "field_value5"
```

|소스코드|설명|
|---|---|
|`student.c`|레코드 삭제의 경우, `-d` 옵션과 함께 **입력된 키값을 만족하는 레코드들을 검색해 전부 삭제**한다. 레코드의 삭제는 해당 레코드의 첫번째 바이트에 `*`를 저장하는 것으로 처리한다. 삭제 레코드 리스트는 따로 관리되어야 한다. 헤더 레코드의 예약 공간에는 삭제 레코드 리스트의 맨 앞 레코드의 rrn을 이진 정수 형태로 저장한다. 레코드 삽입의 경우, `-i` 옵션과 함께 입력된 값들을 **하나의 레코드로 삽입**한다. 삭제 레코드가 존재할 시, 삭제 레코드 리스트의 맨 앞 레코드 공간에 삽입하고 삭제 레코드 리스트를 수정한다.|
|`student.h`||

<br>

### 개발 환경
```
Linux Ubuntu-18.04-desktop
gcc version 7.5
```

## 프로그래밍언어

### 개발 환경
```
Linux Ubuntu-20.04.4-desktop
gcc version 9.4.0
```
