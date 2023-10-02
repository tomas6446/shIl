#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "fcntl.h"

auto BLUE_TEXT = "\033[34m";
auto YELLOW_TEXT = "\033[33m";
auto WHITE_TEXT = "\033[0m";

constexpr size_t MAX_ARGS = 256;
constexpr size_t MAX_ARG_LEN = 1024;

bool parse(const std::string &input, std::array<char *, MAX_ARGS> &parsedArgs, size_t &parsedArgsCount,
           std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t &parsedPipedArgsCount);

void execArgsPiped(std::array<char *, MAX_ARGS> &parsed, std::array<char *, MAX_ARGS> &parsedPipedArgs);

void execArgs(std::array<char *, MAX_ARGS> &parsed);

std::array<char *, MAX_ARGS> split(const std::string_view &str, const std::string &delimiter, size_t &count);

void free(std::array<char *, MAX_ARGS> &parsedArgs, size_t parsedArgsCount,
          std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t parsedPipedArgsCount);

std::string printCurrentDirectory();

