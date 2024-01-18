#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libDict.h"

//#define DEBUG
//#define DEBUG_LEVEL 3

//#ifdef DEBUG
//# define DEBUG_PRINT(x) printf x
//#else
//# define DEBUG_PRINT(x) do {} while (0)
//#endif

#define DICT_INIT_ROWS 1024
#define DICT_GROW_FACTOR 2
#define ROW_INIT_ENTRIES 8
#define ROW_GROW_FACTOR 2

#define PRIME1 77933 // a large prime
#define PRIME2 119557 // a large prime

/**
 * hash *c as a sequence of bytes mod m
 */
int dictHash(char *c, int m){
	int sum=0;
	while(*c!='\0'){
		int num = *c; 
		sum+= PRIME1*num+PRIME2*sum;
		c++;
	}
	if(sum<0)sum=-sum;
	sum = sum%m;
	return sum;
}

/**
 * Print the dictionary, 
 * level==0, dict header
 * level==1, dict header, rows headers
 * level==2, dict header, rows headers, and keys
 */
void dictPrint(Dict *d, int level){
	if(d==NULL){
		printf("\tDict==NULL\n");
		return;
	}
	printf("Dict\n");
	printf("\tnumRows=%d\n",d->numRows);
	if(level<1)return;

	for(int i=0;i<d->numRows;i++){
		printf("\tDictRow[%d]: numEntries=%d capacity=%d keys=[", i, d->rows[i].numEntries, d->rows[i].capacity);
		if(level>=2){
			for(int j=0;j<d->rows[i].numEntries;j++){
				printf("%s, ",d->rows[i].entries[j].key);
			}
		}
		printf("]\n");
	}
}

/**
 * Return the DictEntry for the given key, NULL if not found.
 * This is so we can store NULL as a value.
 */
DictEntry *dictGet(Dict *d, char *key){

	// find row
	if (d == NULL) {
		return NULL;
	};
	for (int i=0; i < d->numRows; i++) {
		for (int j=0; j < d->rows[i].numEntries; j++) {
			if (d->rows[i].entries[j].key == NULL) {
				break;
			};
			if (strcmp(d->rows[i].entries[j].key, key) == 0) {
				DictEntry *dEntry = &d->rows[i].entries[j];
                        	return dEntry;
			};
		};
	};
	// find key in row
	return NULL;
}

/**
 * Delete key from dict if its found in the dictionary
 * Returns 1 if found and deleted
 * Returns 0 otherwise
 */
int dictDel(Dict *d, char *key){
	// find row
	int h=0;

	#ifdef DEBUG
	printf("dictDel(d,%s) hash=%d\n",key, h);
	dictPrint(d,DEBUG_LEVEL);
	#endif

	if (d == NULL) {
		return 0;
	};
	// find key in row
	DictEntry *de = dictGet(d, key);
	if (de == NULL) {
		return 0;
	};
	int i = 0;
	int foundIndexJ = -1;
        int foundIndexI = -1;
        while (i < d->numRows) {
		int j = 0;
		while (j < d->rows[i].numEntries) {
			if (d->rows[i].entries[j].key == NULL) {
                        	break;
                	};
                	if (strcmp(d->rows[i].entries[j].key, key) == 0) {
                        	foundIndexI = i;
				foundIndexJ = j;
                	};
			j++;
		};
                i++;
        }
	// free key
	if (foundIndexI == -1 || foundIndexJ == -1) {
                return 0;
        } else {
                free(d->rows[foundIndexI].entries[foundIndexJ].key);
        };
	// Move everything over
	int k = foundIndexJ + 1;
        while (k < d->rows[foundIndexI].numEntries) {
                d->rows[foundIndexI].entries[k-1] = d->rows[foundIndexI].entries[k];
                k++;
        };
        d->rows[foundIndexI].entries[k-1].key = NULL;
        d->rows[foundIndexI].entries[k-1].value = NULL;
	d->rows[foundIndexI].numEntries -= 1;
	#ifdef DEBUG
	dictPrint(d,DEBUG_LEVEL);
	#endif

	return 1;
}

/**
 * put (key, value) in Dict
 * return 1 for success and 0 for failure
 */
