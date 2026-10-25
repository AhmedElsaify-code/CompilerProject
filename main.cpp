#include <iostream>
#include <fstream>
#include <iterator>
#include <iomanip>
#include <string>
#include <vector>
#include "scanner.h"
#include "token.h"

using namespace std;

int main(int argc, char *argv[])
{
    string filename = (argc > 1) ? argv[1] : "input.c";

    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filename << '\n';
        return 1;
    }

    string source(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>());

    file.close();

    vector<Token> tokens;
    vector<LexicalError> errors;

    scanSource(source, tokens, errors);

    if (!errors.empty())
    {
        cout << "\nLexical Errors:\n";

        for (const auto &error : errors)
        {
            cout << "Line " << error.line
                 << ", Column " << error.column
                 << ": " << error.message << '\n';
        }
    }

    if (!tokens.empty())
    {
        cout << "\nTokens:\n";

        cout << left
             << setw(20) << "Type"
             << setw(20) << "Value"
             << setw(10) << "Line"
             << "Column\n";

        cout << string(60, '-') << '\n';

        for (const auto &token : tokens)
        {
            cout << left
                 << setw(20) << token.type
                 << setw(20) << token.value
                 << setw(10) << token.line
                 << token.column << '\n';
        }
    }

    if (errors.empty())
    {
        cout << "\nScanning completed successfully.\n";
        return 0;
    }

    return 1;
}