#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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


void freeStringList(char **stringList, int listLength) {
    for (int i = 0; i < listLength ; i++)
        free(stringList[i]);
    free(stringList);
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
