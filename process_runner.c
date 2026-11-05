#include "process_runner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/wait.h>
#endif

static char* duplicate_string(const char* src) {
    char* dst = (char*)malloc(strlen(src) + 1);
    if (dst) strcpy(dst, src);
    return dst;
}

void process_init(struct BackgroundProcess* proc, const char* executable, const char** args, int arg_count) {
    proc->executable_path = duplicate_string(executable);
    proc->arg_count = arg_count;
    
    proc->arguments = (char**)malloc((arg_count + 2) * sizeof(char*));
    proc->arguments[0] = duplicate_string(executable);
    for (int i = 0; i < arg_count; ++i) {
        proc->arguments[i + 1] = duplicate_string(args[i]);
    }
    proc->arguments[arg_count + 1] = NULL;

#ifdef _WIN32
    proc->hProcess = NULL;
    proc->hThread = NULL;
#else
    proc->pid = -1;
#endif
}

bool process_start(struct BackgroundProcess* proc) {
#ifdef _WIN32
    int cmd_len = strlen(proc->executable_path) + 1;
    for (int i = 0; i < proc->arg_count; ++i) {
        cmd_len += strlen(proc->arguments[i + 1]) + 1;
    }
    
    char* cmd = (char*)malloc(cmd_len * sizeof(char));
    strcpy(cmd, proc->executable_path);
    for (int i = 0; i < proc->arg_count; ++i) {
        strcat(cmd, " ");
        strcat(cmd, proc->arguments[i + 1]);
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        free(cmd);
        return false;
    }

    proc->hProcess = pi.hProcess;
    proc->hThread = pi.hThread;
    free(cmd);
    return true;
#else
    proc->pid = fork();
    if (proc->pid < 0) return false;
    
    if (proc->pid == 0) {
        execvp(proc->executable_path, proc->arguments);
        fprintf(stderr, "POSIX: Failed to execute process\n");
        exit(1);
    }
    return true;
#endif
}

int process_wait(struct BackgroundProcess* proc) {
#ifdef _WIN32
    if (!proc->hProcess) return -1;
    WaitForSingleObject(proc->hProcess, INFINITE);
    DWORD exit_code;
    if (GetExitCodeProcess(proc->hProcess, &exit_code)) {
        return (int)exit_code;
    }
    return -1;
#else
    if (proc->pid <= 0) return -1;
    int status;
    waitpid(proc->pid, &status, 0);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1;
#endif
}

void process_destroy(struct BackgroundProcess* proc) {
    if (proc->executable_path) free(proc->executable_path);
    if (proc->arguments) {
        for (int i = 0; i <= proc->arg_count; ++i) {
            if (proc->arguments[i]) free(proc->arguments[i]);
        }
        free(proc->arguments);
    }
    
#ifdef _WIN32
    if (proc->hProcess) CloseHandle(proc->hProcess);
    if (proc->hThread) CloseHandle(proc->hThread);
#endif
}