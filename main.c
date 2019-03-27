/*

* Read args IN/OUT
  Read indexfiles location

  consume argsIN
  consume argsOUT


*/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>
#include "custom_set.h"
#include "custom_rank.h"



char *strMemCopy(char *src) {
    char *target = malloc(strlen(src) * sizeof(char) + 1);
    strcpy(target, src);
    
    return target;
}

char **parseFileArg(char *inputString, int *cnt) {
    char **resultList = NULL;  
    const char s[2] = "&";    
    char *token;
    
    char *cpyInputStr = strMemCopy(inputString);
    *cnt = 0;
   
    
    token = strtok(cpyInputStr, s);
    while( token != NULL ) {
        token = strtok(NULL, s);
        (*cnt)++;
    }
    
    resultList = malloc(*cnt * sizeof(char*));
    token = strtok(inputString, s);
    int i = 0;
    while( token != NULL ) {
        resultList[i] = strMemCopy(token);
        //printf("::%s\n", resultList[i]);
        token = strtok(NULL, s);
        i++;
   }

   free(cpyInputStr);
   return resultList;
}


int constructFilePath(char *dirLocation, char *fileName, char *fileExtension, char **filePath) { 
    int pathCharNum = strlen(fileName) + 2; // For the \0
    if (dirLocation != NULL) {
        pathCharNum += strlen(dirLocation); 
        pathCharNum++;// For the '/'
    }
    if(fileExtension != NULL)
        pathCharNum += strlen(fileExtension);
        
    (*filePath) = malloc(sizeof(char) * pathCharNum);

    int v = 0;
    if (fileExtension == NULL) {
        if (dirLocation == NULL) 
            v = sprintf(*filePath, "%s", fileName);
        else
            v = sprintf(*filePath, "%s/%s", dirLocation, fileName);
    } else {
        if (dirLocation == NULL) 
            v = sprintf(*filePath, "%s.%s", fileName, fileExtension);
        else
            v = sprintf(*filePath, "%s/%s.%s", dirLocation, fileName, fileExtension);
    }
    return v;
}


int main (int argc, char *argv[]) {

    #ifdef DEBUG
    printf( "*** Debug Mode***\n");
    #endif
    int c, n;
    int inCnt = 0;
    int notInCnt = 0;
    char **includedFileList = NULL;
    char **notIncludedFileList = NULL;
    int guestValue = 0;
    int s_start = 1;
    //char *includedFileArg = NULL;
    //char *notIncludedFileArg = NULL;
    
    char *fileLocation = NULL;
    char *filePath = NULL; 
    char *fileExtension = NULL; 
    char *fileOut = NULL;
    char *fileSeedPath = NULL;
    
    FILE *fpOut = NULL;
    const char    *short_opt = "hi:o:l:v:e:f:s:";
    struct option   long_opt[] =
    {
        {"help",               no_argument, NULL, 'h'},
        {"seed",          required_argument, NULL, 's'},       
        {"in",          required_argument, NULL, 'i'},
        {"notin",          required_argument, NULL, 'o'},
        {"loc",          required_argument, NULL, 'l'},
        {"val",          required_argument, NULL, 'v'},
        {"ext",          required_argument, NULL, 'e'},
        {NULL,            0,                NULL, 0  }
    };


    while((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch(c) {
            case -1:       /* no more arguments */
            case 0:        /* long options toggles */
            break;

            case 'v':
                guestValue = atoi(optarg);
                break;
            case 's':                
                fileSeedPath = strdup(optarg);
                break;
            case 'i':
                includedFileList = parseFileArg(optarg, &inCnt);
                break;
            case 'o':
                notIncludedFileList = parseFileArg(optarg, &notInCnt);
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
                printf("Performing comparaison ensemble on set of intefger keys");
                printf("-h, --help                print this help and exit\n");
                printf("\n");
                return(0);
      //      case ':':
            case '?':
                printf( "Try `%s --help' for more information.\n", argv[0]);
                return(-2);

            default:
                printf( "%s: invalid option -- %c\n", argv[0], c);
                printf( "Try `%s --help' for more information.\n", argv[0]);
                return(-2);
        }
    }
#ifdef DEBUG
    printf("Input [in] files number %d\n", inCnt);
    printf("Base location is %s\n", fileLocation);
#endif

    if (fileSeedPath != NULL) {
#ifdef DEBUG
    printf("Using following seed file %s\n", fileSeedPath);
#endif
        n = constructFilePath(NULL, fileSeedPath, NULL, &filePath);
        s_start = 0;
    } else {
        n = constructFilePath(fileLocation, includedFileList[0], fileExtension, &filePath);
        s_start = 1;
#ifdef DEBUG
    printf("Starting set from 1st included file::%s\n", filePath);
#endif
    }

    integerSet_t *mainSet = newSetFromFile(filePath);
    free(filePath);

#ifdef DEBUG
    printf("Initial Set Content\n");
    setPrint(mainSet, stdout);
#endif


    integerSet_t *otherSet, *bufferSet;
    for (int s = s_start; s < inCnt; s++) {
        n = constructFilePath(fileLocation, includedFileList[s], fileExtension, &filePath);    
#ifdef DEBUG
        printf("[in] Reading file[%d] :%s\n", s, filePath);
#endif  
        otherSet  = newSetFromFile(filePath);      
#ifdef DEBUG
         setPrint(mainSet, stdout);
#endif    
        bufferSet = setIntersect(mainSet, otherSet);
        moveSet(bufferSet, mainSet);
        otherSet = destroySet(otherSet);  
        free(filePath);
        
#ifdef DEBUG
        printf("Current intersection set\n");
         setPrint(mainSet, stdout);
#endif
        if(mainSet->size == 0) {
            fprintf(stderr, "intersect size is zero, early exit\n");
            break;
        }
        
    }

#ifdef DEBUG
        printf("Final intersection set\n");
        setPrint(mainSet, stdout);
        printf("Input [notin] files number %d\n", notInCnt);
#endif   

   
    for (int s = 0; s < notInCnt; s++) {
        if(mainSet->size == 0){
            fprintf(stderr, "remaining size is zero, early exit substraction\n");
            break;
        }
            

        n = constructFilePath(fileLocation, notIncludedFileList[s], fileExtension, &filePath);
#ifdef DEBUG
        printf("[notin] Reading file[%d] :%s\n", s, filePath);
#endif  
        otherSet  = newSetFromFile(filePath);
        bufferSet = setSubstract(mainSet, otherSet);
        moveSet(bufferSet, mainSet);
        destroySet(otherSet);
        free(filePath);
    }

/*
    int32_t list[11] = { 2,5, 7, 8, 10, 15, 22, 89, 333, 100002, 69022343222 };
    int32_t list2[11] = { 0, 5, 7, 22, 104, 333};
    
    printf("Looking for %d\n", guestValue);
    int index = dichotomicSearch(&list, 11, guestValue);
    printf("-->%d\n", index);
*/


    fpOut = fopen(fileOut, "w");
    fprintf(fpOut, "Final set (Intersect of %d sets) - (Union of %d sets)\n", inCnt, notInCnt);
#ifdef DEBUG  
    fprintf(stderr, "Trying to rank %d\n", mainSet->size);
#endif    
    /*
    rankCell_t *rankList = rankSet(mainSet);
    fprintf(stderr, "Ranked");
    printRankedList(rankList, fpOut);
    */
    rankSetQ(mainSet);
    setPrint(mainSet, fpOut);
    fclose(fpOut);
  
    destroySet(mainSet);
    return 0;
}
