#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"

AddrMapTbl addrmaptbl;
extern FILE *devicefp;

int reserved_empty_blk = DATABLKS_PER_DEVICE;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void print_block(int pbn);
void print_addrmaptbl_info();

int block_is_empty(int pbn);
int find_empty_block(int pbn, FILE *devicefp);

void ftl_open()
{
	int i;

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		addrmaptbl.pbn[i] = -1;
	}

	return;
}

void ftl_write(int lsn, char *sectorbuf)
{
	int offset = lsn % PAGES_PER_BLOCK;
	int lbn = lsn / PAGES_PER_BLOCK;
	int pbn = addrmaptbl.pbn[lbn];
	int ppn;

	char *pagebuf, *tpagebuf;
	SpareData *sdata;

	tpagebuf = (char *)malloc(PAGE_SIZE);
	pagebuf = (char *)malloc(PAGE_SIZE);
	sdata = (SpareData *)malloc(SPARE_SIZE);
	sdata->lsn = lsn;

	memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
	memcpy(pagebuf+SECTOR_SIZE, sdata, SPARE_SIZE);

	if(pbn == -1)
	{
		pbn = find_empty_block(-1, devicefp);
		ppn = pbn * PAGES_PER_BLOCK + offset;

		if(dd_write(ppn, pagebuf)!=1){
			fprintf(stderr, "dd_write error\n");
			exit(1);
		}

		addrmaptbl.pbn[lbn] = pbn;
	}
	else
	{
		ppn = pbn * PAGES_PER_BLOCK + offset;
		
		if(dd_read(ppn, tpagebuf)==1){
			memcpy(sdata, tpagebuf+SECTOR_SIZE, SPARE_SIZE);
		}

		//exist
		if(sdata->lsn!=-1){

			if(block_is_empty(reserved_empty_blk)!=1){
				reserved_empty_blk = find_empty_block(-1, devicefp);
			}

			//copy
			for(int i=0; i<PAGES_PER_BLOCK; i++){
				if(i!=offset){
					if(dd_read(pbn*PAGES_PER_BLOCK+i, tpagebuf)!=1){
						fprintf(stderr, "dd_read error\n");
						exit(1);
					}
			
					if(dd_write(reserved_empty_blk*PAGES_PER_BLOCK+i, tpagebuf)!=1){
						fprintf(stderr, "dd_write error\n");
						exit(1);
					}
				}
			}

			if(dd_write(reserved_empty_blk*PAGES_PER_BLOCK+offset, pagebuf)!=1){
				fprintf(stderr, "dd_write error\n");
				exit(1);
			}

			//erase
			dd_erase(pbn);
			addrmaptbl.pbn[lbn] = reserved_empty_blk;
//			print_block(reserved_empty_blk);
			reserved_empty_blk = pbn;
		}
		else{

			if(dd_write(ppn, pagebuf)!=1){
				fprintf(stderr, "dd_wrote error\n");
				exit(1);
			}

			addrmaptbl.pbn[lbn] = pbn;
//			print_block(pbn);
		}
	}

//	print_addrmaptbl_info();

	free(pagebuf);
	free(tpagebuf);
	free(sdata);

	return;
}

void ftl_read(int lsn, char *sectorbuf)
{
	int offset = lsn % PAGES_PER_BLOCK;
	int lbn = lsn / PAGES_PER_BLOCK;
	int pbn = addrmaptbl.pbn[lbn];

	char pagebuf[PAGE_SIZE];

	if(dd_read(pbn*PAGES_PER_BLOCK + offset, pagebuf)==1){
		memcpy(sectorbuf, pagebuf, SECTOR_SIZE);
	}
	else{
		fprintf(stderr, "dd_read error\n");
	}

	return;
}

//
// for debugging
//
void print_block(int pbn)
{
	char *pagebuf;
	SpareData *sdata;
	int i;
	
	pagebuf = (char *)malloc(PAGE_SIZE);
	sdata = (SpareData *)malloc(SPARE_SIZE);

	printf("Physical Block Number: %d\n", pbn);

	for(i = pbn*PAGES_PER_BLOCK; i < (pbn+1)*PAGES_PER_BLOCK; i++)
	{
		dd_read(i, pagebuf);
		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
		printf("\t   %5d-[%7d]\n", i, sdata->lsn);
	}

	free(pagebuf);
	free(sdata);

	return;
}

//
// for debugging
//
void print_addrmaptbl_info()
{
	int i;

	printf("Address Mapping Table: \n");
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		if(addrmaptbl.pbn[i] >= 0)
		{
			printf("[%d %d]\n", i, addrmaptbl.pbn[i]);
		}
	}
}

int block_is_empty(int pbn)
{
	SpareData *sdata;
	char *pagebuf;

	sdata = (SpareData *)malloc(SPARE_SIZE);
	pagebuf = (char *)malloc(PAGE_SIZE);

	int n = pbn*PAGES_PER_BLOCK;

	for(int i=n; i<n+PAGES_PER_BLOCK; i++)
	{
		if(dd_read(i, pagebuf)==1){
			memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
			if(sdata->lsn != -1){
				return 0;
			}
		}
	}

	free(pagebuf);
	free(sdata);

	return 1;
}

int find_empty_block(int pbn, FILE *devicefp)
{
	for(int i=0; i<BLOCKS_PER_DEVICE; i++){
		if(i==pbn) continue;
		if(block_is_empty(i)) return i;
	}

	return -1;
}
