#include "slip/util/math.hpp"

namespace slip
{

namespace detail
{

double powi_recursive_impl(int32_t base, int32_t power) {
    switch (power) {
        case 0: return 1;
        case 1: return base;
        default:
            double half = powi_recursive_impl(base, power / 2);
            return half * half * (power % 2 ? base : 1);
    }
}

} // namespace detail

double powi(int32_t base, int32_t power) {
    bool is_negative_power = false;
    if (power < 0) {
        is_negative_power = true;
        power = -power;
    }

    double res = detail::powi_recursive_impl(base, power);

    return is_negative_power ? 1.0 / res : res;
}

} // namespace slip
