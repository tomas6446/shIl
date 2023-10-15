#include <utility>

#include "../headers/Job.h"

Job::Job(pid_t pid, std::string status, const Command &cmd) : pid(pid), command(cmd), status(std::move(status)) {}

pid_t Job::getPid() const {
    return pid;
}

void Job::setPid(pid_t pid) {
    Job::pid = pid;
}

const Command Job::getCommand() const {
    return command;
}

void Job::setCommand(const Command &command) {
    Job::command = command;
}

const std::string &Job::getStatus() const {
    return status;
}

void Job::setStatus(const std::string_view &status) {
    Job::status = status;
}

