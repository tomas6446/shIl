#pragma once

#include "main.h"
#include "Command.h"

class Command;

class Job {
public:
    Job(pid_t pid, std::string status, const Command &cmd);

    pid_t getPid() const;

    void setPid(pid_t pid);

    const Command &getCommand() const;

    void setCommand(const Command &command);

    const std::string &getStatus() const;

    void setStatus(const std::string_view &status);

private:
    pid_t pid;
    Command command;
    std::string status;
};
