#include <iostream>
#include "../headers/CommandExecutor.h"

void CommandExecutor::execArgs(Command &command) {
    child_pid = fork();
    jobHandler->setCurrentForegroundJob(child_pid);
    execute(command);
    waitpid(child_pid, nullptr, 0);
    child_pid = -1;
}

void CommandExecutor::execArgsPiped(const std::vector<Command> &commands) {
    if (commands.size() < 2) {
        return;
    }
    int numPipes = commands.size() - 1;
    int pipefds[2 * numPipes];
    for (int i = 0; i < numPipes; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("Couldn't pipe.");
            exit(EXIT_FAILURE);
        }
    }
    std::vector<pid_t> child_pids;
    for (int i = 0; i < commands.size(); ++i) {
        child_pid = fork();
        if (child_pid == 0) {
            if (i < commands.size() - 1) {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }
            if (i != 0) {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }
            for (int i = 0; i < 2 * numPipes; i++) {
                close(pipefds[i]);
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
        close(pipefds[i]);
    }
    for (const auto &pid: child_pids) {
        waitpid(pid, nullptr, 0);
    }
}


void CommandExecutor::execArgsRedirect(Command &command) {

}

void CommandExecutor::execArgsBackground(Command &command) {
    child_pid = fork();
    if (child_pid > 0) {
        jobHandler->addJob(Job(child_pid, "running", command));
    }
    execute(command);
    child_pid = -1;
}

bool CommandExecutor::isBackgroundTask(Command &command) {
    auto isBackground = command.getArgsCount() > 0 && strcmp(command[command.getArgsCount() - 1], "&") == 0;
    if (isBackground) {
        command.remove(command.getArgsCount() - 1);
    }
    return isBackground;
}

void CommandExecutor::execute(Command command) const {
    if (child_pid == -1) {
        std::cout << "Failed forking child.." << std::endl;
        exit(EXIT_FAILURE);
    } else if (child_pid == 0 && execvp(command[0], command.getArgs().data()) < 0) {
        std::cout << "shll: command not found.." << std::endl;
        exit(EXIT_FAILURE);
    }
}
