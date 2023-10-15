#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include "Command.h"
#include "JobHandler.h"

class Command;

class CommandExecutor {
public:
    explicit CommandExecutor(JobHandler *handler, pid_t &pid) : child_pid(pid), jobHandler(handler) {}

    void execArgs(const Command &command);

    void execArgsPiped(const std::vector<Command> &commands);

    void execArgsRedirect(const Command &command);

    void execArgsBackground(const Command &command);

    static bool isBackgroundTask(const Command &command);

private:
    void execute(const Command &command) const;

    pid_t &child_pid;

    JobHandler *jobHandler;
};
