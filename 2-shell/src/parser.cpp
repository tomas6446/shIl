#include "../headers/parser.h"

bool parse(const std::string &input, std::array<char *, MAX_ARGS> &parsedArgs, size_t &parsedArgsCount,
           std::array<char *, MAX_ARGS> &parsedPipedArgs, size_t &parsedPipedArgsCount) {
    auto commands = split(input, " | ", parsedArgsCount);

    if (parsedArgsCount > 1) {
        parsedArgs = split(commands[0], " ", parsedArgsCount);
        auto secondCommandArgs = split(commands[1], " ", parsedPipedArgsCount);
        for (size_t i = 0; i < parsedPipedArgsCount; ++i) {
            parsedPipedArgs[i] = secondCommandArgs[i];
        }
        return true;
    } else {
        parsedArgs = split(input, " ", parsedArgsCount);
        return false;
    }
}

std::array<char *, MAX_ARGS> split(const std::string_view &str, const std::string &delimiter, size_t &count) {
    size_t pos_start = 0;
    size_t pos_end;
    std::string token;
    std::array<char *, MAX_ARGS> result{};

    count = 0;
    while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
        token = str.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delimiter.length();
        addToken(token, result, count);
    }

    token = str.substr(pos_start);
    addToken(token, result, count);

    return result;
}

void addToken(const std::string &token, std::array<char *, MAX_ARGS> &result, size_t &count) {
    if (!token.empty()) {
        auto dest = new char[token.length() + 1];
        std::strcpy(dest, token.c_str());
        result[count++] = dest;
    }
}

