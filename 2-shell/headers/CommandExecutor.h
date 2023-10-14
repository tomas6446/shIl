#pragma once

#include "main.h"
#include "Command.h"

class Command;

class CommandExecutor {
public:
    static void execArgs(Command &command);

    static void execArgsPiped(Command &command, Command &commandPiped);

    static void execArgsRedirect(Command &command);

    static void execArgsBackground(Command &command);

    static bool isBackgroundTask(Command &command);

private:
    static void execute(Command &command, pid_t pid);
};
