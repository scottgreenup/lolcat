

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
    String,
    Double,
};

struct FlagSetElement {
    void *Value;

    // TODO use the default value
    void *DefaultValue;

    std::string Usage;
    FlagSetElementType Type;
    bool HasArg;
};

// Each 'type' has a way to cleanup, a way to create one, and a case block in
// SetArg
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
        auto fv = this->Argument<std::string>(name, value, usage, FlagSetElementType::String);
        this->m_arguments[name].HasArg = true;
        return fv;
    }

    double * Double(std::string name, double value = 0.0, std::string usage = "") {
        auto fv = this->Argument<double>(name, value, usage, FlagSetElementType::Double);
        this->m_arguments[name].HasArg = true;
        return fv;
    }

    template<typename T>
    T* Argument(std::string name, T value, std::string usage, FlagSetElementType type) {
        FlagSetElement element;

        T * flagValue = new T;
        T * defaultValue = new T(value);

        element.Value = static_cast<void*>(flagValue);
        element.DefaultValue = static_cast<void*>(defaultValue);
        element.Type = type;
        element.Usage = usage;
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
            if (arg[0] == '-' && arg[1] == '-') {
                std::string name = arg.substr(2);

                if (this->m_arguments.count(name) == 0) {
                    throw ArgumentException(arg);
                }

                if (this->m_arguments[name].HasArg) {
                    this->SetArg(name, arguments[i+1]);
                }

            } else if (arg[0] == '-') {
                if (arg.length() != 2) {
                    throw ArgumentException(arg);
                }

                std::string name = arg.substr(1);
                std::string value = arguments[i+1];

            } else {
                // TODO we have a non-flag argument
            }
        }
    }

private:
    void SetArg(std::string name, std::string value) {
        switch (this->m_arguments[name].Type) {
        case FlagSetElementType::String:
            {
                std::string * target = static_cast<std::string*>(
                        this->m_arguments[name].Value);
                *target = value;
            }
            break;
        case FlagSetElementType::Double:
            {
                double * target = static_cast<double*>(
                        this->m_arguments[name].Value);
                *target = std::stod(value);
            }
            break;
        default:
            // TODO change this to a better exception
            throw std::logic_error("Unknown FlagSetElementType");
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
    auto something = fs.Double("something");


    auto arguments = fs.Convert(argc, argv);
    fs.Parse(arguments);

    std::cout << "whatever == " << *whatever << std::endl;
    std::cout << "something * 2.5 == " << *something * 2.5 << std::endl;
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
