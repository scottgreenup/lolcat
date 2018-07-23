

#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

class ArgumentException : public std::exception {
public:
   ArgumentException(std::string argument) {
        std::stringstream ss;
        ss << "unknown argument: " << argument;
        this->m_message = ss.str();
   }

    const char * what () const throw () {
        return this->m_message.c_str();
    }

private:
    std::string m_message;
};

namespace flag {

enum FlagSetElementType {
    String
};

struct FlagSetElement {
    void *Value;
    void *DefaultValue;

    std::string Usage;
    FlagSetElementType Type;
    bool HasArg;
};

class FlagSet {
public:
    FlagSet() {}

    ~FlagSet() {
        for (auto [key, val] : this->m_arguments) {
            switch (val.Type) {
            case FlagSetElementType::String:
                delete static_cast<std::string*>(val.Value);
                delete static_cast<std::string*>(val.DefaultValue);
                break;
            default:
                break;
            }
        }
    }

    std::string * String(std::string name, std::string value = "", std::string usage = "") {

        FlagSetElement element;
        std::string * flagValue = new std::string();
        std::string * defaultValue = new std::string(value);

        element.Value = static_cast<void*>(flagValue);
        element.DefaultValue = static_cast<void*>(defaultValue);
        element.Type = FlagSetElementType::String;
        element.Usage = usage;
        element.HasArg = true;
        this->m_arguments[name] = element;

        return flagValue;
    }

    std::vector<std::string> Convert(std::size_t argc, char** argv) {
        std::vector<std::string> arguments(argc);
        for (std::size_t i = 0; i < argc; i++) {
            arguments[i] = std::string(argv[i]);
        }
        return arguments;
    }

    void Parse(std::vector<std::string> arguments) {
        for (std::size_t i = 0; i < arguments.size(); i++) {
            std::string arg = arguments[i];
            if (arg[0] == '-') {
                if (arg[1] == '-') {
                    std::string key = arg.substr(2);

                    if (this->m_arguments.count(key) == 0) {
                        throw ArgumentException(arg);
                    }

                    if (this->m_arguments[key].HasArg) {
                        std::string value = arguments[i+1];
                        std::string * target = static_cast<std::string*>(
                                this->m_arguments[key].Value);
                        *target = value;
                    }


                } else {
                    if (arg.length() != 2) {
                        throw ArgumentException(arg);
                    }

                    std::string key = arg.substr(1);
                    std::string value = arguments[i+1];
                }
            }
        }
    }

private:
    std::map<std::string, FlagSetElement> m_arguments;
};

}

void ParseArgs(int argc, char **argv) {

    // --help,  -h: Show the help message
    // --force, -f: Force color even when stdout is not a tty
    // We do not support --this="syntax"
    //
    auto fs = flag::FlagSet();
    auto whatever = fs.String("whatever");

    auto arguments = fs.Convert(argc, argv);
    fs.Parse(arguments);

    std::cout << "Whatever == " << *whatever << std::endl;
}

int main(int argc, char **argv) {
    try {
        ParseArgs(argc, argv);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
