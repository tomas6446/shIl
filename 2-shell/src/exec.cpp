#include "../headers/exec.h"

void execute(std::array<char *, MAX_ARGS> &parsed, pid_t pid) {
    if (pid == -1) {
        std::cout << "Failed forking child.." << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid == 0 && execvp(parsed[0], parsed.data()) < 0) {
        std::cout << "shll: command not found.." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void execArgs(std::array<char *, MAX_ARGS> &parsed) {
    pid_t pid = fork();

    execute(parsed, pid);
    waitpid(pid, nullptr, 0);

}

void execArgsPiped(std::array<char *, MAX_ARGS> &parsedArgs, std::array<char *, MAX_ARGS> &parsedPipedArgs) {
    int pipefd[2];

    if (pipe(pipefd) < 0) {
        std::cout << "Pipe could not be initialized" << std::endl;
        exit(EXIT_FAILURE);
    }

    pid_t p1 = fork();
    execute(parsedArgs, p1);
    close(pipefd[1]);
    waitpid(p1, nullptr, 0);

    pid_t p2 = fork();
    execute(parsedPipedArgs, p2);
    close(pipefd[0]);
    waitpid(p2, nullptr, 0);
}

void execArgsRedirect(std::array<char *, MAX_ARGS> &parsed) {

}

void execArgsBackground(std::array<char *, MAX_ARGS> &parsed) {
    pid_t pid = fork();
    execute(parsed, pid);
}

bool isBackgroundTask(const std::array<char *, MAX_ARGS> &parsedArgs, size_t parsedArgsCount) {
    return parsedArgsCount > 0 && strcmp(parsedArgs[parsedArgsCount - 1], "&") == 0;
}
