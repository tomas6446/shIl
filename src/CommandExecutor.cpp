#include <iostream>
#include "../headers/CommandExecutor.h"

void CommandExecutor::execute(const Command &command) const {
    if (child_pid == -1) {
        std::cout << "Failed forking child.." << std::endl;
        exit(EXIT_FAILURE);
    } else if (child_pid == 0 && execvp(command.getArguments()[0], command.getArguments().data()) < 0) {
        std::cout << " shIl: command not found.." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void CommandExecutor::execArgs(const Command &command) {
    child_pid = fork();
    jobHandler->setCurrentForegroundJob(child_pid);
    execute(command);
    waitpid(child_pid, nullptr, 0);
}

void CommandExecutor::execArgsPiped(const std::vector<Command> &commands) {
    size_t numPipes = commands.size() - 1;
    int pipefds[2 * commands.size() - 1];
    for (int i = 0; i < numPipes; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("Couldn't pipe.");
            exit(EXIT_FAILURE);
        }
    }
    std::vector<pid_t> child_pids;
    for (auto i = 0; i < commands.size(); ++i) {
        child_pid = fork();
        // If this is the child process
        if (child_pid == 0) {
            if (i < commands.size() - 1) {
                // Redirect stdout to the write end of the pipe
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }
            if (i != 0) {
                // Redirect stdin from the read end of the previous pipe
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }
            for (int j = 0; j < 2 * numPipes; j++) {
                // Close all pipe file descriptors in the child process
                close(pipefds[j]);
            }
            execute(commands[i]);
            exit(EXIT_FAILURE);
        } else if (child_pid < 0) {
            perror("Error forking.");
            exit(EXIT_FAILURE);
        }
        child_pids.push_back(child_pid);
    }
    for (int i = 0; i < 2 * numPipes; i++) {
        // Close all pipe file descriptors in the parent process
        close(pipefds[i]);
    }
    for (const auto &pid: child_pids) {
        waitpid(pid, nullptr, 0);
    }
}

void CommandExecutor::execArgsBackground(const Command &command) {
    child_pid = fork();
    if (child_pid > 0) {
        jobHandler->addJob(Job(child_pid, "running", command));
    }
    execute(command);
}

bool CommandExecutor::isBackgroundTask(const Command &command) {
    auto isBackground = !command.getArguments().empty() && strcmp(command.getArguments()[command.getArguments().size() - 1], "&") == 0;
    if (isBackground) {
        command.getArguments().pop_back();
    }
    return isBackground;
}

void CommandExecutor::execArgsRedirect(const Command &command) {

}
