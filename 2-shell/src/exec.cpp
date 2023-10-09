#include "../headers/exec.h"

pid_t forkAndExec(std::array<char *, MAX_ARGS> &parsed, int inFd, int outFd) {
    pid_t pid = fork();

    if (pid == -1) {
        std::cout << "Failed forking child.." << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if (inFd != STDIN_FILENO) {
            dup2(inFd, STDIN_FILENO);
            close(inFd);
        }
        if (outFd != STDOUT_FILENO) {
            dup2(outFd, STDOUT_FILENO);
            close(outFd);
        }
        if (execvp(parsed[0], parsed.data()) < 0) {
            std::cout << "shll: command not found.." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return pid;
}

void execArgs(std::array<char *, MAX_ARGS> &parsed) {
    pid_t pid = forkAndExec(parsed);
    waitpid(pid, nullptr, 0);
}

void execArgsPiped(std::array<char *, MAX_ARGS> &parsed, std::array<char *, MAX_ARGS> &parsedPipedArgs) {
    int pipefd[2];

    if (pipe(pipefd) < 0) {
        std::cout << "Pipe could not be initialized" << std::endl;
        exit(EXIT_FAILURE);
    }

    pid_t p1 = forkAndExec(parsed, STDIN_FILENO, pipefd[1]);
    close(pipefd[1]);

    pid_t p2 = forkAndExec(parsedPipedArgs, pipefd[0]);
    close(pipefd[0]);

    waitpid(p1, nullptr, 0);
    waitpid(p2, nullptr, 0);
}

void execArgsRedirect(std::array<char *, MAX_ARGS> &parsed, size_t parsedCount, bool inRedirect, bool outRedirect) {
    int inFd = STDIN_FILENO;
    int outFd = STDOUT_FILENO;
    if (inRedirect) {
        inFd = open(parsed[parsedCount - 1], O_RDONLY);
        parsed[parsedCount - 1] = nullptr;
    }
    if (outRedirect) {
        outFd = open(parsed[parsedCount - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        parsed[parsedCount - 1] = nullptr;
    }

    forkAndExec(parsed);
    if (inRedirect) {
        waitpid(inFd, nullptr, 0);
    }
    if (outRedirect) {
        waitpid(outFd, nullptr, 0);
    }
}

void execArgsBackground(std::array<char *, MAX_ARGS> &parsed) {
    forkAndExec(parsed);
}

bool isBackgroundTask(const std::array<char *, MAX_ARGS> &parsedArgs, size_t parsedArgsCount) {
    return parsedArgsCount > 0 && strcmp(parsedArgs[parsedArgsCount - 1], "&") == 0;
}
