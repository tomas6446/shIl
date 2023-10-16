#include <utility>

#include "../headers/Job.h"

Job::Job(pid_t pid, std::string status, const Command &cmd) : pid(pid), command(cmd), status(std::move(status)) {}

pid_t Job::getPid() const {
    return pid;
}

void Job::setPid(pid_t p) {
    Job::pid = p;
}

Command Job::getCommand() const {
    return command;
}

void Job::setCommand(const Command &com) {
    Job::command = com;
}

const std::string &Job::getStatus() const {
    return status;
}

void Job::setStatus(const std::string_view &stat) {
    Job::status = stat;
}

