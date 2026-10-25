
#include "scanner.h"

#include <regex>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

struct TokenRule {
    string type;
    regex pattern;
};

static const unordered_map<string, string> keywords = {
    {"int", "INT"},
    {"void", "VOID"},
    {"char", "CHAR"},
    {"float", "FLOAT"},
    {"double", "DOUBLE"},
    {"long", "LONG"},
    {"short", "SHORT"},
    {"bool", "BOOL"},
    {"main", "MAIN"},
    {"while", "WHILE"},
    {"return", "RETURN"},
    {"if", "IF"},
    {"else", "ELSE"},
    {"for", "FOR"}
};

static const vector<TokenRule> rules = {
    {"WHITESPACE", regex(R"(\s+)")},
    {"NUMBER", regex(R"([0-9]+)")},
    {"IDENTIFIER", regex(R"([a-zA-Z_][a-zA-Z0-9_]*)")},

    {"MULTI_OPERATOR", regex(R"(\+\+|--|&&|\|\||==|!=|<=|>=)")},

    {"ASSIGN", regex(R"(=)")},
    {"RELATIONAL_OPERATOR", regex(R"([<>])")},
    {"LOGICAL_NOT", regex(R"(!)")},
    {"ARITHMETIC_OPERATOR", regex(R"([+\-*/%])")},

    {"LPAREN", regex(R"(\()")},
    {"RPAREN", regex(R"(\))")},
    {"LBRACE", regex(R"(\{)")},
    {"RBRACE", regex(R"(\})")},
    {"LBRACKET", regex(R"(\[)")},
    {"RBRACKET", regex(R"(\])")},
    {"SEMICOLON", regex(R"(;)")},
    {"COMMA", regex(R"(,)")}
};

static void advancePosition(
    const string& text,
    int& line,
    int& column
) {
    for (char ch : text) {
        if (ch == '\n') {
            ++line;
            column = 1;
        } else {
            ++column;
        }
    }
}

static string classifyToken(
    const string& ruleType,
    const string& value
) {
    if (ruleType == "IDENTIFIER") {
        auto it = keywords.find(value);

        if (it != keywords.end()) {
            return it->second;
        }
    }

    if (ruleType == "MULTI_OPERATOR") {
        if (value == "++" || value == "--") {
            return "INCREMENT_OPERATOR";
        }

        if (value == "&&" || value == "||") {
            return "LOGICAL_OPERATOR";
        }

        return "RELATIONAL_OPERATOR";
    }

    return ruleType;
}

bool scanSource(
    const string& source,
    vector<Token>& tokens,
    vector<LexicalError>& errors
) {
    tokens.clear();
    errors.clear();

    size_t pos = 0;
    int line = 1;
    int column = 1;

    while (pos < source.size()) {
        string remaining = source.substr(pos);

        if (remaining.rfind("//", 0) == 0) {
            size_t end = source.find('\n', pos);

            if (end == string::npos) {
                advancePosition(
                    source.substr(pos), line, column
                );
                pos = source.size();
            } else {
                advancePosition(
                    source.substr(pos, end - pos),
                    line, column
                );
                pos = end;
            }

            continue;
        }

        if (remaining.rfind("/*", 0) == 0) {
            size_t end = source.find("*/", pos + 2);

            if (end == string::npos) {
                errors.push_back({
                    "Unterminated block comment",
                    line,
                    column
                });

                break;
            }

            size_t length = end + 2 - pos;

            advancePosition(
                source.substr(pos, length),
                line, column
            );

            pos += length;
            continue;
        }

        bool matched = false;

        for (const auto& rule : rules) {
            smatch match;

            auto start = source.cbegin()
                       + static_cast<ptrdiff_t>(pos);

            if (!regex_search(
                    start,
                    source.cend(),
                    match,
                    rule.pattern,
                    regex_constants::match_continuous
                )) {
                continue;
            }

            string value = match.str();

            if (value.empty()) {
                continue;
            }

            if (rule.type != "WHITESPACE") {
                string type = classifyToken(
                    rule.type, value
                );

                tokens.push_back({
                    type, value, line, column
                });
            }

            advancePosition(value, line, column);
            pos += value.size();
            matched = true;
            break;
        }

        if (!matched) {
            errors.push_back({
                string("Unknown character: '")
                    + source[pos] + "'",
                line,
                column
            });

            advancePosition(
                source.substr(pos, 1), line, column
            );

            ++pos;
        }
    }

    return errors.empty();
}
