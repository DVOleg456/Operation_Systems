#include <stdio.h>
#include <string.h>
#include "process_runner.h"

#ifdef _WIN32
#define SLEEP_SEC(sec) Sleep((sec) * 1000)
#else
#define SLEEP_SEC(sec) sleep(sec)
#endif

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--child_mode") == 0) {
        printf("[CHILD] Background process started.\n");
        SLEEP_SEC(3);
        printf("[CHILD] Exiting with code 42.\n");
        return 42;
    }

    printf("[MAIN] Testing cross-platform background runner.\n");
    struct BackgroundProcess bg_task;
    const char* args[] = {"--child_mode"};
    
    process_init(&bg_task, argv[0], args, 1);

    if (!process_start(&bg_task)) {
        fprintf(stderr, "[MAIN] Failed to start process!\n");
        process_destroy(&bg_task);
        return 1;
    }

    printf("[MAIN] Process running. Waiting...\n");
    int exit_code = process_wait(&bg_task);
    printf("[MAIN] Process finished with exit code: %d\n", exit_code);
    process_destroy(&bg_task);

    return 0;
}