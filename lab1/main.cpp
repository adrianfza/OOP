#include <cstddef>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

enum class LogLevel { TRACE, INFO, WARN, ERROR };

static const std::string DEFAULT_FORMAT = "[%t] [%l] %n: %m";

static std::string levelName(LogLevel lvl) {
    switch (lvl) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

class Logger {
private:
    std::string name;

    static std::string currentTime() {
        std::time_t now = std::time(nullptr);
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &now);
#else
        localtime_r(&now, &tm);
#endif
        std::ostringstream out;
        out << std::put_time(&tm, "%H:%M:%S");
        return out.str();
    }

    static const std::string& randomMessage(LogLevel lvl) {
        static const std::vector<std::vector<std::string>> messages = {
            {"Entering the next step", "Checking the next character", "Still working through the input"},
            {"Operation completed", "Everything looks good", "Here is your result"},
            {"Something looks suspicious", "Proceeding with caution", "This might need attention"},
            {"Operation failed", "Invalid input detected", "Something went wrong"}};
        static std::mt19937 gen{std::random_device{}()};
        const auto& options = messages[static_cast<std::size_t>(lvl)];
        std::uniform_int_distribution<std::size_t> pick(0, options.size() - 1);
        return options[pick(gen)];
    }

    std::string render(LogLevel msgLevel, const std::string& message) const {
        std::string out;
        for (std::size_t i = 0; i < format.size(); ++i) {
            if (format[i] == '%' && i + 1 < format.size()) {
                const char p = format[i + 1];
                bool known = true;
                switch (p) {
                    case 't': out += currentTime(); break;
                    case 'm': out += message; break;
                    case 'n': out += name; break;
                    case 'l': out += levelName(msgLevel); break;
                    case 'r': out += randomMessage(msgLevel); break;
                    default:  known = false; break;
                }
                if (known) {
                    ++i;
                    continue;
                }
            }
            out += format[i];
        }
        return out;
    }

public:
    static LogLevel level;
    static std::string format;

    explicit Logger(const std::string& name) : name(name) {}

    static void setLevel(LogLevel newLevel) { level = newLevel; }
    static void setFormat(const std::string& newFormat) { format = newFormat; }

    void log(LogLevel msgLevel, const std::string& message) const {
        if (static_cast<int>(msgLevel) < static_cast<int>(level)) return;
        std::cout << render(msgLevel, message) << '\n';
    }

    void trace(const std::string& message) const { log(LogLevel::TRACE, message); }
    void info(const std::string& message) const  { log(LogLevel::INFO, message); }
    void warn(const std::string& message) const  { log(LogLevel::WARN, message); }
    void error(const std::string& message) const { log(LogLevel::ERROR, message); }
};

LogLevel Logger::level = LogLevel::INFO;
std::string Logger::format = DEFAULT_FORMAT;

class ParenthesesValidator {
private:
    Logger logger;

public:
    ParenthesesValidator() : logger("Parentheses") {}

    bool isValid(const std::string& input) const {
        std::vector<std::size_t> stack;
        bool valid = true;
        std::string reason;

        for (std::size_t i = 0; i < input.size(); ++i) {
            const char c = input[i];
            if (c == '(') {
                stack.push_back(i);
                logger.trace("Opening parenthesis found at index " + std::to_string(i));
            } else if (c == ')') {
                if (stack.empty()) {
                    logger.warn("Closing parenthesis at index " + std::to_string(i) + " but no opening parenthesis is available to match it");
                    valid = false;
                    reason = "closing parenthesis at index " + std::to_string(i) + " has no matching opening parenthesis";
                    break;
                }
                const std::size_t openIndex = stack.back();
                stack.pop_back();
                logger.info("Matching pair found: indexes " + std::to_string(openIndex) + " and " + std::to_string(i));
            }
        }

        if (valid && !stack.empty()) {
            valid = false;
            reason = std::to_string(stack.size()) + " opening parenthesis(es) never closed";
        }

        if (!valid) {
            logger.error("Invalid input \"" + input + "\": " + reason);
        }
        logger.info(std::string("Validation result: ") + (valid ? "valid" : "invalid"));
        return valid;
    }
};

struct TestCase {
    std::string input;
    bool expected;
};

int main() {
    ParenthesesValidator validator;

    Logger::setLevel(LogLevel::TRACE);
    validator.isValid("(()())");
    validator.isValid("())");

    Logger::setLevel(LogLevel::ERROR);
    validator.isValid("(()");
    validator.isValid("()");

    Logger::setLevel(LogLevel::INFO);
    const std::vector<TestCase> tests = {
        {"()", true},
        {")((()))", false},
        {"(", false},
        {"(())((()())())", true},
        {"", true},
        {"(()", false},
        {"())", false},
        {"(()())", true},
        {"a(b)c", true},
    };

    std::size_t passed = 0;
    for (const auto& t : tests) {
        const bool actual = validator.isValid(t.input);
        if (actual == t.expected) {
            ++passed;
        }
    }
    
    std::cout << passed << "/" << tests.size() << " tests passed\n";
    return passed == tests.size() ? 0 : 1;
}
