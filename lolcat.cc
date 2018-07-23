

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

class UnknownArgumentException : public std::exception {
public:
   UnknownArgumentException(std::string argument) {
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
    Boolean,
    Double,
    String,
};

struct FlagSetElement {
    std::shared_ptr<void> Value;

    // TODO use the default value
    std::shared_ptr<void> DefaultValue;

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
        /*
        for (auto [key, val] : this->m_arguments) {
            switch (val.Type) {
            case FlagSetElementType::String:
                delete static_cast<std::string*>(val.Value);
                delete static_cast<std::string*>(val.DefaultValue);
                break;
            case FlagSetElementType::Double:
                delete static_cast<double*>(val.Value);
                delete static_cast<double*>(val.DefaultValue);
                break;
            case FlagSetElementType::Boolean:
                delete static_cast<bool*>(val.Value);
                delete static_cast<bool*>(val.DefaultValue);
                break;
            default:
                break;
            }
        }
        */
    }

    std::shared_ptr<std::string> String(std::string name, std::string value = "", std::string usage = "") {
        auto fv = this->Argument<std::string>(name, value, usage, FlagSetElementType::String);
        this->m_arguments[name].HasArg = true;
        return fv;
    }

    std::shared_ptr<double> Double(std::string name, double value = 0.0, std::string usage = "") {
        auto fv = this->Argument<double>(name, value, usage, FlagSetElementType::Double);
        this->m_arguments[name].HasArg = true;
        return fv;
    }

    std::shared_ptr<bool> Boolean(std::string name, bool value = false, std::string usage = "") {
        auto fv = this->Argument<bool>(name, value, usage, FlagSetElementType::Boolean);
        this->m_arguments[name].HasArg = false;
        return fv;
    }

    template<typename T>
    std::shared_ptr<T> Argument(std::string name, T value, std::string usage, FlagSetElementType type) {
        FlagSetElement element;
        std::shared_ptr<T> flagValue = std::make_shared<T>();
        std::shared_ptr<T> defaultValue = std::make_shared<T>(value);
        element.Value = std::static_pointer_cast<void>(flagValue);
        element.DefaultValue = std::static_pointer_cast<void>(defaultValue);
        element.Type = type;
        element.Usage = usage;
        this->m_arguments[name] = element;
        return flagValue;
    }

    std::vector<std::string> Convert(std::size_t argc, char** argv) {
        std::vector<std::string> arguments(argc - 1);
        for (std::size_t i = 0; i < argc - 1; i++) {
            arguments[i] = std::string(argv[i+1]);
        }
        return arguments;
    }

    void Parse(std::vector<std::string> arguments) {
        for (std::size_t i = 0; i < arguments.size(); i++) {
            std::string arg = arguments[i];
            if (arg[0] == '-' && arg[1] == '-') {
                std::string name = arg.substr(2);

                if (this->m_arguments.count(name) == 0) {
                    std::cout << "name has no pre: " << name << std::endl;
                    throw UnknownArgumentException(arg);
                }

                std::cout << "name: " << name << std::endl;

                if (this->m_arguments[name].HasArg) {
                    this->SetArg(name, arguments[i+1]);
                    i++;
                } else {
                    this->RunArg(name);
                }

            } else {
                throw UnknownArgumentException(arg);
            }
        }
    }

private:
    void RunArg(std::string name) {
        switch (this->m_arguments[name].Type) {
        case FlagSetElementType::Boolean:
            {
                std::shared_ptr<bool> target = std::static_pointer_cast<bool>(this->m_arguments[name].Value);
                *target = true;
            }
            break;
        default:
            // TODO change this to a better exception
            throw std::logic_error("Unknown FlagSetElementType");
        }
    }

    void SetArg(std::string name, std::string value) {
        switch (this->m_arguments[name].Type) {
        case FlagSetElementType::String:
            {
                std::shared_ptr<std::string> target = std::static_pointer_cast<std::string>(
                        this->m_arguments[name].Value);
                *target = value;
            }
            break;
        case FlagSetElementType::Double:
            {
                std::shared_ptr<double> target = std::static_pointer_cast<double>(
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

struct Options {
    std::shared_ptr<double> horizontal;
    std::shared_ptr<double> vertical;
    std::shared_ptr<bool> force;
};

Options ParseArgs(int argc, char **argv) {
    auto fs = flag::FlagSet();

    Options options = {
        .horizontal = fs.Double("horizontal"),
        .vertical = fs.Double("vertical"),
        .force = fs.Boolean("force"),
    };

    auto arguments = fs.Convert(argc, argv);
    fs.Parse(arguments);

    return options;
}

int main(int argc, char **argv) {
    try {
        auto options = ParseArgs(argc, argv);
        std::cout << *options.force << std::endl;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
