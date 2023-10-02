#include <iostream>
#include <csignal>
#include <array>
#include <vector>
#include <sstream>
#include <cstring>
#include <wait.h>

auto BLUE_TEXT = "\033[34m";
auto YELLOW_TEXT = "\033[33m";
const char *GREEN_TEXT = "\033[32m";
const char *WHITE_TEXT = "\033[0m";


constexpr size_t MAX_ARGS = 256;
constexpr size_t MAX_ARG_LEN = 1024;

void printCurrentDirectory() {
    std::array<char, MAX_ARG_LEN> cwd{};
    std::array<char, 32> username{};
    std::array<char, 32> hostname{};

    getlogin_r(username.data(), username.size());
    gethostname(hostname.data(), hostname.size());
    getcwd(cwd.data(), cwd.size());

    std::cout << YELLOW_TEXT << username.data() << "@" << hostname.data()
              << BLUE_TEXT << " ~" << cwd.data() << " $ " << WHITE_TEXT;
}

std::array<char *, MAX_ARGS> split(const std::string &str, const char delimiter, size_t &count) {
    std::istringstream iss(str);
    std::array<char *, MAX_ARGS> result{};
    std::string item;
    count = 0;

    while (std::getline(iss, item, delimiter) && count < MAX_ARGS) {
        auto cstr = new char[item.length() + 1];
        std::strcpy(cstr, item.c_str());
        result[count++] = cstr;
    }
    return result;
}

bool parse(const std::string &input, std::array<char *, MAX_ARGS> &parsedArgs, size_t &parsedArgsCount,
           std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t &parsedPipedArgsCount) {
    parsedPipedArgs = split(input, '|', parsedPipedArgsCount);
    if (parsedPipedArgsCount > 0) {
        parsedArgs = split(parsedPipedArgs[0], ' ', parsedArgsCount);
        if (parsedPipedArgsCount > 1) {
            parsedPipedArgs = split(parsedPipedArgs[1], ' ', parsedPipedArgsCount);
        }
        return parsedPipedArgsCount > 1;
    } else {
        parsedArgs = split(input, ' ', parsedArgsCount);
        return false;
    }
}

void execArgs(std::array<char *, MAX_ARGS> &parsed) {
    pid_t pid = fork();

    if (pid == -1) {
        std::cout << "\nFailed forking child.." << std::endl;
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed.data()) < 0) {
            std::cout << "\nCould not execute command.." << std::endl;
        }
        exit(0);
    } else {
        wait(nullptr);
        return;
    }
}

void execArgsPiped(std::array<char *, MAX_ARGS> &parsed, size_t parsedCount,
                   std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t parsedPipedArgsCount) {
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        std::cout << "\nPipe could not be initialized" << std::endl;
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        std::cout << "\nCould not fork" << std::endl;
        return;
    }

    if (p1 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        parsed[parsedCount] = nullptr;
        if (execvp(parsed[0], parsed.data()) < 0) {
            std::cout << "\nCould not execute command 1.." << std::endl;
            exit(0);
        }
    } else {
        p2 = fork();

        if (p2 < 0) {
            std::cout << "\nCould not fork" << std::endl;
            return;
        }

        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            parsedPipedArgs[parsedPipedArgsCount] = nullptr;
            if (execvp(parsedPipedArgs[0], parsedPipedArgs.data()) < 0) {
                std::cout << "\nCould not execute command 2.." << std::endl;
                exit(0);
            }
        } else {
            wait(nullptr);
            wait(nullptr);
        }
    }
}

int main() {
    std::string input;
    std::array<char *, MAX_ARGS> parsedArgs{}, parsedPipedArgs{};
    size_t parsedArgsCount = 0, parsedPipedArgsCount = 0;

    while (true) {
        printCurrentDirectory();
        std::getline(std::cin, input);
        bool isPiped = parse(input, parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount);
        if (isPiped) {
            execArgsPiped(parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount);
        } else {
            execArgs(parsedArgs);
        }
        if (input == "exit") {
            for (size_t i = 0; i < parsedArgsCount; ++i) {
                delete[] parsedArgs[i];
            }
            for (size_t i = 0; i < parsedPipedArgsCount; ++i) {
                delete[] parsedPipedArgs[i];
            }
            break;
        }
    }
    return 0;
}






