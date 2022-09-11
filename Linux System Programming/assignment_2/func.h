#ifndef __FUNC_H__
#define __FUNC_H__

int filter(const struct dirent *file);
int getCommand(char *input, char **argv);
char *getExtension(char *file);
double getSize(char *str);
int getUnit(char *str);
char *getFileName(char *file);
void printFileList(Table *table, char m);
char *printFileSize(int number);
void printTime(List *list);
void print_mTime(List *list);
void prompt(char m);

void do_fp_md5(FILE *f, unsigned char *tmp);
void pt_md5(unsigned char *md);

void do_fp_sha1(FILE *f, unsigned char *tmp);
void pt_sha1(unsigned char *md);

void print_command2_usage();

#endif
