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




char *strMemCopy(char *src) {
    char *target = malloc(strlen(src) * sizeof(char) + 1);
    strcpy(target, src);
    
    return target;
}

char **parseFileArg(char *inputString, int *cnt) {
    char **resultList = NULL;  
    const char s[2] = ",";    
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


int constructFilePath(char *dirLocation, char *fileName, char **filePath) { 
    int pathCharNum = strlen(fileName) + 1; // For the \0
    if (dirLocation != NULL) {
        pathCharNum += strlen(dirLocation); 
        pathCharNum++;// For the '/'
    }
    (*filePath) = malloc(sizeof(char) * pathCharNum);

    int v = 0;
    if (dirLocation == NULL) 
        v = sprintf(*filePath, "%s", fileName);
    else
        v = sprintf(*filePath, "%s/%s", dirLocation, fileName);

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

    //char *includedFileArg = NULL;
    //char *notIncludedFileArg = NULL;
    
    char *fileLocation = NULL;
    char *filePath = NULL; 
    const char    *short_opt = "hi:o:l:v:";
    struct option   long_opt[] =
    {
        {"help",               no_argument, NULL, 'h'},
        {"in",          required_argument, NULL, 'i'},
        {"notin",          required_argument, NULL, 'o'},
        {"loc",          required_argument, NULL, 'l'},
        {"val",          required_argument, NULL, 'v'},
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
            case 'i':
                includedFileList = parseFileArg(optarg, &inCnt);
                break;
            case 'o':
                notIncludedFileList = parseFileArg(optarg, &notInCnt);
                break;
            case 'l':
                fileLocation = strdup(optarg);
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

    printf("Files IN number %d in\n", inCnt);

    n = constructFilePath(fileLocation, includedFileList[0], &filePath);
    printf("Starting set from file::%s\n", filePath);

    integerSet_t *mainSet = newSetFromFile(filePath);
    integerSet_t *otherSet, *bufferSet;
    setPrint(mainSet);
    free(filePath);
    for (int s = 1; s < inCnt; s++) {
        n = constructFilePath(fileLocation, includedFileList[s], &filePath);    
#ifdef DEBUG
        printf("Reading in file[%d] :%s\n", s, filePath);
#endif  
        otherSet  = newSetFromFile(filePath);      
#ifdef DEBUG
        setPrint(otherSet);
#endif    
       // setPrint(otherSet);
        bufferSet = setIntersect(mainSet, otherSet);
        moveSet(bufferSet, mainSet);
        free(filePath);
        
#ifdef DEBUG
        printf("Current intersection set\n");
        setPrint(mainSet);
#endif
    }

#ifdef DEBUG
        printf("Final intersection set\n");
        setPrint(mainSet);
#endif   

    for (int s = 0; s < notInCnt; s++) {
        printf("NOTIN %d :%s\n", s, notIncludedFileList[s]);
        n = constructFilePath(fileLocation, notIncludedFileList[s], &filePath);
        
        otherSet  = newSetFromFile(filePath);
       // setPrint(otherSet);
        bufferSet = setSubstract(mainSet, otherSet);
        moveSet(bufferSet, mainSet);
        free(filePath);
    }

    printf("ALL SUBSTRACT COMP DONE");
    setPrint(mainSet);

/*
    int32_t list[11] = { 2,5, 7, 8, 10, 15, 22, 89, 333, 100002, 69022343222 };
    int32_t list2[11] = { 0, 5, 7, 22, 104, 333};
    
    printf("Looking for %d\n", guestValue);
    int index = dichotomicSearch(&list, 11, guestValue);
    printf("-->%d\n", index);
*/


    return 0;
}