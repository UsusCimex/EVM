#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(0));
    size_t N = 1e8; 
    size_t M = 0;
    // scanf("%d", &N);

    //START_TIME
    clock_t start = clock();

    long double x,y;
    //Метод Монте-Карло
    for (int i = 0; i < N; i++)
    { 
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;

        if (x*x + y*y <= 1)
        {
            M++;
        }
    }

    //END_TIME
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;

    printf("%lf\n", 4.0 * M / N);
    printf("Total time: %lf seconds\n", seconds);
    
    return 0;
}
