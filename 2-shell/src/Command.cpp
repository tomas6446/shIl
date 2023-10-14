#include <array>
#include "../headers/Command.h"

Command::Command(size_t count, std::array<char *, MAX_ARGS> const &args) : arguments(args), argsCount(count) {}

const std::array<char *, MAX_ARGS> &Command::getArgs() const {
    return arguments;
}

void Command::setArgs(const std::array<char *, MAX_ARGS> &args) {
    Command::arguments = args;
}

size_t Command::getArgsCount() const {
    return argsCount;
}

void Command::setArgsCount(size_t count) {
    Command::argsCount = count;
}

void Command::addArgument(const std::string &token) {
    if (!token.empty()) {
        auto dest = new char[token.length() + 1];
        std::strcpy(dest, token.c_str());
        arguments[argsCount++] = dest;
    }
}

char *Command::operator[](size_t index) {
    if (index >= argsCount) {
        std::cout << "Index out of bound, exiting";
        exit(0);
    }
    return arguments[index];
}

Command Command::split(const std::string_view &str, const std::string &delimiter) {
    size_t pos_start = 0;
    size_t pos_end;
    std::string token;
    Command result(0, std::array<char *, MAX_ARGS>());

    while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
        token = str.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delimiter.length();
        result.addArgument(token);
    }

    token = str.substr(pos_start);
    result.addArgument(token);

    return result;
}

void Command::free() {
    for (const auto &item: arguments) {
        delete item;
    }
}

void Command::execArgs() {
    CommandExecutor::execArgs(*this);
}

void Command::execArgsPiped(Command &commandPiped) {
    CommandExecutor::execArgsPiped(*this, commandPiped);
}

void Command::execArgsRedirect() {
    CommandExecutor::execArgsRedirect(*this);
}

void Command::execArgsBackground() {
    CommandExecutor::execArgsBackground(*this);
}

bool Command::isBackgroundTask() {
    return CommandExecutor::isBackgroundTask(*this);
}


