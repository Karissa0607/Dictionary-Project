#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libDict.h"
#define BUFF_LEN 1024

int test_dictNew(){
	printf("BEGIN: %s\n", __FUNCTION__);

	Dict *d=dictNew(10);
	dictPrint(d,3);
	dictFree(d);

	printf("END  : %s\n", __FUNCTION__);
}

int test_dictGet_empty(){
	printf("BEGIN: %s\n", __FUNCTION__);

	Dict *d=dictNew();
	char *keys[]={ "", "a", "aa", "ab", "this", "that", "this is a very long string with lots of characters"};
	int keysLen = sizeof(keys)/sizeof(keys[0]); // aka sizeof(keys)/sizeof(char *);

	for(int i=0;i<keysLen;i++){
		DictEntry *de = dictGet(d,keys[i]);
		if(de==NULL){
			printf("OK   : searched for %s on empty Dict and result was NULL\n", keys[i]);
		}else{
			printf("ERROR: searched for %s on empty Dict and result was NOT NULL\n", keys[i]);
		}
	}
	dictFree(d);

	printf("END  : %s\n", __FUNCTION__);
}

/**
 * Check that words at index%m==t in the file are exactly the words
 * in the dictionary
 */
int checkWords(Dict *d, FILE *fp, int numWords, int m, int t){
	printf("BEGIN: %s\n", __FUNCTION__);

	int numWordsProcessed = 0;
	char buff[BUFF_LEN];

	rewind(fp);
	while(fgets(buff, BUFF_LEN, fp)!=NULL){
		buff[strcspn(buff, "\r\n")] = '\0'; // remove trailing newline
		DictEntry *de=dictGet(d,buff);
		if(numWordsProcessed%m==t){
			if(de==NULL){
				printf("ERROR: Key %s not found\n", buff);
				continue;
			}
			char *key=de->key;
			if(de->value!=numWordsProcessed){
				printf("ERROR: %s found, but value incorrect\n", buff);
				continue;
			}
		} else {
			if(de!=NULL){
				printf("ERROR: Deleted key %s still in Dict\n", buff);
				continue;
			}
		}
		numWordsProcessed++;
		if(numWordsProcessed==numWords)break;
	}
	printf("END  : %s\n", __FUNCTION__);
}

/**
 * Add words at index%m==t to the dictionary
 */
int addWords(Dict *d, FILE *fp, int numWords, int m, int t){
	printf("BEGIN: %s\n", __FUNCTION__);
	int numWordsProcessed = 0;
	char buff[BUFF_LEN];
	rewind(fp);
	numWordsProcessed = 0;
	while(fgets(buff, BUFF_LEN, fp)!=NULL){
		buff[strcspn(buff, "\r\n")] = '\0'; // remove trailing newline
		if(numWordsProcessed%m==t){
			//dictPrint(d, 3);
			//printf("Buff word: %s \n", buff);
			//printf("Buff word: %s \n", buff);
			//printf("Dict word: %s \n", d->entries[0].key);
			//printf("%d \n", d->numEntries);
			dictPut(d,buff,numWordsProcessed);
			//dictPrint(d, 3);
		}
		numWordsProcessed++;
		if(numWordsProcessed==numWords)break;
	}
	printf("END  : %s\n", __FUNCTION__);
}

/**
 * Delete words at index%m==t from the dictionary
 */
int delWords(Dict *d, FILE *fp, int numWords, int m, int t){
	printf("BEGIN: %s\n", __FUNCTION__);
	int numWordsProcessed = 0;
	char buff[BUFF_LEN];

	rewind(fp);
	while(fgets(buff, BUFF_LEN, fp)!=NULL){
		buff[strcspn(buff, "\r\n")] = '\0'; // remove trailing newline
		if(numWordsProcessed%m==t){
			DictEntry *de=dictGet(d,buff);
			if(de!=NULL){
				if(!dictDel(d,buff))printf("ERROR (delete): Can't delete %s\n", buff);
			}
		}
		numWordsProcessed++;
		if(numWordsProcessed==numWords)break;
	}
	printf("END  : %s\n", __FUNCTION__);
}

int test_stressTest(int numWords){
	printf("BEGIN: %s\n", __FUNCTION__);

	int numWordsProcessed = 0;
	char buff[BUFF_LEN];
	Dict *d=dictNew();

	char *fileName = "/usr/share/dict/words";
	FILE *fp = fopen(fileName,"r");
	if(fp==NULL){
		perror(fileName);
		exit(1);	}

	addWords(d,fp,numWords,1,0);
	//dictPut(d, "AOL", 20);
	//dictPut(d, "a", 0);
	//dictPrint(d, 3);
	//dictPut(d, "b", 1);
	//dictPrint(d, 3);
	//dictPut(d, "c", 2);
	//dictPrint(d, 3);
	//dictPut(d, "d", 3);
	//dictPrint(d, 3);
	//dictPut(d, "e", 4);
	//dictPrint(d, 3);
	//dictPut(d, "f", 5);
	//dictPrint(d, 3);
	//dictPut(d, "g", 6);
	//dictPrint(d, 3);
	//dictPut(d, "h", 7);
	//dictPrint(d, 3);
	//dictPut(d, "i", 8);
	//dictPrint(d, 3);
	//dictPut(d, "a", 9);
	//dictPrint(d, 3);
	checkWords(d,fp,numWords,1,0);
	delWords(d,fp,numWords,2,1);
	checkWords(d,fp,numWords,2,0);
	//dictPrint(d, 3);
	delWords(d,fp,numWords,2,0);
	checkWords(d,fp,numWords,2,3); // No words should be present
	//dictPrint(d, 3);
	fclose(fp);

	dictFree(d);
	printf("END  : %s\n", __FUNCTION__);
}

int main(int argc, char **argv){
	test_dictNew();
	test_dictGet_empty();
	test_stressTest(150); // change back to 150
}
