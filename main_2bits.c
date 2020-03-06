
#include <getopt.h>
#include "two_bits_encoding.h"



void compareOneToFile(char *queryString, char *targetFile){
    FILE *fp = fopen(targetFile, "r");

    size_t strLenQuery;
    uint64_t binaryQuery = encode(queryString, &strLenQuery);

    size_t strLenCurr;
    uint64_t binaryCurr; 
    int bufferLength = 255;
    int d;
    char currString[bufferLength];
    int *distValues = malloc(sizeof(int) * strLenQuery + 1); 
    for ( size_t i = 0 ; i <= strLenQuery ; i++){
        distValues[i] = 0;
    }
    while(fgets(currString, bufferLength, fp)) {
        currString[strcspn(currString, "\n")] = 0;
        binaryCurr = encode(currString, &strLenCurr);
        assert(strLenCurr == strLenCurr);
        d = hammingDistance(binaryQuery, binaryCurr, strLenQuery);
        distValues[d]++;
        //printf("########\nD=%d\n%s\n%s\n", d, queryString, currString);
    }

    fclose(fp);
    for ( size_t i = 0 ; i <= strLenQuery ; i++){
        printf("%zu:%d\n", i, distValues[i]);
    } 
    free(distValues);
}


int main(int argc, char *argv[]) {
    int c;
    const char    *short_opt = "a:b:t:h";
    struct option   long_opt[] =
    {
        {"help",                 no_argument, NULL, 'h'},
        {"sequenceA",            required_argument, NULL, 'a'},
        {"sequenceB",            required_argument, NULL, 'b'},
        {"FileForsequencesB",            required_argument, NULL, 't'}
    };
    
    char *stringToEncodeQuery = NULL;
    char *stringToEncodeTarget = NULL;
    char *targetFile = NULL;

    while((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch(c) {
            case -1:       /* no more arguments */
            case 0:        /* long options toggles */
            break;
            case 'a':                
                stringToEncodeQuery = strdup(optarg);
                break;
            case 'b':                
                stringToEncodeTarget = strdup(optarg);
                break;
            case 't':                
                targetFile = strdup(optarg);
                break;
   
        }
    }
    if (targetFile != NULL && stringToEncodeQuery !=  NULL) {       
        compareOneToFile(stringToEncodeQuery, targetFile);
        return(1);
    }



    //char *segment = "GCCGTGCTAAGCGTAACAACTTCAAATCCGCG";

    printf("Trying to encode \"%s\"\n", stringToEncodeQuery);
    size_t strLenQuery;
    uint64_t binaryQuery = encode(stringToEncodeQuery, &strLenQuery);

    printf("Encoded string of size %lu\n", strLenQuery);
    printf("%llu\n", binaryQuery);
    char string[BIG_ENOUGH];
    decode(binaryQuery, string, false, strLenQuery);
    printf("%s\n", string);

    if (stringToEncodeTarget != NULL) {
        printf("Trying to encode \"%s\"\n", stringToEncodeTarget);
        size_t strLenTarget;
        uint64_t binaryTarget = encode(stringToEncodeTarget, &strLenTarget);
        assert(strLenTarget == strLenQuery);
        //printf("Encoded string of size %lu\n", strLenQuery);
        printf("%llu\n", binaryTarget);
        //char string[BIG_ENOUGH];
        decode(binaryTarget, string, false, strLenTarget);
        printf("%s\n", string);

        int d = hammingDistance(binaryQuery, binaryTarget, strLenQuery);

        printf("Counted %d missmatches\n", d);
    }

    return 0;
}