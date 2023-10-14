#pragma once

#include "main.h"

void execute(std::array<char *, MAX_ARGS> &parsed, pid_t pid);

void execArgs(std::array<char *, MAX_ARGS> &parsedArgs);

void execArgsPiped(std::array<char *, MAX_ARGS> &parsedArgs, std::array<char *, MAX_ARGS> &parsedPipedArgs);

void execArgsRedirect(std::array<char *, MAX_ARGS> &parsed, const std::vector<std::pair<char, int>> &redirections);

void execArgsBackground(std::array<char *, MAX_ARGS> &parsed);

bool isBackgroundTask(const std::array<char *, MAX_ARGS> &parsedArgs, size_t parsedArgsCount);

