#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace std;

int main()
{
    char filename[100];
    time_t now = time(NULL);
    sprintf(filename, "resulting_running_time_%s.txt", ctime(&now));
    freopen(filename, "w", stderr);

    char* benchFiles[] = {
        "s713.bench","s953.bench", "s5378.bench","s9234.bench","s13207.bench",
        "s1423.bench","s15850.bench","s35932.bench","s38417.bench","s38584.bench",
    };

    char command[256];
    clock_t start, stop;
    for(int K=0; K<sizeof(benchFiles)/sizeof(benchFiles[0]); K++)
    {
        fprintf(stdout, "Running circuit %s ... ", benchFiles[K]);
        fprintf(stderr, "Running circuit %s ... ", benchFiles[K]);
        time(&start);
        sprintf(command, "../atpg -f ../bench/%s", benchFiles[K]);
        system(command);
        time(&stop);

        fprintf(stdout, "[%.0f seconds]\n", difftime(stop, start));
        fprintf(stderr, "[%.0f seconds]\n", difftime(stop, start));
    }
    return 0;
}
