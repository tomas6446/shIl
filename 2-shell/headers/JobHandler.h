#pragma once

#include <list>
#include "main.h"
#include "Job.h"

class Job;

class JobHandler {
public:
    void handleJobs() const;

    void handleFg(const std::string_view &input);

    void addJob(const Job &job);

    void handleBg(const std::string_view &input);

    __sighandler_t handleSigtstp();

    pid_t getCurrentForegroundJob() const;

    void setCurrentForegroundJob(pid_t currentForegroundJob);

private:
    pid_t currentForegroundJob = -1;
    std::list<Job> jobs;
};
