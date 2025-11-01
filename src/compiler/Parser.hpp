#include "BlockStatement.hpp"
#include "Lexer.hpp"

BlockStatement parse_tokens(std::vector<Token>::const_iterator begin,
                            std::vector<Token>::const_iterator end,
                            bool allow_functions = true);