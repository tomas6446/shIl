#include "../headers/main.h"
#include "../headers/Command.h"

auto BLUE_TEXT = "\033[34m";
auto YELLOW_TEXT = "\033[33m";
auto WHITE_TEXT = "\033[0m";

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

bool parse(const std::string &input, Command &command,
           Command &commandPiped) {
    auto commands = Command::split(input, " | ");

    if (commands.getArgsCount() > 1) {
        command = Command::split(commands[0], " ");
        commandPiped = Command::split(commands[1], " ");
        return true;
    } else {
        command = Command::split(input, " ");
        return false;
    }
}

bool isChangeDirCommand(Command &command) {
    return strcmp(command[0], "cd") == 0;
}

void handleChangeDir(Command &command) {
    if (command.getArgsCount() < 2 || chdir(command[1]) >= 0) {
        return;
    }
    perror("chdir");
}

void handleExecution(const std::string &input) {
    Command command(0, std::array<char *, MAX_ARGS>());
    Command commandPiped(0, std::array<char *, MAX_ARGS>());

    if (parse(input, command, commandPiped)) {
        command.execArgsPiped(commandPiped);
    } else if (isChangeDirCommand(command)) {
        handleChangeDir(command);
    } else if (command.isBackgroundTask()) {
        command.execArgsBackground();
    } else {
        command.execArgs();
    }

    command.free();
    commandPiped.free();
}

void validateInput(const std::string_view &input) {
    if (input.empty()) {
        return;
    }
    if (input == "exit") {
        exit(0);
    }
}


int main() {
    while (true) {
        std::string input;

        char const *line = readline(printCurrentDirectory().c_str());
        input = std::string(line);
        validateInput(input);
        add_history(input.c_str());
        handleExecution(input);
    }
    return 0;
}
