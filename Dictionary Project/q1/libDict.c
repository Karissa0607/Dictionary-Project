#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "libDict.h"

#undef DEBUG
#define DEBUG_LEVEL 3

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

#define DICT_INIT_ENTRIES 8
#define DICT_GROW_FACTOR 2

/**
 * Print the dictionary,
 */
void dictPrint(Dict *d, int level){
	if(d==NULL){
		printf("\tDict==NULL\n");
		return;
	}
	printf("\tDict: numEntries=%d capacity=%d [", d->numEntries, d->capacity);
	if(level>=2){
		for(int j=0;j<d->capacity;j++){
			printf("(%s,%d), ",d->entries[j].key, d->entries[j].value);
		}
	}
	printf("]\n");
}

/**
 * Return the DictEntry for the given key, NULL if not found.
 */
DictEntry *dictGet(Dict *d, char *key){
	// find key in row
	if (d == NULL) {
		return NULL;
	};
        for (int i = 0; i < d->numEntries; i++) {
                if (d->entries[i].key == NULL) {
			break;
                };
                if (strcmp(d->entries[i].key, key) == 0) {
			DictEntry *dEntry = &d->entries[i];
			return dEntry;
                };
	};
	return NULL;
}

/**
 * Delete key from Dict if its found in the dictionary
 * Returns 1 if found and deleted
 * Returns 0 otherwise
 */
int dictDel(Dict *d, char *key){
	#ifdef DEBUG
	printf("dictDel(d,%s)\n",key);
	dictPrint(d,DEBUG_LEVEL);
	#endif
	if (d == NULL) {
		return 0;
	};
	// find key
	DictEntry *dEntry = dictGet(d, key);
	if (dEntry == NULL) {
		return 0;
	};
	int i = 0;
	int foundIndex = -1;
	while (i < d->numEntries) {
		if (d->entries[i].key == NULL) {
                        break;
                };
                if (strcmp(d->entries[i].key, key) == 0) {
                        foundIndex = i;
                };
		i++;
	};
	// free key
	if (foundIndex == -1) {
		return 0;
	} else {
		free(d->entries[foundIndex].key);
	};
	// Move everything over
	int k = foundIndex + 1;
	while (k < d->numEntries) {
		d->entries[k-1] = d->entries[k];
		k++;
	};
	d->entries[k-1].key = NULL;
	d->entries[k-1].value = -1;
	d->numEntries -= 1;
	#ifdef DEBUG
	dictPrint(d,DEBUG_LEVEL);
	#endif

	return 1;
}

/**
 * put (key, value) in Dict
 * return 1 for success and 0 for failure
 */
int dictPut(Dict *d, char *key, int value){
	#ifdef DEBUG
	printf("dictPut(d,%s)\n",key);
	dictPrint(d,DEBUG_LEVEL);
	#endif
	if (d == NULL) {
		return 0;
	};
	// If key is already here, just replace value
	#ifdef DEBUG
	dictPrint(d,DEBUG_LEVEL);
	#endif
	DictEntry *dEntry = dictGet(d, key);
	if (dEntry != NULL) {
		dEntry->value = value;
		return 1;
	};

	/** 
	 * At this point we know the key is not in Dict, so we 
	 * need to place (key, value) as a new entry in this 
	 *
	 * if there is no space, expand the row
	 */

	#ifdef DEBUG
	dictPrint(d,DEBUG_LEVEL);
	#endif
	if (d->numEntries == d->capacity) {
		int initial_capacity = d->capacity;
		d->capacity += DICT_GROW_FACTOR;
		if ((d->entries = realloc(d->entries, d->capacity*sizeof(DictEntry))) == NULL) {
			return 0;
		};
		for (int i = initial_capacity; i < d->capacity; i++) {
			if ((d->entries[i].key = (char *) malloc(sizeof(char)*1024)) == NULL) {
				return 0;
			};
			d->entries[i].key = NULL;
			d->entries[i].value = -1;
		};
	};
	/**
	 * We now know we have space.
	 * This is a new key for this row, so we want to place (key, value)
	 *
	 * In python only immutables can be hash keys. If the user can change the key sitting
	 * in the Dict, then we won't be able to find it again. We solve this problem here
	 * by copying keys using strdup.
	 *
	 */
	int len = strlen(key);
	char *copy1 = strdup(key);
	d->entries[d->numEntries].key = copy1;
	d->entries[d->numEntries].value = value;
	d->numEntries += 1;
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
	// free all the keys we allocated
	// free the array of DictEntry
	// free Dict
	for (int i = 0; i < d->capacity; i++) {
		free(d->entries[i].key);
	};
	free(d->entries);
	free(d);
}

/**
 * Allocate and initialize a new Dict. 
 * Returns the address of the new Dict on success
 * Returns NULL on failure
 */
Dict * dictNew(){
	Dict *d = NULL;

	// Create the Dict and initialize it
	if ((d = (Dict *) malloc(sizeof(Dict))) == NULL) return NULL;
        d->numEntries = 0;
        d->capacity = DICT_INIT_ENTRIES;
	// Create the DictEntry array 
	if ((d->entries = (DictEntry *) malloc(sizeof(DictEntry)*d->capacity)) == NULL) {
		free(d);
		return NULL;
	};
	/**
	 * Initialize all of the entries to (NULL, -1). Assuming the rest of our code
	 * is OK, we really should not have to do this. But lets do it
	 * anyway. In this case, entries has 
	 *
	 *     numEntries = 0
	 *     capacity   = DICT_INIT_ENTRIES
	 */
	for (int i = 0; i < d->capacity; i++) {
		if ((d->entries[i].key = (char *) malloc(sizeof(char)*1024)) == NULL) {
			free(d);
			return NULL;
		};
		d->entries[i].key = NULL;
		d->entries[i].value = -1;
	};
	return d;
}

