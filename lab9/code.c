#include <iostream>
#include <vector>
using namespace std;

union ticks 
{
    long long t64;
    struct s32
    {
        long th, tl;
    } t32;
} start0, end0;

unsigned int index = 0;
unsigned int repeat = 2;
long long getTicks(int *arr, unsigned int& full_offset_size){
    asm("rdtsc\n":"=a"(start0.t32.th), "=d"(start0.t32.tl));
    for (int i = 0;  i < full_offset_size * repeat;  i++)
    {
        index = arr[index];
    }
    asm("rdtsc\n":"=a"(end0.t32.th), "=d"(end0.t32.tl));
    return (end0.t64 - start0.t64) / full_offset_size / repeat;
}

unsigned int fragment = 1;
int main(int argc,char *argv[]) 
{
    int OFFSET = 1024*1024*16; //16Mb
    for(int N = 1; N <= 32; N++)
    {
        unsigned int elem_size = OFFSET / (N * sizeof(unsigned int));
        unsigned int full_offset_size = OFFSET / sizeof(unsigned int) * N;
        int *arr = new int[full_offset_size];
        for(int i = 0; i < N; i++)
        {
            for(int j = 0; j < elem_size;  j++) 
            {
                arr[i * OFFSET / sizeof(int) + j] =(i+1) * OFFSET / sizeof(int) + j;
            }
        }
        for(int i = 0; i < elem_size; i++) 
        {
            arr[(N - 1) * OFFSET / sizeof(int) + i] = i +1;
        }
        cout << fragment++ << " fragment : "<< getTicks(arr, full_offset_size) << endl;
        delete[] arr;
    }
    return 0;
}
