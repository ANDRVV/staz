/*
 * staz.h
 * 
 * Copyright (c) 2025 Andrea Vaccaro
 * 
 * Staz is a portable, lightweight C library for blazing fast statistical
 * computations on numeric arrays.
 * 
 * This is the main and only file of this library.
 * Documentation is provided in the /docs folder.
 */

#ifndef STAZ_H
#define STAZ_H
 
#include <stdio.h>
#include <math.h>
#include <errno.h>
 
/**
 * @brief Compute a root of customizable index
 * @param i The index of the root to compute
 * @param x The value to compute the root of
 * @return The i-th root of x
 */
#define sqrti(i, x) (pow(x, 1.0 / i))

#ifdef __cplusplus
    #define size_t std::size_t
#endif

/* --- PRIVATE METHODS --- */

/**
 * @brief Calculates the sum of reciprocals of all elements in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The sum of reciprocals (1/x) of all elements
 *         NAN if nums is NULL, len is 0, or if any element is 0
 * 
 * @note Sets errno to:
 *       - EINVAL if nums is NULL or len is 0
 *       - ERANGE if any element is 0
 *       - 0 if operation succeeds
 */
static double
_sum_recp(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    double vsum = 0;

    // Sum the reciprocals of all array values
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

/**
 * @brief Calculates the sum of all elements in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The sum of all elements in the array
 *         NAN if nums is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - EINVAL if nums is NULL or len is 0
 *       - 0 if operation succeeds
 */
double
sum(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    double vsum = 0;

    // Sum all values of array
    for (size_t i = 0; i < len; i++) {
        vsum += nums[i];
    }

    return vsum;
}

/**
 * @brief Calculates the sum of squares of all elements in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The sum of squares of all elements
 *         NAN if nums is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - EINVAL if nums is NULL or len is 0
 *       - 0 if operation succeeds
 */
double
quadratic_sum(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    double vsum = 0;

    // Sum squares of all values in array
    for (size_t i = 0; i < len; i++) {
        vsum += nums[i] * nums[i];
    }

    return vsum;
}

/**
 * @brief Calculates the product of all elements in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The product of all elements
 *         NAN if nums is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - EINVAL if nums is NULL or len is 0
 *       - 0 if operation succeeds
 *       - Stops calculation early if product becomes 0
 */
double
prod(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    double product = 1.0;

    // Multiply all values of array
    for (size_t i = 0; i < len; i++) {
        product *= nums[i];
        if (product == 0.0) break;  // Early termination optimization
    }

    return product;
}

/**
 * @brief Finds the minimum value in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The minimum value in the array
 *         NAN if nums is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - EINVAL if nums is NULL or len is 0
 *       - 0 if operation succeeds
 */
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

/**
 * @brief Finds the maximum value in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The maximum value in the array
 *         NAN if nums is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - EINVAL if nums is NULL or len is 0
 *       - 0 if operation succeeds
 */
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

/**
 * @brief Enumeration of different mean types supported by the library
 */
typedef enum {
    ARITHMETICAL, /** Arithmetic mean (average) */
    GEOMETRICAL,  /** Geometric mean (nth root of product) */
    HARMONICAL,   /** Harmonic mean (reciprocal of average of reciprocals) */
    QUADRATICAL,  /** Quadratic mean (root mean square) */
    EXTREMES      /** Mean of extreme values (min and max) */
} mean_type;

/**
 * @brief Enumeration of different quantiles supported by the library
 */
typedef enum {
    TERTILE    = 3,   /** Data divided into 3 groups */
    QUARTILE   = 4,   /** Data divided into 4 groups */
    QUINTILE   = 5,   /** Data divided into 5 groups */
    SEXTILE    = 6,   /** Data divided into 6 groups */
    SEPTILE    = 7,   /** Data divided into 7 groups */
    OCTILE     = 8,   /** Data divided into 8 groups */
    DECILE     = 10,  /** Data divided into 10 groups */
    DUO_DECILE = 12,  /** Data divided into 12 groups */
    HEXADECILE = 16,  /** Data divided into 16 groups */
    VENTILE    = 20,  /** Data divided into 20 groups */
    PERCENTILE = 100, /** Data divided into 100 groups */
    PERMILLE   = 1000 /** Data divided into 1000 groups */
} measure_type;


/**
 * @brief Structure representing a linear equation in the form y = mx + q
 */
typedef struct {
    double m; /** Slope of the line (coefficient of x) */
    double q; /** Y-intercept (constant term) */
} line_equation;

/**
 * @brief Calculates the median value of a sorted array
 * 
 * @param nums Pointer to the SORTED array of double values
 * @param len Length of the array
 * 
 * @return double The median value of the array
 *         - For odd-sized arrays: middle element
 *         - For even-sized arrays: average of the two middle elements
 *         - NAN if nums is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - EINVAL if nums is NULL or len is 0
 *       - 0 if operation succeeds
 *       - The array MUST be sorted before calling this function
 */
double
median(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    if (len == 1) return nums[0];

    // If we have a single middle value, return it
    if (len % 2 != 0) {
        return nums[len / 2];
    }
    
    // Save middle as center of array
    const size_t middle = len / 2;

    // Else return average of the two middle values
    return (nums[middle - 1] + nums[middle]) / 2.0;
}

/**
 * @brief Calculates different types of means for an array of values
 * 
 * @param mtype The type of mean to calculate (from MeanType enum)
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The calculated mean value
 *         NAN if:
 *         - nums is NULL or len is 0
 *         - GEOMETRICAL mean with negative product
 *         - HARMONICAL mean with any element = 0
 *         - V_OF_EXTREME with len < 2
 *         - Invalid mtype provided
 * 
 * @note Sets errno to:
 *       - EINVAL if nums is NULL, len is 0, or invalid mtype
 *       - ERANGE for mathematical domain errors
 *       - 0 if operation succeeds
 */
double
mean(mean_type mtype, const double* nums, size_t len) {
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

    case EXTREMES: {
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

/**
 * @brief Calculates the variance of values in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The variance of the values
 *         NAN if nums is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - EINVAL if nums is NULL or len is 0
 *       - 0 if operation succeeds
 *       - This calculates population variance (dividing by n, not n-1)
 */
double
variance(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    const double mean_value = mean(mean_type::ARITHMETICAL, nums, len);
    if (isnan(mean_value)) return NAN;

    // Calculate sum of squared differences from the mean
    int vsum = 0;
    for (size_t i = 0; i < len; i++) {
        const double diff = nums[i] - mean_value;
        vsum += diff * diff;
    }

    return vsum / len;
}

/**
 * @brief Calculates the standard deviation of values in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The standard deviation (square root of variance)
 *         NAN if nums is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - EINVAL if nums is NULL or len is 0
 *       - 0 if operation succeeds
 *       - This calculates population standard deviation
 */
inline double
std_deviation(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = EINVAL;
        return NAN;
    }

    errno = 0;

    const double variance_value = variance(nums, len);

    return (isnan(variance_value)) ? NAN : sqrt(variance_value);
}

/**
 * @brief Calculates the range (max - min) of values in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The range of values (difference between max and min)
 *         NAN if nums is NULL or len is 0
 * 
 * @note Inherits errno settings from min_value() and max_value()
 */
inline double
range(const double* nums, size_t len) {
    const double maxv = max_value(nums, len);
    const double minv = min_value(nums, len);

    return (isnan(maxv) || isnan(minv)) ? NAN : maxv - minv;
}

/**
 * @brief Calculates the mode (most frequent value) of values in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 */
double
mode(const double* nums, size_t len) {
    double maxv = 0, maxc = 0;

    for (size_t i = 0; i < len; ++i) {
        size_t c = 0;

        for (size_t j = 0; j < len; ++j) {
            if (nums[j] == nums[i]) ++c;
        }

        if (c > maxc) {
            maxc = c;
            maxv = nums[i];
        }
    }

    return maxv;
}

/**
 * @brief Calculates the quantile for a sorted numeric array
 * 
 * @param mtype Type of quantile division (e.g., QUARTILE, DECILE)
 * @param nums Pointer to sorted array of double values
 * @param len Length of the array
 * @param posx Position of the quantile (1 to mtype-1)
 * 
 * @return double The calculated quantile value
 *         NAN if input is invalid
 *
 * @note Sets errno to:
 *       - EINVAL if nums is NULL or len or posx is invalid
 *       - 0 if operation succeeds
 * 
 * @note Input array MUST be sorted in ascending order
 * @note Calculates quantile using linear interpolation method
 */
double
quantile(measure_type mtype, const double* nums, size_t len, size_t posx) {
    if (!nums || len == 0 || posx < 1) {
        errno = EINVAL;
        return NAN;
    }

    /* Measure type must be between 1 and mtype-1 */
    if (posx > (size_t)mtype - 1) {
        errno = ERANGE;
        return NAN;
    }

    errno = 0;

    const double index = posx * (len + 1) / (double)mtype;

    int lower = (int)index;

    if (lower >= len) return nums[len - 1];
    if (lower <= 0) return nums[0];

    return nums[lower - 1] + (index - lower) * (nums[lower] - nums[lower - 1]);
}

/**
 * @brief Performs linear regression on two arrays of points
 * 
 * @param x Pointer to the array of x coordinates
 * @param y Pointer to the array of y coordinates
 * @param len Length of both arrays (must be the same)
 * 
 * @return lineEquation Structure containing m (slope) and q (y-intercept)
 *         representing the best-fit line y = mx + q
 * 
 * @note Both arrays must have the same length
 * @note Inherits errno settings from the sum() function
 */
line_equation
linear_regression(const double* x, const double* y, size_t len) {
    const double sum_x = sum(x, len);
    const double sum_y = sum(y, len);

    double sum_xy = 0, sum_x_sq = 0;

    for (size_t i = 0; i < len; i++) {
        sum_xy += x[i] * y[i];
        sum_x_sq += x[i] * x[i];
    }

    const double m = (len * sum_xy - sum_x * sum_y) / (len * sum_x_sq - sum_x * sum_x);
    const double q = (sum_y - m * sum_x) / len;

    return line_equation{m, q};
}

#endif /* STAZ_H */