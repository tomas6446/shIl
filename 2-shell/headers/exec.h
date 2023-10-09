#pragma once

#include "main.h"

pid_t forkAndExec(std::array<char *, MAX_ARGS> &parsed, int inFd = STDIN_FILENO, int outFd = STDOUT_FILENO);

void execArgs(std::array<char *, MAX_ARGS> &parsed);

void execArgsPiped(std::array<char *, MAX_ARGS> &parsed, std::array<char *, MAX_ARGS> &parsedPipedArgs);

void execArgsRedirect(std::array<char *, MAX_ARGS> &parsed, size_t parsedCount, bool inRedirect, bool outRedirect);

void execArgsBackground(std::array<char *, MAX_ARGS> &parsed);

bool isBackgroundTask(const std::array<char *, MAX_ARGS> &parsedArgs, size_t parsedArgsCount);

