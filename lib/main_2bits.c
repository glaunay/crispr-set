
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
    const char    *short_opt = "a:b:t:c:h";
    struct option   long_opt[] =
    {
        {"help",                 no_argument, NULL, 'h'},
        {"sequenceA",            required_argument, NULL, 'a'},
        {"sequenceB",            required_argument, NULL, 'b'},
        {"FileForsequencesB",            required_argument, NULL, 't'},        
        {"Word length to truncate sequences",            required_argument, NULL, 'c'}
    };
    
    char *stringToEncodeQuery = NULL;
    char *stringToEncodeTarget = NULL;
    char *targetFile = NULL;
    int truncLen = 0; 
    uint64_t binaryQueryTrunc, binaryTargetTrunc;

    size_t strLenQuery = 0;
    size_t strLenTarget = 0;
    uint64_t binaryQuery, binaryTarget;
 
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
            case 'c':                
                truncLen = atoi(optarg);
                break;
        }
    }
    if (targetFile != NULL && stringToEncodeQuery !=  NULL) {       
        compareOneToFile(stringToEncodeQuery, targetFile);
        return(1);
    }



    //char *segment = "GCCGTGCTAAGCGTAACAACTTCAAATCCGCG";

    printf("Trying to encode \"%s\"\n", stringToEncodeQuery);
    strLenQuery;
    binaryQuery = encode(stringToEncodeQuery, &strLenQuery);

    printf("Encoded string of size %lu\n", strLenQuery);
    showbits(binaryQuery);
    printf("%llu\n", binaryQuery);
    char string[BIG_ENOUGH];
    decode(binaryQuery, string, false, strLenQuery);
    printf("%s\n", string);

    if (truncLen > 0) {
       printf("Truncating target to %d characters\n", truncLen);
        binaryQueryTrunc = truncateBinaryLeft   (binaryQuery, strLenQuery, truncLen);
        printf("%llu\n", binaryQueryTrunc);
        decode(binaryQueryTrunc, string, false, truncLen);
        showbits(binaryQueryTrunc);
        printf("%s\n", string);
    }


    if (stringToEncodeTarget != NULL) {
        printf("Trying to encode \"%s\"\n", stringToEncodeTarget);
        binaryTarget = encode(stringToEncodeTarget, &strLenTarget);
        assert(strLenTarget == strLenQuery);
        //printf("Encoded string of size %lu\n", strLenQuery);
        printf("%llu\n", binaryTarget);
        //char string[BIG_ENOUGH];
        decode(binaryTarget, string, false, strLenTarget);
        printf("%s\n", string);

        int d = hammingDistance(binaryQuery, binaryTarget, strLenQuery);

        printf("Counted %d missmatches\n", d);

        if (truncLen > 0) {
            printf("Truncating target to %d characters\n", truncLen);
            binaryTargetTrunc = truncateBinaryLeft(binaryTarget, strLenQuery, truncLen);
            printf("%llu\n", binaryTargetTrunc);
            decode(binaryTargetTrunc, string, false, truncLen);
            printf("%s\n", string);
            // All zero passed truncature, strlenquery as total number of comp proves it
            int d = hammingDistance(binaryQueryTrunc, binaryTargetTrunc, strLenQuery);
            printf("Truncated::counted %d missmatches\n", d);
        }

    }

    return 0;
}