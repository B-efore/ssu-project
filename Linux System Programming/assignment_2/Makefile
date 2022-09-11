CC = gcc

all: ssu_sdup ssu_find-md5 ssu_find-sha1 ssu_help

ssu_sdup : ssu_sdup.o DataStructure.o func.o
	$(CC) -o $@ $^ -lcrypto

ssu_find-md5 : ssu_find-md5.o DataStructure.o func.o
	$(CC) -o $@ $^ -lcrypto

ssu_find-sha1 : ssu_find-sha1.o DataStructure.o func.o
	$(CC) -o $@ $^ -lcrypto

ssu_help : ssu_help.o
	$(CC) -o $@ $^

ssu_find-md5.o : ssu_find-md5.c
	$(CC) -c $^

ssu_find-sha1.o : ssu_find-sha1.c
	$(CC) -c $^

ssu_help.o : ssu_help.c
	$(CC) -c $^

ssu_sdup.o: ssu_sdup.c
	$(CC) -c $^

DataStructure.o: DataStructure.c
	$(CC) -c $^

func.o: func.c
	$(CC) -c $^
