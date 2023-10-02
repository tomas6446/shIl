#include "main.h"

bool parse(std::string &input, std::array<char *, MAX_ARGS> &parsedArgs, size_t &parsedArgsCount,
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

void execArgs(char **parsedArgs) {
    pid_t pid = fork();

    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if (execvp(parsedArgs[0], parsedArgs) < 0) {
            printf("\nCould not execute command..");
        }
        exit(0);
    } else {
        wait(nullptr);
        return;
    }
}

// Function where the piped system commands is executed
void execArgsPiped(char **parsedArgs, char **parsedArgsPiped) {
    int pipefd[2];
    pid_t p2;
    pid_t p1;

    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return;
    }

    if (p1 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsedArgs[0], parsedArgs) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    } else {
        p2 = fork();

        if (p2 < 0) {
            printf("\nCould not fork");
            return;
        }
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedArgsPiped[0], parsedArgsPiped) < 0) {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        } else {
            wait(nullptr);
            wait(nullptr);
        }
    }
}

std::array<char *, MAX_ARGS> split(const std::string &str, const std::string &delimiter, size_t &count) {
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
            execArgsPiped(parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount);
        } else {
            execArgs(parsedArgs, parsedArgsCount);
        }
    }
    return 0;
}






