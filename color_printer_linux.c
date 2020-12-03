#include <stdio.h>

int main(int argc, char ** argv){

    for (int j = 1; j < 10; j++)
    {
        printf("\n\n\n%d Starts\n\n", j);
        for (int i = 0; i <= 8; i++)
            printf("\e[%d;%dm (%3d, %3d) This is sample text \e[0m \n",j, i, j, i);
        for (int i = 30; i <= 47; i++)
            printf("\e[%d;%dm (%3d, %3d) This is sample text \e[0m \n",j, i, j, i);
        for (int i = 90; i <= 107; i++)
            printf("\e[%d;%dm (%3d, %3d) This is sample text \e[0m \n",j, i, j, i);
    }
    
    return 0;
}