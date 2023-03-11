#include <iostream>
#include <vector>
#include <unordered_set>
#include <cstdlib>
#include <ctime>

using namespace std;

void getNormalArr(unsigned int *array, unsigned int n)
{
    for(unsigned int i = 0; i < n - 1; ++i)
        array[i] = i + 1;

    array[n - 1] = 0;
}

void getReverseArr(unsigned int *array, unsigned int n)
{
    for(unsigned int i = n - 1; i > 0; --i)
        array[i] = i - 1;

    array[0] = n - 1;
}

void getRandomArr(unsigned int *array, unsigned int n)
{
    vector<int> vec(n-1);
    for (int i = 0; i < n-1; ++i) vec[i] = i + 1;

    int oldIndex = 0;
    for (int i = 0; i < n-1; ++i)
    {
        int index = rand() % vec.size();
        int val = vec[index];
        vec.erase(vec.begin() + index);
        array[oldIndex] = val;
        oldIndex = val;
    }
}

unsigned int parsIndex = 0;

void runParsing(const unsigned int *array, unsigned int &size, unsigned int &k)
{
    for(unsigned int j = 0; j < size * k; ++j)
        parsIndex = array[parsIndex];
}

union ticks
{
    long long t64;
    struct s32
    {
        long th, tl;
    } t32;
} start0, end0;

int main(int argc, char **argv)
{
    srand(time(0));

    unsigned int n = 256; //256 * 4 = 1024 b = 1kb
    unsigned int k = 5;
    unsigned long long parsTime;
    unsigned int pr = 1;

    for (unsigned int i = 0; i < 16; ++i, n*=2)
    {
        cout << n << endl;
        unsigned int *array = (unsigned int *) malloc(n * sizeof(unsigned int));
        getNormalArr(array, n);
        runParsing(array, n, pr);

        getNormalArr(array, n);
        asm("rdtsc\n":"=a"(start0.t32.th), "=d"(start0.t32.tl));
        runParsing(array, n, k);
        asm("rdtsc\n":"=a"(end0.t32.th), "=d"(end0.t32.tl));
        parsTime = end0.t64 - start0.t64;
        cout << "Normal parsing: " << parsTime / (k * n) << endl;
        free(array);

        array = (unsigned int *) malloc(n * sizeof(unsigned int));
        getNormalArr(array, n);
        runParsing(array, n, pr);

        getReverseArr(array, n);
        asm("rdtsc\n":"=a"(start0.t32.th), "=d"(start0.t32.tl));
        runParsing(array, n, k);
        asm("rdtsc\n":"=a"(end0.t32.th), "=d"(end0.t32.tl));
        parsTime = end0.t64 - start0.t64;
        cout << "Reverse parsing: " << parsTime / (k * n) << endl;
        free(array);

        array = (unsigned int *) malloc(n * sizeof(unsigned int));
        getNormalArr(array, n);
        runParsing(array, n, pr);

        getRandomArr(array, n);
        asm("rdtsc\n":"=a"(start0.t32.th), "=d"(start0.t32.tl));
        runParsing(array, n, k);
        asm("rdtsc\n":"=a"(end0.t32.th), "=d"(end0.t32.tl));
        parsTime = end0.t64 - start0.t64;
        cout << "Random parsing: " << parsTime / (k * n) << endl;
        free(array);

        cout << endl;
    }
    return 0;
}
