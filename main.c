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
#include "custom_transform.h"
#include <stdbool.h>
#include <assert.h>

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

void freeFilePath(char **includedFileList, int inCnt, char **notIncludedFileList, int notInCnt) {
    int i;
    for (i = 0; i < inCnt ; i++)
        free(includedFileList[i]);
    free(includedFileList);
    for (i = 0; i < notInCnt ; i++)
        free(notIncludedFileList[i]);
    free(notIncludedFileList);
}
   

int main (int argc, char *argv[]) {

    #ifdef DEBUG
    printf( "*** Debug Mode***\n");
    #endif
    int c, n;
    int inCnt = 0;
    int notInCnt = 0;
    int iSuffLength = 0;
    int jSuffLength = 0;
    bool doProject = false;
    uint64_t (*truncFn)(uint64_t, int, int, int);
    char **includedFileList = NULL;
    char **notIncludedFileList = NULL;
    //int guestValue = 0;
    int s_start = 1;
    //char *includedFileArg = NULL;
    //char *notIncludedFileArg = NULL;
    
    char *fileLocation = NULL;
    char *filePath = NULL; 
    char *fileExtension = NULL; 
    char *fileOut = NULL;
    char *fileSeedPath = NULL;
    FILE *fpOut = NULL;
    char codec[1024];
    const char    *short_opt = "hi:o:l:v:e:f:s:d:c:";
    struct option   long_opt[] =
    {
        {"help",                 no_argument, NULL, 'h'},          
        {"codom",          required_argument, NULL, 'c'},       
        {"seed",           required_argument, NULL, 's'},       
        {"in",             required_argument, NULL, 'i'},
        {"notin",          required_argument, NULL, 'o'},
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

            case 'c':                
                jSuffLength = atoi(optarg);
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
            case 's':
                fileSeedPath = strdup(optarg);
                break;
            case 'h':
                printf("Performing comparaison ensemble on set of intefger keys");
                printf("-h, --help                print this help and exit\n");
                printf("\n");
                return(0);
      //      case ':':
            case '?':
                fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
                return(-2);

            default:
                fprintf(stderr, "%s: invalid option -- %c\n", argv[0], c);
                fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
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

    integerSet_t *_mainSet, *_otherSet;
    integerSet_t *mainSet = newSetFromFile(filePath, &iSuffLength, codec);
    free(filePath);
    
    #ifdef DEBUG
    printf("CODEC INFO >%d %s<\n", iSuffLength, codec);
    #endif
    assert (strcmp(codec, "pow2") == 0 || strcmp(codec, "twobits")  == 0 );
    truncFn = strcmp(codec, "pow2") == 0 ? &customTruncate : &truncateBinaryLeftWrapper;
    
    if (jSuffLength != 0) {

        if ( iSuffLength < jSuffLength  || iSuffLength * jSuffLength < 0 ||  iSuffLength + jSuffLength < 0 ||
            (iSuffLength + jSuffLength > 0  && iSuffLength * jSuffLength == 0)  // One defined on cli and not the other
            ) {
            fprintf(stderr, "irregular suffix length {from:%d, to:%d}\n", iSuffLength, jSuffLength);
            exit(1);
        }
        doProject = true;
    }

    #ifdef DEBUG
        printf("Initial Set Content\n");
        setPrint(mainSet, stdout);
    #endif

    if (doProject) {
        _mainSet = mainSet;     
        mainSet = project(_mainSet, iSuffLength, jSuffLength, 4, truncFn) ;
        destroySet(_mainSet);         
    }
  

    integerSet_t *otherSet, *bufferSet;
    for (int s = s_start; s < inCnt; s++) {
        n = constructFilePath(fileLocation, includedFileList[s], fileExtension, &filePath);    
        #ifdef DEBUG
            printf("[in] Reading file[%d] :%s\n", s, filePath);
        #endif
        int currSuffLength;
        char currCodec[1024]; 
        otherSet  = newSetFromFile(filePath, &currSuffLength, currCodec);
        assert(strcmp(currCodec, codec) == 0);
        assert(currSuffLength == iSuffLength);
        if(doProject) {
            _otherSet = otherSet;
            otherSet = project(_otherSet, iSuffLength, jSuffLength, 4, truncFn) ;
            destroySet(_otherSet);  
        }      
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
        
    }

        #ifdef DEBUG
            printf("Final intersection set\n");
            setPrint(mainSet, stdout);
            printf("Input [notin] files number %d\n", notInCnt);
        #endif   

   
    for (int s = 0; s < notInCnt; s++) {
        if(mainSet->size == 0){
            fprintf(stderr, "intersect size is zero, early exit\n");
            break;
        }
            

        n = constructFilePath(fileLocation, notIncludedFileList[s], fileExtension, &filePath);
        #ifdef DEBUG
            printf("[notin] Reading file[%d] :%s\n", s, filePath);
        #endif  
        otherSet  = newSetFromFile(filePath, &iSuffLength, codec);
         if(doProject) {
            _otherSet = otherSet;
            otherSet = project(_otherSet, iSuffLength, jSuffLength, 4, truncFn) ;
            destroySet(_otherSet);  
        }   
        bufferSet = setSubstract(mainSet, otherSet);
        moveSet(bufferSet, mainSet);
        destroySet(otherSet);
        free(filePath);

        if(doProject) {
            //_mainSet, _otherSet
        }
    }

/*
    int32_t list[11] = { 2,5, 7, 8, 10, 15, 22, 89, 333, 100002, 69022343222 };
    int32_t list2[11] = { 0, 5, 7, 22, 104, 333};
    
    printf("Looking for %d\n", guestValue);
    int index = dichotomicSearch(&list, 11, guestValue);
    printf("-->%d\n", index);
*/

    fpOut = fopen(fileOut, "w");
    fprintf(fpOut, "{ \"comments\" : \"Final set (Intersect of %d sets) - (Union of %d sets)\", ", inCnt, notInCnt);
    fprintf(fpOut, "\"size\" : %d, \"codec\" : \"%s\", " , mainSet->size, codec );
    if (doProject) 
        fprintf(fpOut, "\"from\" : %d, \"to\" : %d }\n", iSuffLength, jSuffLength );
    else
        fprintf(fpOut, "\"length\" : %d }\n", iSuffLength);

#ifdef DEBUG  
    fprintf(stderr, "Trying to rank %d\n", mainSet->size);
#endif    
    
    rankSetQ(mainSet);
    if(doProject)
        tidySet(mainSet);

    setPrint(mainSet, fpOut);
    fclose(fpOut);
    
    //Cleaning
    destroySet(mainSet);
    freeFilePath(includedFileList, inCnt, notIncludedFileList, notInCnt);
    if (fileLocation != NULL)
        free(fileLocation);
    if (fileExtension != NULL)
        free(fileExtension);
    if (fileOut != NULL)
        free(fileOut);
    if (fileSeedPath != NULL)
        free(fileSeedPath);
    

    return 0;
}
