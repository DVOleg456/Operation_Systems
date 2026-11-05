#ifndef PROCESS_RUNNER_H
#define PROCESS_RUNNER_H

#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#endif

struct BackgroundProcess {
    char* executable_path;
    char** arguments;
    int arg_count;

#ifdef _WIN32
    HANDLE hProcess;
    HANDLE hThread;
#else
    pid_t pid;
#endif
};

void process_init(struct BackgroundProcess* proc, const char* executable, const char** args, int arg_count);
bool process_start(struct BackgroundProcess* proc);
int process_wait(struct BackgroundProcess* proc);
void process_destroy(struct BackgroundProcess* proc);

#endif // PROCESS_RUNNER_H