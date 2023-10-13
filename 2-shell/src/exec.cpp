#include "../headers/exec.h"

std::pair<std::vector<std::pair<char, int>>, std::array<char *, MAX_ARGS>> findRedirectionSymbolsAndCleanArgs(std::array<char *, MAX_ARGS> &args) {
    std::vector<std::pair<char, int>> redirections;
    std::array<char *, MAX_ARGS> cleanedArgs{};
    int j = 0;

    for (int i = 0; args[i] != nullptr; i++) {
        if (strcmp(args[i], "<") == 0) {
            redirections.emplace_back('<', i);
        } else if (strcmp(args[i], ">") == 0) {
            redirections.emplace_back('>', i);
        } else if (redirections.empty() || (i != redirections.back().second && i != redirections.back().second + 1)) {
            cleanedArgs[j++] = args[i];
        }
    }

    return {redirections, cleanedArgs};
}


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
    auto [redirections, cleanedArgs] = findRedirectionSymbolsAndCleanArgs(parsed);
    for (auto const [key, value]: redirections) {
        if (value != -1) {
            execArgsRedirect(cleanedArgs, redirections);
        } else {
            execute(parsed, pid);
            waitpid(pid, nullptr, 0);
        }
    }
}

void execArgsPiped(std::array<char *, MAX_ARGS> &parsedArgs, std::array<char *, MAX_ARGS> &parsedPipedArgs) {
    int pipefd[2];

    if (pipe(pipefd) < 0) {
        std::cout << "Pipe could not be initialized" << std::endl;
        exit(EXIT_FAILURE);
    }
    auto [redirections1, cleanedParsedArgs] = findRedirectionSymbolsAndCleanArgs(parsedArgs);
    for (auto const [key, value]: redirections1) {
        if (value != -1) {
            execArgsRedirect(cleanedParsedArgs, redirections1);
        } else {
            pid_t p1 = fork();
            execute(parsedArgs, p1);
            close(pipefd[1]);
            waitpid(p1, nullptr, 0);
        }
    }
    auto [redirections2, cleanedParsedPipedArgs] = findRedirectionSymbolsAndCleanArgs(parsedPipedArgs);
    for (auto const [key, value]: redirections2) {
        if (value != -1) {
            execArgsRedirect(cleanedParsedPipedArgs, redirections2);
        } else {
            pid_t p2 = fork();
            execute(parsedPipedArgs, p2);
            close(pipefd[0]);
            waitpid(p2, nullptr, 0);
        }
    }
}

void execArgsRedirect(std::array<char *, MAX_ARGS> &parsed, const std::vector<std::pair<char, int>> &redirections) {
    pid_t pid = fork();
    if (pid == 0) {
        for (auto const [key, value]: redirections) {
            if (key == '<') {
                int inFd = open(parsed[value + 1], O_RDONLY);
                if (inFd == -1) {
                    perror("Error opening input file");
                    exit(EXIT_FAILURE);
                }
                dup2(inFd, STDIN_FILENO);
                close(inFd);
            } else if (key == '>') {
                int outFd = open(parsed[value + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (outFd == -1) {
                    perror("Error opening output file");
                    exit(EXIT_FAILURE);
                }
                dup2(outFd, STDOUT_FILENO);
                close(outFd);
            }
        }
        execute(parsed, pid);
    }
    waitpid(pid, nullptr, 0);
}

void execArgsBackground(std::array<char *, MAX_ARGS> &parsed) {
    pid_t pid = fork();
    execute(parsed, pid);
}

bool isBackgroundTask(const std::array<char *, MAX_ARGS> &parsedArgs, size_t parsedArgsCount) {
    return parsedArgsCount > 0 && strcmp(parsedArgs[parsedArgsCount - 1], "&") == 0;
}
