#pragma once

#include "Command.h"

class Command;

class Job {
public:
    Job(pid_t, std::string, const Command &);

    pid_t getPid() const;

    void setPid(pid_t);

    Command getCommand() const;

    void setCommand(const Command &);

    const std::string &getStatus() const;

    void setStatus(const std::string_view &);

private:
    pid_t pid;
    Command command;
    std::string status;
};
