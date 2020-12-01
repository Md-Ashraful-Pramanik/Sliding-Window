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

        remainder[generator_length - 1] = input[i];

        if (remainder[0] == generator_polynomial[0]) {
            for (int j = 0; j < generator_length; ++j) {
                if (remainder[j] == generator_polynomial[j])
                    remainder[j] = '0';
                else
                    remainder[j] = '1';
            }
        }

    }

    remainder[generator_length - 1] = '\0';
    input[input_length - generator_length + 1] = '\0';

    return remainder;
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

    return 0;
}
