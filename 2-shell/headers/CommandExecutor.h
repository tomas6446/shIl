#pragma once

#include "main.h"
#include "Command.h"
#include "JobHandler.h"

class Command;

class CommandExecutor {
public:
    explicit CommandExecutor(JobHandler *handler) : jobHandler(handler) {}

    void execArgs(Command &command);

    static void execArgsPiped(Command &command, Command &commandPiped);

    void execArgsRedirect(Command &command);

    void execArgsBackground(Command &command);

    static bool isBackgroundTask(Command &command);

private:
    static void execute(Command &command, pid_t pid);

    JobHandler *jobHandler;
};
