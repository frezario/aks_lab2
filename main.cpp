#include <iostream>
#include <map>
#include <exception>
#include <functional>
#include <cmath>
#include <numbers>
#include <fstream>
#include <sstream>
#include <chrono>
#include <atomic>
#include <array>
#include "integrals.h"

namespace time_functions {
    inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto res_time = std::chrono::high_resolution_clock::now();
        std::atomic_thread_fence(std::memory_order_seq_cst);
        return res_time;
    }

    template<class D>
    inline long long to_ms(const D &d) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
    }
}

namespace functions {
    double f1(double x1, double x2) {
        double sum = 0.0;
        for (int i = -2; i <= 2; i++) {
            for (int j = -2; j <= 2; j++) {
                sum += 1 / (5 * (i + 2) + j + 3 + pow((x1 - 16 * j), 6) + pow((x2 - 16 * i), 6));
            }
        }
        sum += 0.002;
        return 1 / sum;
    }

    double f2(double x1, double x2) {
        double a = 20;
        double b = 0.2;
        double c = 2 * std::numbers::pi;
        auto first_term = -a * exp(-b * sqrt(0.5 * (x1 * x1 + x2 * x2)));
        auto second_term = -exp(0.5 * (cos(c * x1) + cos(c * x2)));
        return first_term + second_term + a + exp(1);
    }

    double f3(double x1, double x2) {
        size_t m = 5;
        double a1[]{1, 2, 1, 1, 5};
        double a2[]{4, 5, 1, 2, 4};
        double c[]{2, 1, 4, 7, 2};

        double sum = 0;
        for (size_t i = 0; i != m; i++) {
            auto pattern = (x1 - a1[i]) * (x1 - a1[i]) + (x2 - a2[i]) * (x2 - a2[i]);
            auto first_factor = c[i] * exp(-1 / std::numbers::pi * pattern);
            auto second_factor = cos(std::numbers::pi * pattern);
            sum += first_factor * second_factor;
        }
        return -sum;
    }

    double f4(double x1, double x2) {
        double a = 20;
        double b = 0.2;
        double c = 2 * std::numbers::pi;
        auto first_term = -a * exp(-b * sqrt(0.5 * (x1 * x1, x2 * x2)));
        auto second_term = -exp(0.5 * (cos(c * x1) + cos(c * x2)));
        return first_term + second_term + a + exp(1);
    }

}


std::vector<std::string> file_reader(std::ifstream &filestream) {
    std::vector<std::string> result;
    std::string word;
    while (std::getline(filestream, word)) {
        if (word.empty() || word.find('=') == word.size()) {
            continue;
        }
        result.push_back(word);
    }
    return result;
}

// gets configuration parameters from file given
std::map<std::string, double> get_config(const std::string &file_name) {
    std::ifstream config_file(file_name);
    if (!config_file.is_open()) {
        throw std::system_error();
    }
    // reading the content
    auto expressions = file_reader(config_file);

    // making the map of default config properties
    std::map<std::string, double> result = {
            {"abs_err",      0.000005},
            {"rel_err",      0.0002},
            {"x_start",      -50},
            {"x_end",        50},
            {"y_start",      -50},
            {"y_end",        50},
            {"init_steps_x", 100},
            {"init_steps_y", 100},
            {"max_iter",     20}
    };

    for (auto &expr: expressions) {
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
        catch (std::invalid_argument &) {
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

    const std::function<double(double, double)> functions[] = {
            functions::f1,
            functions::f2,
            functions::f3,
            functions::f4,
    };

    auto start_time_point = time_functions::get_current_time_fenced();

    auto res = integrals::calculate_integral(functions[num - 1], config["abs_err"], config["rel_err"], config["x_start"],
                                             config["x_end"],
                                             config["y_start"], config["y_end"], (size_t) config["init_steps_x"],
                                             (size_t) config["init_steps_y"], (size_t) config["max_iter"]);

    auto end_time_point = time_functions::get_current_time_fenced();
    auto dur = end_time_point - start_time_point;


    std::cout << std::get<0>(res) << std::endl;
    std::cout << std::get<1>(res) << std::endl;
    std::cout << std::get<2>(res) << std::endl;
    std::cout << time_functions::to_ms(dur) << std::endl;

    return 0;
}
