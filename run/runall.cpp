#include<iostream>
#include<cstdlib>
#include<cstdio>

char* benchmarks[] = { "custom1", "custom2", "s713", "s953", "s1423", "s5378", "s9234"};
#define SIZE 6

using namespace std;

int main()
{
    // Generating single pattern per fault (No fault collapsing)
    char command[1024];
    char* ROOT_DIR = "/home/frathoso/Dropbox/ASpring_2014/Capstone/atpg";

    for(int K = 0; K < SIZE; K++)
    {
        break;

        sprintf(command, "%s/bin/atpg -D 1 -X R -b %s/benchmarks/%s.bench -Z -f %s/benchmarks/%s.flt > %s/run/single_%s_atpg.txt",
                ROOT_DIR, ROOT_DIR, benchmarks[K], ROOT_DIR, benchmarks[K], ROOT_DIR, benchmarks[K]);


        system(command);

        sprintf(command, "wine %s/bin/atalanta.exe %s/benchmarks/%s.bench -Z -l %s/run/single_%s_atalanta.txt -f %s/benchmarks/%s.flt > %s/run/results.txt",
                ROOT_DIR, ROOT_DIR, benchmarks[K], ROOT_DIR, benchmarks[K], ROOT_DIR, benchmarks[K], ROOT_DIR);
        system(command);
    }

    // Perform fault collapsing
    for(int K = 0; K < SIZE; K++)
    {

        sprintf(command, "%s/bin/atpg -D 1 -X R -b %s/benchmarks/%s.bench -f %s/benchmarks/%s.flt > %s/run/comp_%s_atpg.txt",
                ROOT_DIR, ROOT_DIR, benchmarks[K], ROOT_DIR, benchmarks[K], ROOT_DIR, benchmarks[K]);
        //system(command);

        sprintf(command, "wine %s/bin/atalanta.exe %s/benchmarks/%s.bench -l %s/run/comp_%s_atalanta.txt -f %s/benchmarks/%s.flt > %s/run/results.txt",
                ROOT_DIR, ROOT_DIR, benchmarks[K], ROOT_DIR, benchmarks[K], ROOT_DIR, benchmarks[K], ROOT_DIR);
        system(command);
    }

    return 0;
}
