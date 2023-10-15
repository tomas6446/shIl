#include "../headers/CommandExecutor.h"

void CommandExecutor::execArgs(Command &command) {
    pid_t pid = fork();
    jobHandler->setCurrentForegroundJob(pid);
    execute(command, pid);
    waitpid(pid, nullptr, 0);
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
    int pid;
    int commandIndex = 0;
    while (commandIndex < commands.size()) {
        pid = fork();
        if (pid == 0) {
            if (commandIndex < commands.size() - 1) {
                dup2(pipefds[commandIndex * 2 + 1], STDOUT_FILENO);
            }
            if (commandIndex != 0) {
                dup2(pipefds[(commandIndex - 1) * 2], STDIN_FILENO);
            }
            for (int i = 0; i < 2 * numPipes; i++) {
                close(pipefds[i]);
            }
            execute(commands[commandIndex], pid);
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("Error forking.");
            exit(EXIT_FAILURE);
        }
        commandIndex++;
    }
    for (int i = 0; i < 2 * numPipes; i++) {
        close(pipefds[i]);
    }
    for (int i = 0; i < numPipes + 1; i++) {
        waitpid(pid, nullptr, 0);
    }
}

void CommandExecutor::execArgsRedirect(Command &command) {

}

void CommandExecutor::execArgsBackground(Command &command) {
    pid_t pid = fork();
    if (pid > 0) {
        jobHandler->addJob(Job(pid, "running", command));
    }
    execute(command, pid);
}

bool CommandExecutor::isBackgroundTask(Command &command) {
    auto isBackground = command.getArgsCount() > 0 && strcmp(command[command.getArgsCount() - 1], "&") == 0;
    if (isBackground) {
        command.remove(command.getArgsCount() - 1);
    }
    return isBackground;
}

void CommandExecutor::execute(Command command, pid_t pid) {
    if (pid == -1) {
        std::cout << "Failed forking child.." << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid == 0 && execvp(command[0], command.getArgs().data()) < 0) {
        std::cout << "shll: command not found.." << std::endl;
        exit(EXIT_FAILURE);
    }
}
