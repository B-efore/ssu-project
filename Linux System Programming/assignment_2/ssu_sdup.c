#include "DataStructure.h"
#include "func.h"

int main(void)
{
	int argc=0, status;
	char input[BUFFER_SIZE];
	char* argv[ARGUMENT_SIZE+1];

	pid_t pid;

	while(1){
		printf("20202914> ");
		fgets(input, sizeof(input), stdin);
		input[strlen(input)-1] = '\0';

		argc = getCommand(input, argv);
		argv[ARGUMENT_SIZE] = (char*)0;

		if(argc==0)
			continue;

		if(!strcmp(argv[0], "exit")){
			printf("Prompt End\n");
			exit(0);
		}

		if(!strcmp(argv[0], "fmd5")){
			if(argc!=5){
				printf("Error: Usage fmd5 [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]\n");
				continue;
			}
			else{

				if((pid = fork())<0){
					fprintf(stderr, "fork error\n");
					exit(1);
				}
				else if(pid==0){

					if(execv("./ssu_find-md5", argv)<0)
						fprintf(stderr,"execv error\n");
				}
				else
					wait(&status);
			}
		}
		else if(!strcmp(argv[0], "fsha1")){
			if(argc!=5){
				printf("Error: Usage fsha1 [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]\n");
				continue;
			}
			else{

				if((pid = fork())<0){
					fprintf(stderr, "fork error\n");
					exit(1);
				}
				else if(pid==0){

					if(execv("./ssu_find-sha1", argv)<0)
						fprintf(stderr, "execv error\n");
				}
				else
					wait(&status);
			}
		}
		else{

			if((pid = fork())<0){
				fprintf(stderr, "fork error\n");
				exit(1);
			}
			else if(pid==0){

				if(execv("./ssu_help", NULL)<0)
					fprintf(stderr, "execv error\n");
			}
			else
				wait(&status);
		}
	}
}
