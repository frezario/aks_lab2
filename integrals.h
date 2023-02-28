//
// Created by Naz on 2/25/2023.
//

#ifndef INTEGRATE_SERIAL_INTEGRALS_H
#define INTEGRATE_SERIAL_INTEGRALS_H

#include <cstddef>
#include <cmath>

namespace integrals {

    /* Calculates the integral of the function with given relative and
     * absolute errors. The function must be of two parameters.
     * */

    template <class F>
    double calculate_riemann_sum(F function, double x_start, double x_end, double y_start,
                                 double y_end, size_t steps_x, size_t steps_y,
                                 double shift_x = 0, double shift_y = 0) {
        auto delta_x = (x_end - x_start) / (double)steps_x;
        auto delta_y = (y_end - y_start) / (double)steps_y;
        double result = 0;
        for (size_t step_x = 0; step_x != steps_x; step_x++) {
            for (size_t step_y = 0; step_y != steps_y; step_y++) {
                auto value_of_func = function(x_start + (double)step_x * delta_x + shift_x,
                                              y_start + (double)step_y * delta_y + shift_y);
                result += delta_x * delta_y * value_of_func;
            }
        }
        return result;
    }

    template <class F>
    std::tuple<double, double, double> calculate_integral(F function, double abs_err, double rel_err,
                              double x_start, double x_end, double y_start,
                              double y_end, size_t init_steps_x,
                              size_t init_steps_y, size_t max_iter) {

        // first step: dividing an interval and computing
        // the initial value of the riemann sum over it
        double first_riemann_sum = 0;
        double second_riemann_sum = 0;
        auto steps_x = init_steps_x;
        auto steps_y = init_steps_y;
        size_t counter = 0;
        do {
            auto delta_x = (x_end - x_start) / (double)steps_x;
            auto delta_y = (y_end - y_start) / (double)steps_y;

            if (counter == 0) {
                first_riemann_sum = calculate_riemann_sum(function, x_start, x_end, y_start, y_end, steps_x, steps_y);
            }
            else {
                first_riemann_sum = second_riemann_sum;
            }

            auto first = calculate_riemann_sum(function, x_start, x_end, y_start, y_end,
                                               steps_x, steps_y, delta_x / 2, 0);
            auto second = calculate_riemann_sum(function, x_start, x_end, y_start, y_end,
                                                steps_x, steps_y, 0, delta_y / 2);
            auto third = calculate_riemann_sum(function, x_start, x_end, y_start, y_end,
                                               steps_x, steps_y, delta_x / 2, delta_y / 2);
            second_riemann_sum = 0.25 * (first_riemann_sum + first + second + third);
            steps_x *= 2;
            steps_y *= 2;
            counter++;
        }
        while ((fabs(second_riemann_sum - first_riemann_sum) < abs_err &&
                fabs((second_riemann_sum - first_riemann_sum) / second_riemann_sum) < rel_err) &&
                counter != max_iter);

        return std::make_tuple(second_riemann_sum, fabs(second_riemann_sum - first_riemann_sum),
                               fabs((second_riemann_sum - first_riemann_sum) / second_riemann_sum));
    }
}


#endif //INTEGRATE_SERIAL_INTEGRALS_H
