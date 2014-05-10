#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE* fp = fopen("../benchmarks/s713.flt", "r");
    if(fp == NULL) return 0;
    char* line = NULL;
    size_t len = 0;
    char gate1[20], gate2[20];
    int single = 0, fault1 = -1, fault2 = -1;
    while((getline(&line, &len, fp)) != -1)
    {
        // Extract gate
        if(strstr(line, "->") == NULL)
        {
            sscanf(line, "%[a-z,A-Z,0-9]", gate1);
            single = 1;
        }
        else sscanf(line, "%[a-z,A-Z,0-9]->%[a-z,A-Z,0-9]", gate1, gate2);

        // Extract fault
        while(1)
        {
            if((line = strstr(line, "/")) == NULL) break;
            line++;

            printf("Line: %s\n", line);

            if(fault1 < 0)  sscanf(line, "%d", &fault1);
            else sscanf(line, "%d", &fault2);
        }
        break;
    }

    if(single) printf("%s ", gate1);
    else printf("%s->%s ", gate1, gate2);

    if(fault1 >= 0) printf(" /%d", fault1);
    if(fault2 >= 0) printf(" /%d", fault2);
    printf("\n");

    if(line) free(line);

    return 0;
}
