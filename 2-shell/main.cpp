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

void printCurrentDirectory() {
    std::array<char, 1024> cwd{};
    std::array<char, 32> username{};
    std::array<char, 32> hostname{};

    getlogin_r(username.data(), username.size());
    gethostname(hostname.data(), hostname.size());
    getcwd(cwd.data(), cwd.size());

    std::cout
            << YELLOW_TEXT << username.data() << "@" << hostname.data()
            << BLUE_TEXT << " ~" << cwd.data() << " $ " << WHITE_TEXT;
}


std::vector<char *> split(const std::string &str, const char delimiter) {
    if (str.find(delimiter) == std::string::npos) {
        return {};
    }

    std::istringstream iss(str);
    std::vector<char *> result;
    std::string item;

    while (std::getline(iss, item, delimiter)) {
        auto cstr = new char[item.length() + 1];
        std::strcpy(cstr, item.c_str());
        result.push_back(cstr);
    }

    return result;
}

void parse(const std::string &input, std::vector<char *> &parsedArgs, std::vector<char *> &parsedPipedArgs) {
    parsedPipedArgs = split(input, '|');
    if (!parsedPipedArgs.empty()) {
        parsedArgs = split(parsedPipedArgs[0], ' ');
        parsedArgs = split(parsedPipedArgs[1], ' ');
    } else {
        parsedArgs = split(input, ' ');
    }
}

void execArgs(std::vector<char *> parsed) {
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
        wait(NULL);
        return;
    }
}

void execArgsPiped(std::vector<char *> parsed, std::vector<char *> parsedpipe) {
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
            if (execvp(parsedpipe[0], parsedpipe.data()) < 0) {
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
    std::vector<char *> parsedArgs;
    std::vector<char *> parsedPipedArgs;
    while (true) {
        printCurrentDirectory();
        std::getline(std::cin, input);
        parse(input, parsedArgs, parsedPipedArgs);
    }

    return 0;
}






