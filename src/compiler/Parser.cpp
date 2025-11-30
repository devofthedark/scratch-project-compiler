#include "Parser.hpp"

#include <format>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <stack>
#include <utility>

#include "compiler/BinaryOp.hpp"
#include "compiler/FunctionDeclaration.hpp"
#include "compiler/FunctionExpression.hpp"
#include "compiler/IfStatement.hpp"
#include "compiler/Lexer.hpp"
#include "compiler/LiteralNodes.hpp"
#include "compiler/NotOperator.hpp"
#include "compiler/ReturnStatement.hpp"
#include "compiler/VariableAssignment.hpp"
#include "compiler/VariableDeclaration.hpp"
#include "compiler/VariableExpression.hpp"
#include "compiler/WhileLoopStatement.hpp"
#include "exceptions/LanguageErrors.hpp"

namespace {
std::unique_ptr<Statement> parse_statement(std::vector<Token>::const_iterator begin,
                                           std::vector<Token>::const_iterator end);

std::unique_ptr<FunctionDeclaration> parse_function_header(std::vector<Token>::const_iterator begin,
                                                           std::vector<Token>::const_iterator end);

std::unique_ptr<IfStatement> parse_if_header(std::vector<Token>::const_iterator begin,
                                             std::vector<Token>::const_iterator end);

std::unique_ptr<WhileLoopStatement> parse_while_header(std::vector<Token>::const_iterator begin,
                                                       std::vector<Token>::const_iterator end);

std::unique_ptr<VariableDeclaration> parse_declaration(std::vector<Token>::const_iterator begin,
                                                       std::vector<Token>::const_iterator end);

std::unique_ptr<VariableAssignment> parse_assignment(std::vector<Token>::const_iterator begin,
                                                     std::vector<Token>::const_iterator end);

std::unique_ptr<FunctionExpression> parse_function_call(std::vector<Token>::const_iterator &begin,
                                                        std::vector<Token>::const_iterator end);

std::unique_ptr<Expression> parse_equation(std::vector<Token>::const_iterator begin,
                                           std::vector<Token>::const_iterator end);
/*
Calculates the bracket deficit in the range [begin, end)

Returns positive if more LPAREN than RPAREN, negative if more RPAREN than LPAREN
*/

int paren_deficit(std::vector<Token>::const_iterator begin,
                  std::vector<Token>::const_iterator end) {
    int count = 0;
    for (auto it = begin; it != end; ++it) {
        if (it->type == TokenType::LPAREN) {
            count++;
        } else if (it->type == TokenType::RPAREN) {
            count--;
        }
    }
    return count;
}

/*
Finds the matching RPAREN for the first LPAREN in the range [begin, end)
If no matching RPAREN is found, returns end
*/
std::vector<Token>::const_iterator find_matching_rparen(std::vector<Token>::const_iterator begin,
                                                        std::vector<Token>::const_iterator end) {
    int bracket_count = 0;
    for (auto it = begin; it != end; ++it) {
        if (it->type == TokenType::LPAREN) {
            bracket_count++;
        } else if (it->type == TokenType::RPAREN) {
            bracket_count--;
            if (bracket_count == 0) {
                return it;
            }
        }
    }
    return end; // No matching RPAREN found
}

int brace_deficit(std::vector<Token>::const_iterator begin,
                  std::vector<Token>::const_iterator end) {
    int count = 0;
    for (auto it = begin; it != end; ++it) {
        if (it->type == TokenType::LBRACE) {
            count++;
        } else if (it->type == TokenType::RBRACE) {
            count--;
        }
    }
    return count;
}

std::vector<Token>::const_iterator find_matching_rbrace(std::vector<Token>::const_iterator begin,
                                                        std::vector<Token>::const_iterator end) {
    int brace_count = 0;
    for (auto it = begin; it != end; ++it) {
        if (it->type == TokenType::LBRACE) {
            brace_count++;
        } else if (it->type == TokenType::RBRACE) {
            brace_count--;
            if (brace_count == 0) {
                return it;
            }
        }
    }
    return end; // No matching RBRACE found
}

std::vector<Token>::const_iterator find_matching_rbrack(std::vector<Token>::const_iterator begin,
                                                        std::vector<Token>::const_iterator end) {
    int bracket_count = 0;
    for (auto it = begin; it != end; ++it) {
        if (it->type == TokenType::LBRACKET) {
            bracket_count++;
        } else if (it->type == TokenType::RBRACKET) {
            bracket_count--;
            if (bracket_count == 0) {
                return it;
            }
        }
    }
    return end; // No matching RBRACE found
}

/*
Parses a list of tokens into a BlockStatement (AST)

Range is [begin, end)
*/

std::unique_ptr<Statement> parse_statement(std::vector<Token>::const_iterator begin,
                                           std::vector<Token>::const_iterator end) {
    // Parses a single Statement from the tokens
    // std::cerr << "parse_statement()\n";
    if (begin == end) {
        throw SyntaxError("Empty Statement", begin->line);
    }

    if (begin->type == TokenType::NUM) {
        return parse_declaration(begin, end);
    }
    // The only valid begin must be of TokenType::IDENTIFIER
    // Throw error if not
    if (begin->type == TokenType::IDENTIFIER) {
        auto itr = begin + 1;
        if (itr == end) {
            // std::cerr << begin->value << "\n";
            throw SyntaxError("Unexpected end of statement after identifier", begin->line);
        }
        if (itr->type == TokenType::ASSIGN) {
            return parse_assignment(begin, end);
        }
        if (itr->type == TokenType::LPAREN) {
            return parse_function_call(begin, end);
        }
        throw SyntaxError("Cannot parse naked equation", begin->line);
    }
    if (begin->type == TokenType::RETURN) {
        begin++; // Skip the RETURN token
        if (begin == end) {
            // Just a return statement
            // std::cerr << "Return statement\n\n";
            return std::make_unique<ReturnStatement>(nullptr);
        }
        return std::make_unique<ReturnStatement>(parse_equation(begin, end));
    }
    throw SyntaxError(std::format("Unexpected token {} at start of statement",
                                  magic_enum::enum_name(begin->type)),
                      begin->line);
    // Placeholder, should return a valid Statement
    // std::cerr << "\n\n";
}

/*
parses a function header (name, parameters, return type)

begin is the function name token
*/
std::unique_ptr<FunctionDeclaration> parse_function_header(std::vector<Token>::const_iterator begin,
                                                           std::vector<Token>::const_iterator end) {
    // Parses a function header (name, parameters, return type)
    // TODO: actual parsing, for now just skip and parse block
    // std::cerr << "parse_function_header()\n";
    auto loc = find_matching_rparen(begin, end);
    if (loc == end && paren_deficit(begin, end) != 0) {
        throw SyntaxError("Unmatched parentheses in function header", begin->line);
    }
    std::string name = begin->value;
    std::vector<Parameter> parameters;
    Type return_type = Type::VOID; // void by default
    for (auto it = begin + 2; it != loc; ++it) {
        if (it->type != TokenType::NUM && it->type != TokenType::STR) {
            throw SyntaxError("Expected type in function parameter", it->line);
        }
        auto param_type = it->type;
        it++; // Move to identifier
        if (it == loc || it->type != TokenType::IDENTIFIER) {
            throw SyntaxError("Expected identifier in function parameter", it->line);
        }
        if (param_type == TokenType::NUM) {
            parameters.push_back({it->value, Type::DOUBLE});
        } else {
            parameters.push_back({it->value, Type::STRING});
        }
        if (it + 1 != loc && (it + 1)->type == TokenType::COMMA) {
            it++; // Skip comma
        }
    }
    // Now find the opening brace
    auto itr = loc;
    while (itr != end && itr->type != TokenType::LBRACE) {
        itr++;
    }
    if (itr == end) {
        throw SyntaxError("Expected '{' in function definition", loc->line);
    }
    if (itr != loc + 1 && (loc + 1)->type == TokenType::ARROW) {
        // Has return type
        auto return_type_token = loc + 2;
        if (return_type_token == itr) {
            throw SyntaxError("Expected return type in function definition", itr->line);
        }
        if (return_type_token->type != TokenType::NUM) {
            throw SyntaxError("Expected return type in function definition",
                              return_type_token->line);
        }
        // For now, only DOUBLE return type is supported
        return_type = Type::DOUBLE;
        loc += 2;
    } else if (itr != loc + 1 && (loc + 1)->type != TokenType::LBRACKET) {
        throw SyntaxError("Unexpected token in function definition", itr->line);
    }
    if ((loc + 1)->type == TokenType::LBRACKET && find_matching_rbrack(loc + 1, end) != loc + 3) {
        throw SyntaxError("Internal stdcall error", (loc + 1)->line);
    }
    if ((loc + 1)->type == TokenType::LBRACKET && (loc + 2)->type == TokenType::STDCALL) {
        return std::make_unique<FunctionDeclaration>(name,
                                                     std::make_shared<BlockStatement>(
                                                         parse_tokens(loc + 5, end, false)),
                                                     std::move(parameters),
                                                     return_type,
                                                     true);
    }
    return std::make_unique<FunctionDeclaration>(name,
                                                 std::make_shared<BlockStatement>(
                                                     parse_tokens(itr + 1, end, false)),
                                                 std::move(parameters),
                                                 return_type);
}

std::unique_ptr<IfStatement> parse_if_header(std::vector<Token>::const_iterator begin,
                                             std::vector<Token>::const_iterator end) {
    // Parses an if statement header (condition)
    // For now just skip to the block
    // std::cerr << "parse_if_header()\n";
    auto loc = find_matching_rparen(begin, end);
    if (loc == end && paren_deficit(begin, end) != 0) {
        throw SyntaxError("Unmatched parentheses in if header", begin->line);
    }
    // Now find the opening brace
    auto itr = loc;
    while (itr != end && itr->type != TokenType::LBRACE) {
        itr++;
    }
    if (itr == end) {
        throw SyntaxError("Expected '{' in if statement", loc->line);
    }
    return std::make_unique<IfStatement>(parse_equation(begin + 2, loc),
                                         std::make_unique<BlockStatement>(
                                             parse_tokens(itr + 1, end, false)),
                                         nullptr);
}

std::unique_ptr<WhileLoopStatement> parse_while_header(std::vector<Token>::const_iterator begin,
                                                       std::vector<Token>::const_iterator end) {
    // Parses a while statement header (condition)
    // std::cerr << "parse_while_header()\n";
    auto loc = find_matching_rparen(begin, end);
    if (loc == end && paren_deficit(begin, end) != 0) {
        throw SyntaxError("Unmatched parentheses in while header", begin->line);
    }
    // Now find the opening brace
    auto itr = loc;
    while (itr != end && itr->type != TokenType::LBRACE) {
        itr++;
    }
    if (itr == end) {
        throw SyntaxError("Expected '{' in while statement", loc->line);
    }
    return std::make_unique<WhileLoopStatement>(parse_equation(begin + 2, loc),
                                                std::make_unique<BlockStatement>(
                                                    parse_tokens(itr + 1, end, false)));
}

std::unique_ptr<VariableDeclaration> parse_declaration(std::vector<Token>::const_iterator begin,
                                                       std::vector<Token>::const_iterator end) {
    // Parses a variable declaration statement
    // std::cerr << "parse_declaration()\n";
    begin++; // Skip the NUM token
    if (begin == end) {
        throw SyntaxError("Unexpected end of declaration", (begin - 1)->line);
    }
    if (begin->type != TokenType::IDENTIFIER) {
        throw SyntaxError("Expected identifier in declaration", begin->line);
    }
    if (begin + 1 == end || (begin + 1)->type != TokenType::ASSIGN) {
        throw SyntaxError("Expected '=' in declaration", (begin + 1)->line);
    }
    auto assignment = parse_assignment(begin, end);

    // std::cerr << "\n\n";
    return std::make_unique<VariableDeclaration>(*assignment, Type::DOUBLE);
}

std::unique_ptr<VariableAssignment> parse_assignment(std::vector<Token>::const_iterator begin,
                                                     std::vector<Token>::const_iterator end) {
    // Parses an assignment statement
    // std::cerr << "parse_assignment()\n";
    if (begin == end || begin->type != TokenType::IDENTIFIER) {
        throw SyntaxError("Expected identifier at start of assignment", begin->line);
    }
    auto identifier = begin->value;
    begin++; // Skip the identifier
    if (begin == end || begin->type != TokenType::ASSIGN) {
        throw SyntaxError("Expected '=' in assignment", begin->line);
    }
    begin++; // Skip the '='
    if (begin == end) {
        throw SyntaxError("Unexpected end of assignment", begin->line);
    }
    // std::cerr << "\n\n";
    return std::make_unique<VariableAssignment>(identifier, parse_equation(begin, end));
}

std::unique_ptr<FunctionExpression> parse_function_call(std::vector<Token>::const_iterator &begin,
                                                        std::vector<Token>::const_iterator end) {
    // Parses a function call statement
    // std::cerr << "parse_function_call()\n";
    if (begin == end || begin->type != TokenType::IDENTIFIER) {
        throw SyntaxError("Expected identifier at start of function call", begin->line);
    }
    // std::cerr << "Function call to " << begin->value << "\n";
    std::string func_name = begin->value;
    begin++; // Skip the identifier
    if (begin == end || begin->type != TokenType::LPAREN) {
        throw SyntaxError("Expected '(' in function call", begin->line);
    }
    int bracket_count = paren_deficit(begin, end);
    if (bracket_count != 0) {
        throw SyntaxError("Unmatched parentheses in function call", begin->line);
    }
    if ((end - 2)->type == TokenType::COMMA) {
        throw SyntaxError("Trailing comma in function call arguments", (end - 2)->line);
    }
    std::vector<std::unique_ptr<Expression>> arguments;
    for (auto it = begin + 1; it != end; ++it) {
        if (it->type == TokenType::LPAREN) {
            bracket_count++;
        } else if (it->type == TokenType::RPAREN) {
            bracket_count--;
        } else if (it->type == TokenType::COMMA && bracket_count == 0) {
            arguments.push_back(parse_equation(begin + 1, it));
            begin = it;
        }
    }
    auto tmp = parse_equation(begin + 1, end - 1);
    if (tmp) {
        arguments.push_back(std::move(tmp));
    }
    auto res = std::make_unique<FunctionExpression>(func_name, std::move(arguments));
    // res->print(0, "Function Call Expression: ");
    return res;
}

std::unique_ptr<Expression> identifier_to_expression_node(
    std::vector<Token>::const_iterator &itr, const std::vector<Token>::const_iterator &end) {
    // Converts an identifier token to an Expression
    if (itr + 1 != end && (itr + 1)->type == TokenType::LPAREN) {
        auto start = itr;
        auto func_end = find_matching_rparen(itr + 1, end);
        if (func_end == end && paren_deficit(itr + 1, end) != 0) {
            throw SyntaxError("Unmatched parentheses in function call within expression",
                              itr->line);
        }
        // Function call
        itr = func_end; // Move iterator to the end of the function call
        return parse_function_call(start, func_end + 1);
    }
    // Variable
    return std::make_unique<VariableExpression>(itr->value);
}

void process_bracket_closure(std::stack<Token> &operators,
                             std::stack<std::unique_ptr<Expression>> &output,
                             std::vector<Token>::const_iterator &itr) {
    while (!operators.empty() && operators.top().type != TokenType::LPAREN) {
        Token opr = operators.top();
        operators.pop();
        if (output.size() < 2) {
            throw SyntaxError("Insufficient values in expression", itr->line);
        }
        auto right = std::move(output.top());
        output.pop();
        auto left = std::move(output.top());
        output.pop();
        output.push(std::make_unique<BinaryOp>(std::move(left),
                                               std::move(right),
                                               token_to_binary_operator(opr.type)));
    }
    if (operators.empty() || operators.top().type != TokenType::LPAREN) {
        throw SyntaxError("Insufficient values in expression", itr->line);
    }
    operators.pop(); // Pop the '('
}

void process_operator(std::stack<Token> &operators,
                      std::stack<std::unique_ptr<Expression>> &output,
                      std::vector<Token>::const_iterator &itr) {
    while (!operators.empty()
           && get_operator_precedence(operators.top().type) >= get_operator_precedence(itr->type)) {
        Token opr = operators.top();
        operators.pop();
        if (opr.type != TokenType::NOT) {
            if (output.size() < 2) {
                throw SyntaxError("Insufficient values in expression", itr->line);
            }
            auto right = std::move(output.top());
            output.pop();
            auto left = std::move(output.top());
            output.pop();
            output.push(std::make_unique<BinaryOp>(std::move(left),
                                                   std::move(right),
                                                   token_to_binary_operator(opr.type)));
        } else {
            if (output.empty()) {
                throw SyntaxError("Insufficient values in expression", itr->line);
                auto operand = std::move(output.top());
                output.pop();
                output.push(std::make_unique<NotOperator>(std::move(operand)));
            }
        }
    }
}

std::unique_ptr<Expression> parse_equation(std::vector<Token>::const_iterator begin,
                                           std::vector<Token>::const_iterator end) {
    // We use the shunting yard algorithm to parse the equation into an AST
    std::stack<Token> operators;
    std::stack<std::unique_ptr<Expression>> output;
    for (auto it = begin; it != end; ++it) {
        if (it->type == TokenType::LITERAL_NUMBER) {
            output.push(std::make_unique<LiteralNumberNode>(std::stod(it->value)));
            continue;
        }
        if (it->type == TokenType::LITERAL_STRING) {
            output.push(std::make_unique<LiteralStringNode>(it->value));
            continue;
        }
        if (it->type == TokenType::IDENTIFIER) {
            output.push(identifier_to_expression_node(it, end));
            continue;
        }
        if (it->type == TokenType::LPAREN) {
            operators.push(*it);
            continue;
        }
        if (it->type == TokenType::RPAREN) {
            process_bracket_closure(operators, output, it);
            continue;
        }
        // Operator
        process_operator(operators, output, it);
        operators.push(*it);
    }
    while (!operators.empty()) {
        Token opr = operators.top();
        operators.pop();
        if (opr.type != TokenType::NOT) {
            if (output.size() < 2) {
                throw SyntaxError("Insufficient values in expression", (end - 1)->line);
            }
            auto right = std::move(output.top());
            output.pop();
            auto left = std::move(output.top());
            output.pop();
            output.push(std::make_unique<BinaryOp>(std::move(left),
                                                   std::move(right),
                                                   token_to_binary_operator(opr.type)));
        } else {
            if (output.empty()) {
                throw SyntaxError("Insufficient values in expression", (end - 1)->line);
            }
            auto operand = std::move(output.top());
            output.pop();
            output.push(std::make_unique<NotOperator>(std::move(operand)));
        }
    }
    if (output.empty()) {
        return nullptr;
    }
    if (output.size() != 1) {
        throw SyntaxError("Error parsing expression", begin->line);
    }
    // std::cerr << "Parsed expression:\n";
    // output.top()->print(0, "Result: ");
    return std::move(output.top());
}
} // namespace

