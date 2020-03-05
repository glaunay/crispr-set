#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>

unsigned int64_t bitEncode(char *word) {
    int64_t value = 32;
    printf("2-Bit encoding of a %lu length\n", strlen(word)) ;
    
    return value;
}


void binprintf(unsigned int64_t v)
{
    unsigned int64_t mask=1<<((sizeof(int64_t)<<3)-1);
    while(mask) {
        printf("%d", (v&mask ? 1 : 0));
        mask >>= 1;
    }
}



int main(int argc, char *argv[]){
    int c;
    const char    *short_opt = "hi:o:l:v:e:f:s:d:c:";
    struct option   long_opt[] =
    {
        {"help",                 no_argument, NULL, 'h'},
        {"dom",            required_argument, NULL, 'd'}
    };
    
    char *stringToEncode = NULL;
    while((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch(c) {
            case -1:       /* no more arguments */
            case 0:        /* long options toggles */
            break;

         /*   case 'v':
                guestValue = atoi(optarg);
                break;*/
            case 'd':                
                stringToEncode = strdup(optarg);
                break;
   
        }
    }
    printf("Encoding \"%s\"\n", stringToEncode);
    unsigned int64_t x = bitEncode(stringToEncode);
    return 1;
}
