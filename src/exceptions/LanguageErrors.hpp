#include <exception>
#include <string>

class LanguageError : public std::exception {
    using std::exception::exception;
};

class SyntaxError : public LanguageError {
private:
    int line;
    std::string message;
    mutable std::string cached;

public:
    SyntaxError(std::string msg, int line);
    const char *what() const noexcept override;
};