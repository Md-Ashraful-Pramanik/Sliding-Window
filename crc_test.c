#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//100100
//1101
//001

char *getCRC(char *input, char *generator_polynomial) {
    int generator_length = strlen(generator_polynomial);
    for (int i = 0; i < (generator_length - 1); ++i)
        strcat(input, "0");

    int input_length = strlen(input);

    char *remainder = (char *) malloc(generator_length * sizeof(char));
    for (int i = 0; i < (generator_length-1); ++i)
        remainder[i+1] = input[i];

    for (int i = generator_length; i <= input_length; ++i) {
        for (int j = 1; j < generator_length; ++j)
            remainder[j - 1] = remainder[j];

        remainder[generator_length - 1] = input[i-1];

        if (remainder[0] == generator_polynomial[0]) {
            for (int j = 0; j < generator_length; ++j) {
                if (remainder[j] == generator_polynomial[j])
                    remainder[j] = '0';
                else
                    remainder[j] = '1';
            }
        }

    }

    for (int j = 1; j < generator_length; ++j)
        remainder[j - 1] = remainder[j];

    remainder[generator_length - 1] = '\0';
    input[input_length - generator_length + 1] = '\0';

    return remainder;
}

char* get_bit_string(char c){
    char *str=(char *)malloc(sizeof(char )*9);
    for (int i = 0; i < 8; ++i) {
        if(c&(1<<(7-i)))
            str[i]='1';
        else
            str[i]='0';
    }
    str[8] = '\0';
    return str;
}

int string_to_int(char *str){
    int length = strlen(str);
    int result = 0;
    for (int i = 0; i < length; ++i) {
        if(str[i]=='1')
            result|=(1<<(length-1-i));
    }
    return result;
}

int main() {
    char *generator_polynomial = "1101";
    int generator_length = strlen(generator_polynomial);
    char *input = (char *) malloc((100) * sizeof(char));
    strcpy(input, "100100");

    char *remainder = getCRC(input, generator_polynomial);
    printf("%s\n", input);
    printf("%s\n", generator_polynomial);
    printf("%s\n", remainder);

    free(remainder);
    free(input);
//
//    char* str = get_bit_string(37);
//    printf("%s\n", str);
//    printf("%d", string_to_int(str));
//
//    free(str);
    return 0;
}

00000001000000000000000001011010011000010110000101100001
00000001000000000000000001011010011000010110000101100001