BlockStatement parse_tokens(std::vector<Token>::const_iterator begin,
                            std::vector<Token>::const_iterator end,
                            bool allow_functions) {
    std::vector<std::unique_ptr<Statement>> statements;

    // Really disgusting code
    for (auto it = begin; it != end; ++it) {
        // Anything that has a block attached to it (function, while, if)
        if ((it->type == TokenType::FN && allow_functions) || it->type == TokenType::IF
            || it->type == TokenType::WHILE) {
            auto brace_start = find_matching_rparen(it, end);
            if (brace_start == end) {
                throw SyntaxError("Unmatched braces in block statement", it->line);
            }
            auto brace_end = find_matching_rbrace(brace_start, end);
            if (brace_deficit(brace_start, brace_end + 1) != 0) {
                throw SyntaxError("RBACE not at end of block statement", it->line);
            }
            if (it->type == TokenType::FN) {
                statements.push_back(parse_function_header(it + 1, brace_end));
            } else if (it->type == TokenType::IF) {
                statements.push_back(parse_if_header(it, brace_end));
            } else if (it->type == TokenType::WHILE) {
                statements.push_back(parse_while_header(it, brace_end));
            }
            it = brace_end;
            continue;
        }
        // Typical token (ends in ;)

        auto start_pos = it;

        while (it != end && it->type != TokenType::SEMICOLON) {
            it++;
        }
        if (it == end) {
            throw SyntaxError("Unmatched semicolon in statement", (it - 1)->line);
        }
        statements.push_back(parse_statement(start_pos, it));
    }
    // std::cout << statements.size() << "\n";
    return BlockStatement(statements);
}