#pragma once

#include <list>
#include "Job.h"

class Job;

class JobHandler {
public:
    void handleJobs() const;

    void handleFg(const std::string_view &);

    void addJob(const Job &);

    void handleBg(const std::string_view &);

    void handleSigtstp();

    pid_t getCurrentForegroundJob() const;

    void setCurrentForegroundJob(pid_t);

private:
    pid_t currentForegroundJob = -1;
    std::list<Job> jobs;
};
