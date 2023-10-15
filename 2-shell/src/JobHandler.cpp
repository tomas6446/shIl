#include <iostream>
#include <sstream>
#include <signal.h>
#include "../headers/JobHandler.h"

void JobHandler::handleJobs() const {
    int jobNumber = 1;
    if (jobs.empty()) {
        std::cout << "No jobs at the moment." << std::endl;
        return;
    }
    for (auto const &job: jobs) {
        std::cout << "[" << jobNumber << "] " << job.getStatus() << " " << job.getCommand().toString() << std::endl;
        jobNumber++;
    }
}

void JobHandler::handleFg(const std::string_view &input) {
    std::istringstream iss(input.data());
    std::string cmd;
    int jobNumber = -1;
    iss >> cmd;
    iss >> jobNumber;

    if (jobNumber == -1) {
        for (auto const &job: jobs) {
            kill(job.getPid(), SIGTERM);
        }
        jobs.clear();
    } else if (jobNumber > 0 && jobNumber <= jobs.size()) {
        auto it = std::next(jobs.begin(), jobNumber - 1);
        Job const &job = *it;
        kill(job.getPid(), SIGCONT);
        jobs.erase(it);
    }
}

void JobHandler::handleBg(const std::string_view &input) {
    std::istringstream iss(input.data());
    std::string cmd;
    int jobNumber = -1;
    iss >> cmd;
    iss >> jobNumber;

    if (jobNumber == -1) {
        if (!jobs.empty()) {
            Job &lastJob = jobs.back();
            if (lastJob.getStatus() == "stopped") {
                kill(lastJob.getPid(), SIGCONT);
                lastJob.setStatus("running");
            }
        }
    } else if (jobNumber > 0 && jobNumber <= jobs.size()) {
        auto it = std::next(jobs.begin(), jobNumber - 1);
        Job &job = *it;
        if (job.getStatus() == "stopped") {
            kill(job.getPid(), SIGCONT);
            job.setStatus("running");
        }
    }
}

void JobHandler::addJob(const Job &job) {
    jobs.emplace_back(job);
}

void JobHandler::handleSigtstp() {
    if (currentForegroundJob != -1) {
        kill(currentForegroundJob, SIGTSTP);
        for (auto &item: jobs) {
            if (item.getPid() == currentForegroundJob) {
                item.setStatus("stopped");
                std::cout << "Job [" << jobs.size() << "] " << currentForegroundJob << " suspended." << std::endl;
            }
        }
    }
}

pid_t JobHandler::getCurrentForegroundJob() const {
    return currentForegroundJob;
}

void JobHandler::setCurrentForegroundJob(pid_t currentForegroundJob) {
    JobHandler::currentForegroundJob = currentForegroundJob;
}
