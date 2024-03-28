#include "prog.h"


void PerformComputation(useconds_t delay)
{
    // simulate main program performing computations by adding random delay 
    usleep(delay);
}


useconds_t RandomDelay(void)
{
    // minimum 100ms up to 600ms
    return 100 * 1000 + 100 * (rand() % 500);
}


int parse_args(int argc, char **argv)
{
    int nprocs = 0;
    if(argc != 2) {
        fprintf(stderr, "Alternate usage: %s <nprocs>\n", argv[0]);
        fprintf(stderr, "Default nprocs is set to %d\n", NPROCS);
        nprocs = NPROCS;
        return nprocs;
    }
    char *str;
    nprocs = (int) strtol(argv[1], &str, 10);
    
    if(*argv[1] == '\0' || *str != '\0'){
        fprintf(stderr, "%s is not a number, %s%s\n", argv[1], 
            (*argv[1] == '\0') ? "it is an empty string" : "it contains ", 
            (*argv[1] == '\0') ? "" : str);
        nprocs = NPROCS;
    }

    if(nprocs < 2 || nprocs > 5){
        fprintf(stderr, "%d is not a natural number in [1, 5]\n", nprocs);
        nprocs = NPROCS;
    }
    return nprocs;
}

int main(int argc, char **argv)
{
    
    int nprocs = parse_args(argc, argv);
    int numSnapshots = 0;
    int snapshot;

    printf("Options:\n");
    printf("Press ENTER for initiating a new snapshot\n");
    printf("Press q or Q for quitting the program\n\n");
    
    ProcessController processController = ProcessController(nprocs);

    printf("\nFinished spawning child processes\n");

    char line[256];
    while(true)
    {
        fgets(line, 255, stdin);
        bool isQuitting = line[0] == 'q' || line[0] == 'Q';
        bool isTakingSnapshot = line[0] == '\n';
        if(!(isQuitting || isTakingSnapshot))
        {
            continue;
        }
        processController.SendInitSnapshotMessage();
        snapshot = processController.TakeSnapshot();
        printf("GlobalState after snapshot%02d: %d\n", ++numSnapshots, snapshot);
        if(isTakingSnapshot)
        {
            processController.UnfreezeChildren();
        }
        if(isQuitting)
        {
            break;
        }
    }
    processController.SendQuitMessageToChildren();
    processController.WaitChildren();
    
    return 0;
}