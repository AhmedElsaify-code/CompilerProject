
#include "scanner.h"

#include <cctype>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

bool scanSource(
    const string& source,
    vector<Token>& tokens,
    vector<LexicalError>& errors
) {
    tokens.clear();
    errors.clear();

    const unordered_map<string, string> keywords = {
        {"int", "INT"},
        {"float", "FLOAT"},
        {"char", "CHAR"},
        {"bool", "BOOL"},
        {"main", "MAIN"},
        {"if", "IF"},
        {"else", "ELSE"},
        {"while", "WHILE"},
        {"return", "RETURN"},
        {"void", "VOID"},
        {"true", "BOOL_LITERAL"},
        {"false", "BOOL_LITERAL"}
    };

    const vector<pair<regex, string>> patterns = {
        {regex(R"(^[ \t\r\n]+)"), "WHITESPACE"},
        {regex(R"(^[0-9]+\.[0-9]+)"), "FLOAT_NUMBER"},
        {regex(R"(^[0-9]+)"), "NUMBER"},
        {regex(R"(^'(\\.|[^'\\])')"), "CHAR_LITERAL"},
        {regex(R"(^[a-zA-Z_][a-zA-Z0-9_]*)"), "IDENTIFIER"},
        {regex(R"(^(\+\+|--|==|!=|<=|>=|&&|\|\|))"), "OPERATOR"},
        {regex(R"(^[=<>!])"), "RELATIONAL_OPERATOR"},
        {regex(R"(^[+\-*/%])"), "ARITHMETIC_OPERATOR"},
        {regex(R"(^[(){};,])"), "PUNCTUATION"}
    };

    size_t i = 0;
    int line = 1;
    int column = 1;

    while (i < source.size()) {
        string remaining = source.substr(i);
        smatch match;
        string value;
        string type;

        if (source[i] == '/' && i + 1 < source.size()) {
            if (source[i + 1] == '/') {
                while (i < source.size() && source[i] != '\n') {
                    ++i;
                    ++column;
                }
                continue;
            }

            if (source[i + 1] == '*') {
                int startLine = line;
                int startColumn = column;

                i += 2;
                column += 2;

                bool closed = false;

                while (i < source.size()) {
                    if (source[i] == '*' &&
                        i + 1 < source.size() &&
                        source[i + 1] == '/') {
                        i += 2;
                        column += 2;
                        closed = true;
                        break;
                    }

                    if (source[i] == '\n') {
                        ++line;
                        column = 1;
                        ++i;
                    } else {
                        ++i;
                        ++column;
                    }
                }

                if (!closed) {
                    errors.push_back({
                        "Unterminated block comment",
                        startLine,
                        startColumn
                    });
                }

                continue;
            }
        }

        bool found = false;

        for (const auto& pattern : patterns) {
            if (regex_search(
                    remaining,
                    match,
                    pattern.first,
                    regex_constants::match_continuous
                )) {
                value = match.str();
                type = pattern.second;
                found = true;
                break;
            }
        }

        if (!found) {
            errors.push_back({
                string("Unrecognized character: ") + source[i],
                line,
                column
            });

            ++i;
            ++column;
            continue;
        }

        if (type == "WHITESPACE") {
            for (char ch : value) {
                if (ch == '\n') {
                    ++line;
                    column = 1;
                } else {
                    ++column;
                }
            }

            i += value.size();
            continue;
        }

        if (type == "IDENTIFIER") {
            auto keyword = keywords.find(value);

            if (keyword != keywords.end()) {
                type = keyword->second;
            }
        }

        if (type == "PUNCTUATION") {
            if (value == "(") type = "LPAREN";
            else if (value == ")") type = "RPAREN";
            else if (value == "{") type = "LBRACE";
            else if (value == "}") type = "RBRACE";
            else if (value == ";") type = "SEMICOLON";
            else if (value == ",") type = "COMMA";
        }

        if (type == "OPERATOR") {
            if (value == "==" || value == "!=" ||
                value == "<=" || value == ">=") {
                type = "RELATIONAL_OPERATOR";
            } else if (value == "&&" || value == "||") {
                type = "LOGICAL_OPERATOR";
            } else if (value == "++" || value == "--") {
                type = "INCREMENT_OPERATOR";
            }
        }

        if (value == "=") {
            type = "ASSIGN";
        } else if (value == "<" || value == ">") {
            type = "RELATIONAL_OPERATOR";
        } else if (value == "!") {
            type = "LOGICAL_OPERATOR";
        }

        tokens.push_back({type, value, line, column});

        i += value.size();
        column += static_cast<int>(value.size());
    }

    return errors.empty();
}
