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
    std::array<char, 32> username{};
    std::array<char, 32> hostname{};

    getlogin_r(username.data(), username.size());
    gethostname(hostname.data(), hostname.size());
    getcwd(cwd.data(), cwd.size());

    return std::string(YELLOW_TEXT) + username.data() + "@" + hostname.data()
           + std::string(BLUE_TEXT) + " ~" + cwd.data() + " $ " + std::string(WHITE_TEXT);
}

void handleRedirects(std::string &input, bool &inRedirect, bool &outRedirect) {
    unsigned long pos = input.find('<');
    if (pos != std::string::npos) {
        inRedirect = true;
        input.erase(pos, 1);
    }
    pos = input.find('>');
    if (pos != std::string::npos) {
        outRedirect = true;
        input.erase(pos, 1);
    }
}

void handleExecution(const std::string &input,
                     std::array<char *, MAX_ARGS> &parsedArgs,
                     size_t parsedArgsCount,
                     std::array<char *, MAX_ARGS> &parsedPipedArgs,
                     size_t parsedPipedArgsCount,
                     bool inRedirect,
                     bool outRedirect) {

    if (parse(input, parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount)) {
        execArgsPiped(parsedArgs, parsedPipedArgs);
    } else if (inRedirect || outRedirect) {
        execArgsRedirect(parsedArgs, parsedArgsCount, inRedirect, outRedirect);
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
    std::string input;
    std::array<char *, MAX_ARGS> parsedArgs{};
    std::array<char *, MAX_ARGS> parsedPipedArgs{};
    size_t parsedArgsCount = 0;
    size_t parsedPipedArgsCount = 0;
    bool inRedirect = false;
    bool outRedirect = false;

    while (true) {
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
        handleRedirects(input, inRedirect, outRedirect);
        handleExecution(input, parsedArgs, parsedArgsCount, parsedPipedArgs, parsedPipedArgsCount, inRedirect, outRedirect);
    }
    return 0;
}
