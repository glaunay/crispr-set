#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>

#include "io_utils.h"
#include <stdbool.h>
#include <assert.h>
#include "two_bits_encoding.h"


int *createHD_distrib(binaryWord_t queryBinary) {
    size_t maxMissMatches = (size_t)(queryBinary.len + 1);
    #ifdef DEBUG
    printf("Creating HD distrib array with [%zu] elements\n", maxMissMatches);
    #endif
    return createZeroIntArray( maxMissMatches );
}

void printHD_distrib(int *distrib, binaryWord_t queryBinary) {
    printf("HD_distrib of %zu elements\n ", queryBinary.len);
    printf(" k missmatch | n words\n");
    printf("---------------------------\n");
    int sum = 0;
    for (int i = 0 ; i < queryBinary.len + 1 ; i++) {
       // if (i < nbPos - 1) 
        printf("%14d|%9d\n", i, distrib[i]);
        //else  printf("%d\n", distrib[i]);
        sum += distrib[i];
    }
    printf("Total words compared : %d\n", sum);
}

void updateHD_distrib(binaryWord_t binaryQuery, char *targetFile, int *results){
    #ifdef DEBUG
        printf("Updating HD with %s\n", targetFile);
    #endif
    char codec[1024];
    binaryWord_t binaryCurr; 
    
    FILE *fp = fopen(targetFile, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error, can't open%s\n", targetFile);
        exit(1);
    }
    uint64_t cValue;
    size_t count, wLen;
    int total = -1;
    int k;
    char *line = NULL;
    size_t len, read;
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        if(total < 0) {
            sscanf (line, "# %d %zu %s", &total, &wLen, codec);
            assert(wLen == binaryQuery.len);
            assert(strcmp(codec, "twobits") == 0);
            binaryCurr.len = wLen;
            continue;
        }

        sscanf (line, "%llu %zu", &cValue, &count);
        binaryCurr.value = cValue;

        k = hammingDistance(binaryQuery, binaryCurr);
        results[k] += 1;
    }
    fclose(fp);
    if (line)
        free(line);
}

int *computeHD_distrib(binaryWord_t binaryQuery, char **indexFileList, int nbFile) {
    int *results = createHD_distrib(binaryQuery);
    for (int nF = 0 ; nF < nbFile; nF++) {
        updateHD_distrib(binaryQuery, indexFileList[nF], results);
    }
    return results;
}

int main (int argc, char *argv[]) {

    #ifdef DEBUG
    printf( "*** Debug Mode***\n");
    #endif
    int c, n;
    char **compareFileList = NULL;
    char *filePath = NULL; 
    char *fileExtension = NULL; 
    char *fileOut = NULL;
    char *fileLocation = NULL;
    int *HD_distrib = NULL;
    char *querySequence;
    int niCnt;
    
    binaryWord_t queryBinary;
    const char    *short_opt = "hq:n:l:e:f:";
    struct option   long_opt[] =
      {
        {"help",                 no_argument, NULL, 'h'},                    
        {"query",             required_argument, NULL, 'q'},
        {"notin",          required_argument, NULL, 'n'},
        {"loc",            required_argument, NULL, 'l'},
      //  {"val",            required_argument, NULL, 'v'},
        {"ext",            required_argument, NULL, 'e'},
        {NULL,             0,                 NULL, 0  }
    };


     while((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch(c) {
            case -1:       /* no more arguments */
            case 0:        /* long options toggles */
            break;

            case 'q':                
                querySequence = strdup(optarg);
                break;
            case 'n':
                compareFileList = parseFileArg(optarg, &niCnt);
                break;
            case 'l':                
                fileLocation = strdup(optarg);
                break;
            case 'e':
                fileExtension = strdup(optarg);
                break;
            case 'f':
                fileOut = strdup(optarg);
                break;           
            case 'h':
                printf("Performing edition distance based operations between a query sequence and an ensemble of integer keys");
                printf("-h, --help                print this help and exit\n");
                printf("\n");
                return(0);
            case '?':
                fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
                return(-2);
            default:
                fprintf(stderr, "%s: invalid option -- %c\n", argv[0], c);
                fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
                return(-2);
        }
     } 
    if (querySequence == NULL) {
        fprintf(stderr, "-q option required\n");
        return(-2);
    }

   

    queryBinary = encode(querySequence);

    #ifdef DEBUG
        printf("Query:%s = %lld\n", querySequence, queryBinary.value);
    #endif
    char **indexFileList = malloc(sizeof(char*) * niCnt);
    for (int s = 0; s < niCnt; s++) {

        n = constructFilePath(fileLocation, compareFileList[s], fileExtension, &filePath);  
        indexFileList[s] = malloc( (strlen(filePath) + 1) * sizeof (char) );
        strcpy(indexFileList[s], filePath);

        #ifdef DEBUG
        printf("[in] Loading file name [%d] :%s\n", s, indexFileList[s]);
        #endif
    }
  

    fprintf(stderr, "1)\n");
    HD_distrib = computeHD_distrib(queryBinary, indexFileList, niCnt);
    fprintf(stderr, "2)\n");
    
    printHD_distrib(HD_distrib, queryBinary);

    freeStringList(compareFileList, niCnt);
    freeStringList(indexFileList, niCnt);
    if (fileLocation != NULL)
        free(fileLocation);
    if (fileExtension != NULL)
        free(fileExtension);
    if (fileOut != NULL)
        free(fileOut);
    free(HD_distrib);
    
    return 0;
}