int dictPut(Dict *d, char *key, void *value){
	if (d == NULL) {
		return 0;
	};
	int h = dictHash(key, d->numRows);

	#ifdef DEBUG
	printf("dictPut(d,%s) hash=%d\n",key, h);
	dictPrint(d,DEBUG_LEVEL);
	#endif

	// If key is already here, just replace value
	DictEntry *dEntry = dictGet(d, key);
        if (dEntry != NULL) {
		int l = strlen(value);
                dEntry->value = strndup(value, l);
                return 1;
        };

	#ifdef DEBUG
	dictPrint(d,DEBUG_LEVEL);
	#endif

	/** 
	 * else we need to place (key,value) as a new entry in this row
	 * if there is no space, expand the row
	 */
	if (d->rows[h].numEntries == d->rows[h].capacity) {
                int initial_capacity = d->rows[h].capacity;
                d->rows[h].capacity += DICT_GROW_FACTOR;
                if ((d->rows[h].entries = realloc(d->rows[h].entries, d->rows[h].capacity*sizeof(DictEntry))) == NULL) {
                        return 0;
                };
                for (int i = initial_capacity; i < d->rows[h].capacity; i++) {
                        if ((d->rows[h].entries[i].key = (char *) malloc(sizeof(char)*1024)) == NULL) {
                                return 0;
                        };
                        d->rows[h].entries[i].key = NULL;
                        d->rows[h].entries[i].value = NULL;
                };
        };
	#ifdef DEBUG
	dictPrint(d,DEBUG_LEVEL);
	#endif

	/** 
	 * This is a new key for this row, so we want to place the key, value pair
	 * In python only immutables can be hash keys. If the user can change the key sitting
	 * in the Dict, then we won't be able to find it again. We solve this problem here
	 * by copying keys using strdup.
	 * 
	 * At this point we know there is space, so copy the key and place it in the row
	 * along with its value.
	 */
	int len = strlen(key);
        char *copy1 = strndup(key, len);
	int l2 = strlen(value);
	char *copy2 = strndup(value, l2);
        d->rows[h].entries[d->rows[h].numEntries].key = copy1;
        d->rows[h].entries[d->rows[h].numEntries].value = copy2;
	d->rows[h].numEntries += 1;
	#ifdef DEBUG
	dictPrint(d,DEBUG_LEVEL);
	#endif
	return 1;
}

/**
 * free all resources allocated for this Dict. Everything, and only those things
 * allocated by this code should be freed.
 */
void dictFree(Dict *d){
	for (int i=0; i < d->numRows; i++) {
		for (int j=0; j < d->rows[i].capacity; j++) {
			free(d->rows[i].entries[j].key);
			free(d->rows[i].entries[j].value);
		};
	};
	for (int i=0; i < d->numRows; i++) {
		free(d->rows[i].entries);
	};
	free(d->rows);
	free(d);
}

/**
 * Allocate and initialize a new Dict. Initially this dictionary will have initRows
 * hash slots. If initRows==0, then it defaults to DICT_INIT_ROWS
 * Returns the address of the new Dict on success
 * Returns NULL on failure
 */
Dict * dictNew(int initRows){
	Dict *d=NULL;
	if ((d = (Dict *) malloc(sizeof(Dict))) == NULL) return NULL;
	if (initRows == 0) {
		d->numRows = DICT_INIT_ROWS;
	} else {
		d->numRows = initRows;
	};
	if ((d->rows = (DictRow *) malloc(sizeof(DictRow)*d->numRows)) == NULL) {
		free(d);
		return NULL;
	};
	for (int i=0; i < d->numRows; i++) {
		d->rows[i].numEntries = 0;
		d->rows[i].capacity = ROW_INIT_ENTRIES;
		if ((d->rows[i].entries = (DictEntry *) malloc(sizeof(DictEntry)*d->rows[i].capacity)) == NULL) {
			free(d);
			return NULL;
		};
		for (int j=0; j < d->rows[i].capacity; j++) {
			if ((d->rows[i].entries[j].key = (char *) malloc(sizeof(char)*1024)) == NULL) {
                        	free(d);
                        	return NULL;
			};
			if ((d->rows[i].entries[j].value = (void *) malloc(sizeof(void))) == NULL) {
				free(d);
				return NULL;
			};
			d->rows[i].entries[j].key = NULL;
			d->rows[i].entries[j].value = NULL;
                };
	};
	return d;
}

