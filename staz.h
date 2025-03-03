/*
 * staz.h
 * 
 * Copyright (c) 2025 Andrea Vaccaro
 * 
 * Staz is a portable, lightweight C library for blazing fast statistical
 * computations on numeric arrays.
 * 
 * This is the main and only file of this library.
 * Documentation are provided on /docs folder.
 */

#ifndef STAZ_H
#define STAZ_H

#include <stdio.h>
#include <math.h>
#include <errno.h>

// Compute a root of customizable index
#define sqrti(i, x) (pow(x, 1.0 / i))

#ifdef __cplusplus
    #define size_t ssize_t
#endif

/* --- PRIVATE METHODS --- */

static double
_sum_recp(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    double vsum = 0;

    // sum all values of array
    for (size_t i = 0; i < len; i++) {
        if (nums[i] == 0) {
            errno = ERANGE;
            return NAN; 
        }

        vsum += 1.0 / nums[i];
    }

    return vsum;
}

/* --- SHARED METHODS --- */

// Sum.
double
sum(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    double vsum = 0;

    // sum all values of array
    for (size_t i = 0; i < len; i++) {
        vsum += nums[i];
    }

    return vsum;
}

// Sum of quadratic values.
double
quadratic_sum(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    double vsum = 0;

    // sum all values of array
    for (size_t i = 0; i < len; i++) {
        vsum += nums[i] * nums[i];
    }

    return vsum;
}

// Prod.
double
prod(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    double product = 1.0;

    // multiplies all values of array
    for (size_t i = 0; i < len; i++) {
        product *= nums[i];
        if (product == 0.0) break;
    }

    return product;
}

// Min value.
double
min_value(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    double min = nums[0];

    for (size_t i = 1; i < len; i++) {
        if (nums[i] < min) min = nums[i];
    }

    return min;
}

// Max value.
double
max_value(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    double max = nums[0];

    for (size_t i = 1; i < len; i++) {
        if (nums[i] > max) max = nums[i];
    }

    return max;
}

/* --- PUBLIC METHODS --- */

enum MeanType {
    ARITHMETICAL, // arithmetic mean
    GEOMETRICAL, // geometrical mean
    HARMONICAL, // harmonic mean
    QUADRATICAL, // quadratic mean
    V_OF_EXTREME // mean of 2 extremes
};

struct lineEquation {
    double m; // value of m for x
    double q; // angular coefficient
};

// Median value.
// Parameter "nums" must be sorted.
// You can sort with sort()
double
median(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    if (len == 1) return nums[0];

    // if have a single middle value return it
    if (len % 2 != 0) {
        return nums[len / 2];
    }
    
    // Save middle as center of array
    const size_t middle = len / 2;

    // else return middle of 2 values
    return (nums[middle - 1] + nums[middle]) / 2.0;
}

double
mean(const MeanType mtype, const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    switch (mtype) {
    case ARITHMETICAL:
        return sum(nums, len) / len;

    case GEOMETRICAL: {
        const double prod_value = prod(nums, len);
        if (prod_value == 0) return 0.0;
        if (prod_value < 0) {
            errno = ERANGE;
            return NAN;
        }

        return sqrti(len, prod_value);
    }
        
    case HARMONICAL: {
        const double recp_sum = _sum_recp(nums, len);
        if (isnan(recp_sum)) return NAN;

        return len / recp_sum;
    }

    case QUADRATICAL:
        return sqrt(quadratic_sum(nums, len) / len);

    case V_OF_EXTREME: {
        if (len < 2) {
            errno = ERANGE;
            return NAN;
        }
        return (nums[0] + nums[len - 1]) / 2.0;
    }

    default:
        errno = EINVAL;
        return NAN;
    };
}

// Variance.
double
variance(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    const double mean_value = mean(MeanType::ARITHMETICAL, nums, len);
    if (isnan(mean_value)) return NAN;

    // numerator of variance
    int vsum = 0;
    for (size_t i = 0; i < len; i++) {
        const double diff = nums[i] - mean_value;
        vsum += diff * diff;
    }

    return vsum / len;
}

// Standard deviation.
inline double
std_deviation(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    const double variance_value = variance(nums, len);
    if (isnan(variance_value)) return NAN;

    return sqrt(variance_value);
}

// Range.
// Diff of max and min value.
inline double
range(const double* nums, size_t len) {
    const double maxv = max_value(nums, len);
    const double minv = min_value(nums, len);

    if (isnan(maxv) || isnan(minv)) return NAN;

    return maxv - minv;
}

// Linear regression function for double values.
lineEquation
linear_regression(const double* x, const double* y, size_t len) {
    const double sum_x = sum(x, len), sum_y = sum(y, len);
    double sum_xy = 0, sum_x_sq = 0;

    for (size_t i = 0; i < len; i++) {
        sum_xy += x[i] * y[i];
        sum_x_sq += x[i] * x[i];
    }

    const double m = (len * sum_xy - sum_x * sum_y) / (len * sum_x_sq - sum_x * sum_x);
    const double q = (sum_y - m * sum_x) / len;

    return lineEquation{m, q};
}

#endif /* STAZ_H */