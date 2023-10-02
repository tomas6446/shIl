#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <wait.h>

auto BLUE_TEXT = "\033[34m";
auto YELLOW_TEXT = "\033[33m";
const char *WHITE_TEXT = "\033[0m";

constexpr size_t MAX_ARGS = 256;
constexpr size_t MAX_ARG_LEN = 1024;

bool parse(std::string &input, std::array<char *, MAX_ARGS> &parsedArgs, size_t &parsedArgsCount,
           std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t &parsedPipedArgsCount);

void execArgsPiped(std::array<char *, MAX_ARGS> &parsed, size_t parsedCount,
                   std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t parsedPipedArgsCount);

void execArgs(std::array<char *, MAX_ARGS> &parsed, size_t count);

std::array<char *, MAX_ARGS> split(const std::string_view &str, const std::string &delimiter, size_t &count);

void clear(std::array<char *, MAX_ARGS> &parsedArgs, size_t parsedArgsCount,
           std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t parsedPipedArgsCount);

void printCurrentDirectory();

