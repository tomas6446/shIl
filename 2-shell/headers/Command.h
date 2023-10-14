#pragma once

#include "main.h"
#include "CommandExecutor.h"

class CommandExecutor;

class Command {
public:
    explicit Command(size_t count, std::array<char *, MAX_ARGS> const &args);

    const std::array<char *, MAX_ARGS> &getArgs() const;

    void setArgs(const std::array<char *, MAX_ARGS> &args);

    size_t getArgsCount() const;

    void setArgsCount(size_t count);

    void addArgument(const std::string &token);

    static Command split(const std::string_view &str, const std::string &delimiter);

    void free();

    char *operator[](size_t index);

    void execArgs();

    void execArgsPiped(Command &commandPiped);

    void execArgsRedirect();

    void execArgsBackground();

    bool isBackgroundTask();

private:
    std::array<char *, MAX_ARGS> arguments{};
    size_t argsCount = 0;
    CommandExecutor *commandExecutor{};
};
