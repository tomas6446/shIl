#include "../headers/main.h"
#include "../headers/exec.h"
#include "../headers/parser.h"

auto BLUE_TEXT = "\033[34m";
auto YELLOW_TEXT = "\033[33m";
auto WHITE_TEXT = "\033[0m";


void freeMemory(std::array<char *, MAX_ARGS> &parsedArgs, size_t parsedArgsCount,
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
    std::array<char, MAX_ARG_LEN> username{};
    std::array<char, MAX_ARG_LEN> hostname{};

    getlogin_r(username.data(), username.size());
    gethostname(hostname.data(), hostname.size());
    getcwd(cwd.data(), cwd.size());

    return std::string(YELLOW_TEXT) + username.data() + "@" + hostname.data()
           + std::string(BLUE_TEXT) + " ~" + cwd.data() + " $ " + std::string(WHITE_TEXT);
}

void handleExecution(const std::string &input,
                     std::array<char *, MAX_ARGS> &parsedArgs,
                     size_t parsedArgsCount,
                     std::array<char *, MAX_ARGS> &parsedPipedArgs,
                     size_t parsedPipedArgsCount) {
    if (parse(input, parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount)) {
        execArgsPiped(parsedArgs, parsedPipedArgs);
    } else if (strcmp(parsedArgs[0], "cd") == 0) {
        if (chdir(parsedArgs[1]) >= 0 || parsedArgsCount < 2) {
            return;
        }
        perror("chdir");
    } else if (isBackgroundTask(parsedArgs, parsedArgsCount)) {
        execArgsBackground(parsedArgs);
    } else {
        execArgs(parsedArgs);
    }
}


int main() {
    while (true) {
        std::string input;
        std::array<char *, MAX_ARGS> parsedArgs{};
        std::array<char *, MAX_ARGS> parsedPipedArgs{};
        size_t parsedArgsCount = 0;
        size_t parsedPipedArgsCount = 0;

        char const *line = readline(printCurrentDirectory().c_str());
        input = std::string(line);
        if (input.empty()) {
            continue;
        }
        if (input == "exit") {
            freeMemory(parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount);
            break;
        }
        add_history(input.c_str());
        handleExecution(input, parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount);
        freeMemory(parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount);
    }
    return 0;
}
