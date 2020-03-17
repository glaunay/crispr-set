//File: io_utils.h
#ifndef IO_UTILS_H
#define IO_UTILS_H

char *strMemCopy(char *src);
char **parseFileArg(char *inputString, int *cnt);
int constructFilePath(char *dirLocation, char *fileName, char *fileExtension, char **filePath);
void freeStringList(char **stringList, int listLength);
int *createZeroIntArray(size_t size);

#endif
