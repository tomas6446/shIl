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

const char *Command::operator[](size_t index) const {
    if (index >= argsCount) {
        throw std::out_of_range("Index out of range");
    }
    return arguments[index];
}

char *&Command::operator[](size_t index) {
    if (index >= argsCount) {
        throw std::out_of_range("Index out of range");
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

std::vector<Command> Command::splitCommands(const std::string_view &str, const std::string &delimiter) {
    size_t pos_start = 0;
    size_t pos_end;
    std::string token;
    std::vector<Command> commands;

    while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
        token = str.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delimiter.length();

        Command cmd = split(token, " ");
        commands.push_back(cmd);
    }

    token = str.substr(pos_start);
    Command cmd = split(token, " ");
    commands.push_back(cmd);

    return commands;
}

void Command::free() {
    for (const auto &item: arguments) {
        delete item;
    }
}

std::string Command::toString() const {
    std::string result;
    for (size_t i = 0; i < argsCount; ++i) {
        result += arguments[i];
        if (i < argsCount - 1) {
            result += " ";
        }
    }
    return result;
}

void Command::remove(size_t i) {
    arguments[i] = nullptr;
    argsCount--;
}


