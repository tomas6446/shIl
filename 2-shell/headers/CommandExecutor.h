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

    void execArgs(const Command &);

    void execArgsPiped(const std::vector<Command> &);

    void execArgsRedirect(const Command &);

    void execArgsBackground(const Command &);

    static bool isBackgroundTask(const Command &);

private:
    void execute(const Command &) const;

    pid_t &child_pid;

    JobHandler *jobHandler;
};
