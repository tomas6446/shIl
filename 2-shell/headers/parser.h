#pragma once

#include "main.h"

bool parse(const std::string &input, std::array<char *, MAX_ARGS> &parsedArgs, size_t &parsedArgsCount,
           std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t &parsedPipedArgsCount);

std::array<char *, MAX_ARGS> split(const std::string_view &str, const std::string &delimiter, size_t &count);

void addToken(const std::string &token, std::array<char *, MAX_ARGS> &result, size_t &count);
