#include "main.h"

bool parse(const std::string &input, std::array<char *, MAX_ARGS> &parsedArgs, size_t &parsedArgsCount,
           std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t &parsedPipedArgsCount) {
    parsedPipedArgs = split(input, " | ", parsedPipedArgsCount);
    if (parsedPipedArgsCount > 0) {
        parsedArgs = split(parsedPipedArgs[0], " ", parsedArgsCount);
        parsedPipedArgsCount -= 1;
        if (parsedPipedArgsCount > 1) {
            parsedPipedArgs = split(parsedPipedArgs[1], " ", parsedPipedArgsCount);
        }
        return parsedPipedArgsCount > 1;
    } else {
        parsedArgs = split(input, " ", parsedArgsCount);
        return false;
    }
}

pid_t forkAndExec(std::array<char *, MAX_ARGS> &parsed, int inFd = STDIN_FILENO, int outFd = STDOUT_FILENO) {
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

void execArgsPiped(std::array<char *, MAX_ARGS> &parsed,
                   std::array<char *, MAX_ARGS> &parsedPipedArgs) {
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

std::array<char *, MAX_ARGS> split(const std::string_view &str, const std::string &delimiter, size_t &count) {
    size_t pos_start = 0;
    size_t pos_end;
    size_t delim_len = delimiter.length();
    std::string token;
    std::array<char *, MAX_ARGS> result{};

    count = 0;
    while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
        token = str.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;

        auto dest = new char[token.length() + 1];
        std::strcpy(dest, token.c_str());
        result[count++] = dest;
    }

    token = (str.substr(pos_start));
    auto dest = new char[token.length() + 1];
    std::strcpy(dest, token.c_str());
    result[count++] = dest;

    return result;
}

void clear(std::array<char *, MAX_ARGS> &parsedArgs, size_t parsedArgsCount,
           std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t parsedPipedArgsCount) {
    for (size_t i = 0; i < parsedArgsCount; ++i) {
        delete[] parsedArgs[i];
    }
    for (size_t i = 0; i < parsedPipedArgsCount; ++i) {
        delete[] parsedPipedArgs[i];
    }
}

std::string printCurrentDirectory() {
    std::array<char, MAX_ARG_LEN> cwd{};
    std::array<char, 32> username{};
    std::array<char, 32> hostname{};

    getlogin_r(username.data(), username.size());
    gethostname(hostname.data(), hostname.size());
    getcwd(cwd.data(), cwd.size());

    return std::string(YELLOW_TEXT) + username.data() + "@" + hostname.data()
           + BLUE_TEXT + " ~" + cwd.data() + " $ " + WHITE_TEXT;
}

int main() {
    std::string input;
    std::array<char *, MAX_ARGS> parsedArgs{};
    std::array<char *, MAX_ARGS> parsedPipedArgs{};
    size_t parsedArgsCount = 0;
    size_t parsedPipedArgsCount = 0;

    while (true) {
        std::string dir = printCurrentDirectory();
        char const *line = readline(dir.c_str());
        input = std::string(line);

        if (!input.empty()) {
            add_history(input.c_str());
        }
        if (input == "exit") {
            clear(parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount);
            break;
        }

        if (parse(input, parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount)) {
            execArgsPiped(parsedArgs, parsedPipedArgs);
        } else {
            if (strcmp(parsedArgs[0], "cd") == 0) {
                if (chdir(parsedArgs[1]) < 0 && parsedArgsCount >= 2) {
                    perror("chdir");
                }
                continue;
            }
            execArgs(parsedArgs);
        }
    }
    return 0;
}
