
#pragma once

#include <string>
#include <vector>
#include "token.h"

struct LexicalError {
    std::string message;
    int line;
    int column;
};

bool scanSource(
    const std::string& source,
    std::vector<Token>& tokens,
    std::vector<LexicalError>& errors
);
