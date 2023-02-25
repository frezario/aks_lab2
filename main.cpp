#include <iostream>
#include <map>
#include <exception>
#include <functional>
#include <cmath>
#include <numbers>
#include <fstream>
#include <sstream>


namespace functions {
    double f1(double x1, double x2) {
        double sum = 0.0;
        for (int i = -2; i != 2; i++) {
            for (int j = -2; j != 2; j++) {
                sum += 1 / (5 * (i + 2) + j + 3 + pow((x1 - 16 * j), 6) + pow((x2 - 16 * i), 6));
            }
        }
        sum += 0.002;
        return 1 / sum;
    }

    double f2(double x1, double x2, double a = 20, double b = 0.2, double c = 2 * std::numbers::pi) {

        return {};
    }


}


std::vector<std::string> file_reader(std::ifstream &filestream) {
    std::vector<std::string> result;
    std::string word;
    while (std::getline(filestream, word)) {
        if (word.empty() || (word.find('=') == word.size())) {
            continue;
        }
        result.push_back(word);
    }
    return result;
}

// gets configuration parameters from file given
std::map<std::string, double> get_config(const std::string &file_name) {
    std::ifstream config_file(file_name);
    if (!config_file.is_open()){
        throw std::system_error();
    }
    // reading the content
    auto expressions = file_reader(config_file);

    // making the map of default config properties
    std::map<std::string, double> result = {
            {"abs_err",  0.000005},
            {"rel_err",  0.0002},
            {"x_start", -50},
            {"x_end", 50},
            {"y_start", -50},
            {"y_end", 50},
            {"init_steps_x", 100},
            {"init_steps_y", 100},
            {"max_iter", 20}
    };

    for (auto& expr: expressions) {
        if (expr[0] == '#') {
            continue;
        }
        expr.erase(std::remove(expr.begin(), expr.end(), ' '), expr.end());

        // checking whether expression is correct
        if (!std::count(expr.begin(), expr.end(), '=')) {
            std::string error_message = std::string("Error while parsing a line ");
            error_message.append(expr);
            throw std::logic_error(error_message);
        }

        std::replace(expr.begin(), expr.end(), '=', ' ');
        std::stringstream ss{expr};

        // getting the parameter and value
        std::string param;
        std::string value;

        ss >> param >> value;

        // checking whether parameter name is correct
        if (!result.contains(param)) {
            std::string error_message = std::string("Unknown argument ");
            error_message.append(param);
            error_message.append(" in config file");
            throw std::logic_error(error_message);
        }

        // checking whether the value of the parameter is correct number
        double value_parsed;
        try {
            value_parsed = std::stoi(value);
        }
        catch (std::invalid_argument&) {
            std::string error_message = std::string("Invalid value ");
            error_message.append(value);
            error_message.append(" for parameter ");
            error_message.append(param);
            throw std::logic_error(error_message);
        }

        // adding new value of parameter to map
        result[param] = value_parsed;
    }

    // checking whether config properties are correct
    if (result["x_start"] > result["x_end"] || result["y_start"] > result["y_end"]) {
        throw std::logic_error("Wrong bounds for x or y passed.");
    }

    // all checks passed! closing the file and returning a map

    config_file.close();
    return result;
}


int main(int argc, char **argv) {

    if (argc != 3) {
        std::cerr << "Wrong number of arguments" << std::endl;
        exit(1);
    }

    std::map<std::string, double> config;

    // trying to get the configuration
    try {
        config = get_config(argv[1]);
    }
    catch (const std::logic_error &error) {
        std::cerr << "Unable to read the configuration properly: " << std::endl;
        // the name of the absent argument
        std::cerr << error.what();
        exit(5);
    }
    catch (...) {
        std::cerr << "Unable to open config file" << std::endl;
        exit(3);
    }

    // trying to parse the number of the function

    int num;
    try {
        num = std::stoi(argv[2]);
    }
    catch (std::invalid_argument &) {
        std::cerr << "Unable to parse the function number" << std::endl;
        exit(2);
    }
    catch (...) {
        std::cerr << "The function number is too large" << std::endl;
        exit(2);
    }

    if (num < 1 || num > 4) {
        std::cerr << "Invalid function number" << std::endl;
        exit(2);
    }

    return 0;
}
