/*
 * staz.h
 * 
 * Copyright (c) 2025 Andrea Vaccaro
 * 
 * A lightweight C library for statistical computations on numeric arrays.
 * 
 * This is the main and only file of this library.
 * Public functions:
 *  - sum
 *  - prod
 *  - median
 *  - arithmetic_mean
 *  - geometric_mean
 *  - harmonic_mean
 *  - extreme_mean
 *  - variance
 *  - std_deviation
 *  - min_value
 *  - max_value
 *  - range
*/

#ifndef STAZ_H
#define STAZ_H

#include <stdio.h>
#include <math.h>

// Compute a root of customizable index
#define sqrti(i, x) (pow(x, 1.0 / i))

#ifdef __cplusplus
    #define size_t ssize_t
#endif

// Sum reciprocals function for integer values.
static int
_sum_recp(int* nums, int len) {
    int vsum = 0;

    // sum all values of array
    for (int i = 0; i < len; i++) {
        if (nums[i] != 0) vsum += 1 / nums[i];
    }

    return vsum;
}

// Sum reciprocals function for double values.
static double
_sum_recp(double* nums, int len) {
    double vsum = 0;

    // sum all values of array
    for (int i = 0; i < len; i++) {
        if (nums[i] != 0) vsum += 1.0 / nums[i];
    }

    return vsum;
}

// Sum function for integer values.
int
sum(int* nums, int len) {
    int vsum = 0;

    // sum all values of array
    for (int i = 0; i < len; i++) {
        vsum += nums[i];
    }

    return vsum;
}

// Sum function for double values.
double
sum(double* nums, int len) {
    double vsum = 0;

    // sum all values of array
    for (int i = 0; i < len; i++) {
        vsum += nums[i];
    }

    return vsum;
}

// Prod function for integer values.
int
prod(int* nums, int len) {
    int product = 1;

    // multiplies all values of array
    for (int i = 0; i < len; i++) {
        product *= nums[i];
    }

    return product;
}

// Prod function for double values.
double
prod(double* nums, int len) {
    double product = 1;

    // multiplies all values of array
    for (int i = 0; i < len; i++) {
        product *= nums[i];
    }

    return product;
}

// Median value for integer values.
// Parameter "int* nums" must be sorted.
// You can sort with sort()
double
median(int* nums, int len) {
    // if have a single middle value return it
    if (len % 2 != 0) {
        return nums[len / 2];
    }
    
    // Save middle as center of array
    size_t middle = len / 2;

    // else return middle of 2 values
    return (nums[middle - 1] + nums[middle]) / 2.0;
}

// Median value for double values.
// Parameter "double* nums" must be sorted.
// You can sort with sort()
double
median(double* nums, int len) {
    // if have a single middle value return it
    if (len % 2 != 0) {
        return nums[len / 2];
    }
    
    // Save middle as center of array
    size_t middle = len / 2;

    // else return middle of 2 values
    return (nums[middle - 1] + nums[middle]) / 2.0;
}

// Arithmetic mean for integer values.
inline double
arithmetic_mean(int* nums, int len) {
    return sum(nums, len) / len;
}

// Arithmetic mean for double values.
inline double
arithmetic_mean(double* nums, int len) {
    return sum(nums, len) / len;
}

// Geometric mean for integer values.
inline double
geometric_mean(int* nums, int len) {
    return sqrti(len, prod(nums, len));
}

// Geometric mean for double values.
inline double
geometric_mean(double* nums, int len) {
    return sqrti(len, prod(nums, len));
}

// Harmonic mean for integer values.
inline double
harmonic_mean(int* nums, int len) {
    return len / _sum_recp(nums, len);
}

// Harmonic mean for double values.
inline double
harmonic_mean(double* nums, int len) {
    return len / _sum_recp(nums, len);
}

// Mean of extreme integer values.
// Parameter "int* nums" must be sorted.
// You can sort with sort()
inline double
extreme_mean(int* nums, int len) {
    return (nums[0] + nums[len - 1]) / 2.0;
}

// Mean of extreme double values.
// Parameter "double* nums" must be sorted.
// You can sort with sort()
inline double
extreme_mean(double* nums, int len) {
    return (nums[0] + nums[len - 1]) / 2.0;
}

// Variance for integer values.
double
variance(int* nums, int len) {
    double mean = arithmetic_mean(nums, len);

    // numerator of variance
    int vsum = 0;
    for (int i = 0; i < len; i++) {
        double diff = nums[i] - mean;
        vsum += diff * diff;
    }

    return vsum / len;
}


// Variance for double values.
double
variance(double* nums, int len) {
    double mean = arithmetic_mean(nums, len);

    // numerator of variance
    int vsum = 0;
    for (int i = 0; i < len; i++) {
        double diff = nums[i] - mean;
        vsum += diff * diff;
    }

    return vsum / len;
}

// Standard deviation for integer values.
inline double
std_deviation(int* nums, int len) {
    return sqrt(variance(nums, len));
}

// Standard deviation for double values.
inline double
std_deviation(double* nums, int len) {
    return sqrt(variance(nums, len));
}

// Standard deviation for integer values.
int
min_value(int* nums, int len) {
    int min = nums[0];
    for (int i = 1; i < len; i++) {
        if (nums[i] < min) min = nums[i];
    }
    return min;
}

// Min value for double values.
double
min_value(double* nums, int len) {
    double min = nums[0];
    for (int i = 1; i < len; i++) {
        if (nums[i] < min) min = nums[i];
    }
    return min;
}

// Max value for integer values.
int
max_value(int* nums, int len) {
    int max = nums[0];
    for (int i = 1; i < len; i++) {
        if (nums[i] > max) max = nums[i];
    }
    return max;
}

// Max value for double values.
double
max_value(double* nums, int len) {
    double max = nums[0];
    for (int i = 1; i < len; i++) {
        if (nums[i] > max) max = nums[i];
    }
    return max;
}

// Range for integer values.
// Diff of max and min value.
inline int
range(int* nums, int len) {
    return max_value(nums, len) - min_value(nums, len);
}

// Range for double values.
// Diff of max and min value.
inline double
range(double* nums, int len) {
    return max_value(nums, len) - min_value(nums, len);
}

struct lineEquation {
    double m;
    double q;
};

// Linear regression function for double values.
lineEquation
linear_regression(double* x, double* y, int len) {
    double sum_x = sum(x, len), sum_y = sum(y, len), sum_xy = 0, sum_x_sq = 0;

    for (int i = 0; i < len; i++) {
        sum_xy += x[i] * y[i];
        sum_x_sq += x[i] * x[i];
    }

    double m = (len * sum_xy - sum_x * sum_y) / (len * sum_x_sq - sum_x * sum_x);
    double q = (sum_y - m * sum_x) / len;

    return lineEquation{m, q};
}


#endif /* STAZ_H */