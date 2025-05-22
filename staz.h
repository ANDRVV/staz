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
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
    #include <cstddef> // for size_t
#endif
 
/**
 * @brief Compute a root of customizable index
 * @param i The index of the root to compute
 * @param x The value to compute the root of
 * @return The i-th root of x
 */
#define staz_sqrti(i, x) (pow((x), 1.0 / (i)))

/* --- ERROR HANDLING --- */

typedef enum {
    NO_ERROR = 0,
    MEMORY_ALLOCATION_ERROR = 1,
    INVALID_PARAMETERS_ERROR,
    ZERO_DIVISION_ERROR,
    MATH_DOMAIN_ERROR,
    NAN_ERROR,
    RANGEOUT_ERROR,
    UNKNOWN_ERROR
} staz_error;

staz_error
staz_geterrno() {
    if (errno >= 0 && errno <= 6) {
        return (staz_error)errno;
    }
    return UNKNOWN_ERROR;
}

const char*
staz_strerrno() {
    const char* msg;

    switch (errno) {
    case 0:
        msg = "No error occurred";
        break;
    case MEMORY_ALLOCATION_ERROR:
        msg = "Dynamic memory allocation fails";
        break;
    case INVALID_PARAMETERS_ERROR:
        msg = "Arguments to function are invalid";
        break;
    case ZERO_DIVISION_ERROR:
        msg = "Division by zero";
        break;
    case MATH_DOMAIN_ERROR:
        msg = "Error of domain (e.g., negative root)";
        break;
    case NAN_ERROR:
        msg = "Calculation with NAN numbers";
        break;
    case RANGEOUT_ERROR:
        msg = "Number as argument to function out of range";
        break;
    default:
        msg = "An unknown error occurred";
    }

    return msg;
}

void
staz_printerrno() {
    const char* msg = staz_strerrno();
    fprintf(stderr, "STAZ: '%s'\n", msg);
}

/* --- PRIVATE METHODS --- */

/**
 * @brief Creates a deep copy of an array of double values
 * 
 * @param nums Pointer to the source array of doubles to be copied
 * @param len Number of elements in the array
 * 
 * @return double* Pointer to the newly allocated array containing copies of all elements
 *         NULL if nums is NULL, len is 0, or if memory allocation fails
 * 
 * @note Sets errno to:
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0
 *       - MEMORY_ALLOCATION_ERROR if memory allocation fails
 *       - Unchanged if operation succeeds or if input parameters are invalid
 */
static double*
copy_array(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NULL;
    }  

    errno = 0;
    
    double* copy = (double *)malloc(len * sizeof(double));
    if (!copy) {
        errno = MEMORY_ALLOCATION_ERROR;
        return NULL;
    }

    memcpy(copy, nums, len * sizeof(double));
    return copy;
}

/**
 * @brief Recursively computes the pairwise sum of elements in a double array
 * 
 * @param nums Pointer to the array of double values
 * @param start Starting index of the range to be summed
 * @param end Ending index of the range to be summed (inclusive)
 * 
 * @return double The sum of elements from nums[start] to nums[end]
 * 
 * @note This function uses pairwise recursive summation to improve
 *       numerical accuracy compared to naive linear summation.
 *       It does not perform parameter validation; the caller must ensure:
 *       - nums is not NULL
 *       - start <= end
 *       - end is within bounds of the original array
 */
static double
_staz_sum_recursive(const double* nums, size_t start, size_t end) {
    if (start == end) {
        return nums[start];
    } else if (end - start == 1) {
        return nums[start] + nums[end];
    }
        
    size_t mid = start + (end - start) / 2;

    double left = _staz_sum_recursive(nums, start, mid);
    double right = _staz_sum_recursive(nums, mid + 1, end);

    return left + right;
}

/**
 * @brief Calculates the Kahan sum of reciprocals of all elements in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The sum of reciprocals (1/x) of all elements
 *         NAN if nums is NULL, len is 0, or if any element is 0
 * 
 * @note Sets errno to:
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0
 *       - ZERO_DIVISION_ERROR if any element is 0
 *       - 0 if operation succeeds
 */
static double
_sum_recp(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    errno = 0;

    double sum = 0.0;
    double c = 0.0;

    // Compute sum of reciprocals with Kahan
    for (size_t i = 0; i < len; i++) {
        if (nums[i] == 0.0) {
            errno = ZERO_DIVISION_ERROR;
            return NAN;
        }

        double term = 1.0 / nums[i];
        double y = term - c;
        double t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }

    return sum;
}

/**
 * @brief Comparison function for qsort to sort doubles in ascending order
 * 
 * @param a Pointer to first double to compare
 * @param b Pointer to second double to compare
 * 
 * @return int Negative if a < b, zero if a == b, positive if a > b
 * 
 * @note Designed for use with qsort() standard library function
 * @warning Does not handle potential double overflow from subtraction
 */
static inline int
comp(const void *a, const void *b) {
    double da = *(const double *)a, db = *(const double *)b;
    return (da < db) ? -1 : (da > db) ? 1 : 0;
}

/* --- SHARED METHODS --- */

/**
 * @brief Calculates the Pairwise summation of all elements in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The sum of all elements in the array
 *         NAN if nums is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0
 *       - 0 if operation succeeds
 */
double
staz_sum(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    errno = 0;

    return _staz_sum_recursive(nums, 0, len - 1);
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
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0
 *       - 0 if operation succeeds
 */
double
staz_quadratic_sum(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
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
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0
 *       - 0 if operation succeeds
 *       - Stops calculation early if product becomes 0
 */
double
staz_prod(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
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
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0
 *       - 0 if operation succeeds
 */
double
staz_min_value(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
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
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0
 *       - 0 if operation succeeds
 */
double
staz_max_value(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
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
    EXTREMES,     /** Mean of extreme values (min and max) */
    TRIMEAN,      /** The Tukey's mean */
    MIDHINGE      /** Mean of Q1 and Q2 */
} staz_mean_type;

/**
 * @brief Enumeration of different range types supported by the library
 */
typedef enum {
    R_STANDARD,      /** Standard range */
    R_INTERQUARTILE, /** Interquartile range (IQR) */
    R_PERCENTILE,    /** Percentile range */
} staz_range_type;

/**
 * @brief Enum for different deviation calculation methods
 */
typedef enum {
    D_STANDARD, /** Standard deviation from mean */
    D_AVERAGE,  /** Average deviation method */
    D_RELATIVE, /** Relative deviation calculation */
    D_MAD_AVG,  /** Mean Absolute Deviation from average */
    D_MAD_MED,  /** Mean Absolute Deviation from median */
} staz_deviation_type;

/**
 * @brief Structure representing the information required to draw a boxplot.
 */
typedef struct {
    double BOX_HIGH;          /** Q3 */
    double BOX_CENTRE;        /** Median */
    double BOX_LOW;           /** Q1 */
    double BOX_UPPER_WHISKER; /** Q3 + 1.5 * iqr */
    double BOX_LOWER_WHISKER; /** Q1 - 1.5 * iqr */
    double BOX_UPPER_OUTLIER; /** max */
    double BOX_LOWER_OUTLIER; /** min */
} staz_boxplot_info;

/**
 * @brief Structure representing a linear equation in the form y = mx + q
 */
typedef struct {
    double m; /** Slope of the line (coefficient of x) */
    double q; /** Y-intercept (constant term) */
} staz_line_equation;

/**
 * @brief Calculates the median value
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The median value of the array
 *         - For odd-sized arrays: middle element
 *         - For even-sized arrays: average of the two middle elements
 *         - NAN if nums is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0
 *       - 0 if operation succeeds
 */
double
staz_median(double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    errno = 0;

    double* sorted = copy_array(nums, len);
    if (!sorted) return NAN;

    qsort(sorted, len, sizeof(double), comp);

    double med;

    if (len == 1) {
        med = sorted[0];
    } else if (len % 2 != 0) {
        med = sorted[len / 2];
    } else {
        const size_t middle = len / 2;
        med = (sorted[middle - 1] + sorted[middle]) / 2.0;
    }

    free(sorted);
    return med;
}

/**
 * @brief Calculates the quantile for a numeric array
 * 
 * @param mtype Quantile division (e.g., 1000, 20, 30, 4)
 * @param nums Pointer to array of double values
 * @param len Length of the array
 * @param posx Position of the quantile (range: 1 to mtype-1)
 * 
 * @return double The calculated quantile value
 *         NAN if input is invalid
 *
 * @note Sets errno to:
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len or posx is invalid
 *       - RANGEOUT_ERROR if mtype is invalid for len of nums
 *       - 0 if operation succeeds
 * 
 * @note Calculates quantile using linear interpolation method
 */
double
staz_quantile(int mtype, size_t posx, double* nums, size_t len) {
    if (!nums || len == 0 || posx < 1) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    /* Measure type must be between 1 and mtype-1 */
    if (posx > (size_t)mtype - 1) {
        errno = RANGEOUT_ERROR;
        return NAN;
    }

    errno = 0;

    double* sorted = copy_array(nums, len);
    if (!sorted) return NAN;

    qsort(sorted, len, sizeof(double), comp);

    const double index = posx * (len + 1) / (double)mtype;

    size_t lower = (size_t)index;

    if (lower >= len) {
        const int ret = sorted[len - 1];
        free(sorted);
        return ret;
    }
    if (lower <= 0) {
        const int ret = sorted[0];
        free(sorted);
        return ret;
    }

    double qu = sorted[lower - 1] + (index - lower) * (sorted[lower] - sorted[lower - 1]);
    free(sorted);
    return qu;
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
 *       - INVALID_PARAMETERS_ERROR if nums is NULL, len is 0, or invalid mtype
 *       - MATH_DOMAIN_ERROR for mathematical domain errors
 *       - 0 if operation succeeds
 */
double
staz_mean(staz_mean_type mtype, double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    errno = 0;

    switch (mtype) {
    case ARITHMETICAL:
        return staz_sum(nums, len) / len;

    case GEOMETRICAL: {
        const double prod_value = staz_prod(nums, len);
        if (prod_value == 0) return 0.0;
        if (prod_value < 0) {
            errno = MATH_DOMAIN_ERROR;
            return NAN;
        }

        return staz_sqrti(len, prod_value);
    }
        
    case HARMONICAL: {
        const double recp_sum = _sum_recp(nums, len);
        if (isnan(recp_sum)) return NAN;

        return len / recp_sum;
    }

    case QUADRATICAL:
        return sqrt(staz_quadratic_sum(nums, len) / len);

    case EXTREMES: {
        if (len < 2) {
            errno = INVALID_PARAMETERS_ERROR;
            return NAN;
        }

        return (staz_min_value(nums, len) + staz_max_value(nums, len)) / 2.0;
    }

    case TRIMEAN: {
        const double q1 = staz_quantile(4, 1, nums, len);
        const double q2 = staz_quantile(4, 2, nums, len);
        const double q3 = staz_quantile(4, 3, nums, len);

        return (q1 + 2 * q2 + q3) / 4.0;
    }

    case MIDHINGE: {
        const double q1 = staz_quantile(4, 1, nums, len);
        const double q3 = staz_quantile(4, 3, nums, len);

        return (q1 + q3) / 2;
    }

    default:
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }
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
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0
 *       - NAN_ERROR if computed value of mean is NAN
 *       - 0 if operation succeeds
 *       - This calculates population variance (dividing by n, not n-1)
 */
double
staz_variance(double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    errno = 0;

    const double mean_value = staz_mean(ARITHMETICAL, nums, len);

    if (isnan(mean_value)) {
        errno = NAN_ERROR;
        return NAN;
    }

    for (size_t i = 0; i < len; i++) {
        const double diff = nums[i] - mean_value;
        nums[i] = diff * diff;
    }

    return staz_mean(ARITHMETICAL, nums, len);
}

/**
 * This function supports multiple deviation metrics:
 * - Standard deviation: square root of variance
 * - Relative deviation: standard deviation divided by mean (coefficient of variation)
 * - Mean absolute deviation: average of absolute deviations from the mean
 * - Median absolute deviation: median of absolute deviations from the median
 * 
 * @param dtype Type of deviation to calculate (from staz_deviation_type enum)
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @return double The calculated deviation value
 *         NAN if:
 *         - nums is NULL or len is 0
 *         - RELATIVE deviation when mean equals zero
 *         - Memory allocation fails for MAD_MED
 *         - Invalid dtype provided
 * 
 * @note Sets errno to:
 *       - INVALID_PARAMETERS_ERROR if nums is NULL, len is 0, or invalid dtype
 *       - NAN_ERROR or ZERO_DIVISION_ERROR if computed values is NAN or zero
 *       - MEMORY_ALLOCATION_ERROR if memory allocation fails for MAD_MED
 *       - 0 if operation succeeds
 */
double
staz_deviation(staz_deviation_type dtype, double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    errno = 0;

    switch (dtype) {
    case D_STANDARD: {
        const double variance_value = staz_variance(nums, len);

        if (isnan(variance_value)) {
            errno = NAN_ERROR;
            return NAN;
        }

        return sqrt(variance_value);
    }
    
    case D_RELATIVE: {
        const double meanv = staz_mean(ARITHMETICAL, nums, len);
        if (isnan(meanv)) {
            errno = NAN_ERROR;
            return NAN;
        } else if (meanv == 0) {
            errno = ZERO_DIVISION_ERROR;
            return NAN;
        }

        return staz_deviation(D_STANDARD, nums, len) / meanv;
    }
    
    case D_MAD_AVG: {
        const double meanv = staz_mean(ARITHMETICAL, nums, len);

        for (size_t i = 0; i < len; i++) {
            nums[i] = fabs(nums[i] - meanv);
        }
        
        return staz_mean(ARITHMETICAL, nums, len);
    }

    case D_MAD_MED: {
        const double medv = staz_median(nums, len);

        for (size_t i = 0; i < len; i++) {
            nums[i] = fabs(nums[i] - medv);
        }

        return staz_mean(ARITHMETICAL, nums, len);
    }

    default:
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }
}

/**
 * @brief Calculates the mode (most frequent value) of values in an array
 * 
 * @param nums Pointer to the array of double values
 * @param len Length of the array
 * 
 * @note Sets errno to:
 *       - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0
 *       - 0 if operation succeeds
 */
double
staz_mode(const double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    errno = 0;

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
 * @brief Calculates different types of range for a numeric array
 * 
 * @param rtype Type of range calculation
 * @param nums Pointer to array of double values
 * @param len Length of the array
 * 
 * @return double The calculated range value
 *         NAN if input is invalid or calculation fails
 * 
 * @note Supports multiple range calculation methods
 *       Sets errno to:
 *       - INVALID_PARAMETERS_ERROR if either array is NULL or len is 0
 *       - NAN_ERROR if a computed value is NAN
 */
double
staz_range(staz_range_type rtype, double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    errno = 0;

    switch (rtype) {
    case R_STANDARD: {
        const double maxv = staz_max_value(nums, len);
        const double minv = staz_min_value(nums, len);

        if (isnan(maxv) || isnan(minv)) {
            errno = NAN_ERROR;
            return NAN;
        }

        return maxv - minv;
    }

    case R_INTERQUARTILE: {
        const double q1 = staz_quantile(4, 1, nums, len);
        const double q3 = staz_quantile(4, 3, nums, len);

        if (isnan(q1) || isnan(q3)) {
            errno = NAN_ERROR;
            return NAN;
        }

        return q3 - q1;
    }

    case R_PERCENTILE: {
        const double p10 = staz_quantile(100, 10, nums, len);
        const double p90 = staz_quantile(100, 90, nums, len);

        if (isnan(p10) || isnan(p90)) {
            errno = NAN_ERROR;
            return NAN;
        }

        return p90 - p10;
    }

    default:
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }
}

/**
 * @brief Calculates the covariance between two arrays of values
 * 
 * Covariance measures how much two random variables vary together.
 * A positive value indicates that variables tend to move in the same direction,
 * while a negative value indicates they move in opposite directions.
 * 
 * @param x Pointer to the first array of double values
 * @param y Pointer to the second array of double values
 * @param len Length of both arrays (must be the same)
 * 
 * @return double The covariance between the two arrays
 *         NAN if either array is NULL or len is 0
 * 
 * @note Sets errno to:
 *       - INVALID_PARAMETERS_ERROR if either array is NULL or len is 0
 *       - NAN_ERROR if mean of x or y is NAN
 *       - 0 if operation succeeds
 *       - This calculates population covariance (dividing by n, not n-1)
 */
double
staz_covariance(double* x, double* y, size_t len) {
    if (!x || !y || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    errno = 0;

    const double mean_x = staz_mean(ARITHMETICAL, x, len);
    const double mean_y = staz_mean(ARITHMETICAL, y, len);

    if (isnan(mean_x) || isnan(mean_y)) {
        errno = NAN_ERROR;
        return NAN;
    }

    double nums[len];
    
    for (size_t i = 0; i < len; i++) {
        nums[i] = (x[i] - mean_x) * (y[i] - mean_y);
    }
    
    return staz_mean(ARITHMETICAL, nums, len);
}

/**
 * @brief Calculates the Pearson correlation coefficient between two arrays
 * 
 * Correlation coefficient measures the strength and direction of a linear 
 * relationship between two variables. Values range from -1 to 1, where:
 * 1  = perfect positive correlation
 * 0  = no correlation
 * -1 = perfect negative correlation
 * 
 * @param x Pointer to the first array of double values
 * @param y Pointer to the second array of double values
 * @param len Length of both arrays (must be the same)
 * 
 * @return double The correlation coefficient between the two arrays
 *         NAN if either array is NULL or len is 0
 *         NAN if either array has zero standard deviation
 * 
 * @note Sets errno to:
 *       - INVALID_PARAMETERS_ERROR if either array is NULL or len is 0
 *       - NAN_ERROR if either array has NAN standard deviation or coviariance is NAN
 *       - ZERO_DIVISION_ERROR if either array has zero standard deviation
 *       - 0 if operation succeeds
 */
double
staz_correlation(double* x, double* y, size_t len) {
    if (!x || !y || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return NAN;
    }

    errno = 0;

    const double cov = staz_covariance(x, y, len);
    const double dev_x = staz_deviation(D_STANDARD, x, len);
    const double dev_y = staz_deviation(D_STANDARD, y, len);

    if (isnan(cov) || isnan(dev_x) || isnan(dev_y)) {
        errno = NAN_ERROR;
        return NAN;
    }

    if (dev_x == 0.0 || dev_y == 0.0) {
        errno = ZERO_DIVISION_ERROR;
        return NAN;
    }

    return cov / (dev_x * dev_y);
}

/**
 * @brief Calculates the boxplot information for a numeric array.
 *
 * @param nums Pointer to the array of double values.
 * @param len Length of the array.
 *
 * @return staz_boxplot_info Structure containing the Q3, median, Q1, upper whisker, lower whisker, upper outlier, and lower outlier.
 *
 * @note Sets errno to:
 *    - INVALID_PARAMETERS_ERROR if nums is NULL or len is 0.
 *    - 0 if operation succeeds.
 */
staz_boxplot_info
staz_boxplot(double* nums, size_t len) {
    if (!nums || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return (staz_boxplot_info) {NAN, NAN, NAN, NAN, NAN, NAN, NAN};
    }

    errno = 0;

    const double q1 = staz_quantile(4, 1, nums, len);
    const double q3 = staz_quantile(4, 3, nums, len);

    const double med = staz_median(nums, len);
    const double iqr = q3 - q1;

    const double upper_whisker = q3 + 1.5 * iqr;
    const double lower_whisker = q1 - 1.5 * iqr;

    const double upper_outlier = staz_max_value(nums, len);
    const double lower_outlier = staz_min_value(nums, len);

    return (staz_boxplot_info) {
        q3,
        med,
        q1,
        upper_whisker,
        lower_whisker,
        upper_outlier,
        lower_outlier
    };
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
 * @note Inherits errno settings from the staz_sum() function
 * 
 * @note Sets errno to:
 *    - INVALID_PARAMETERS_ERROR if x or y is NULL or len is 0
 *    - ZERO_DIVISION_ERROR if divisor of m is zero
 *    - 0 if operation succeeds
 */
staz_line_equation
staz_linear_regression(const double* x, const double* y, size_t len) {
    if (!x || !y || len == 0) {
        errno = INVALID_PARAMETERS_ERROR;
        return (staz_line_equation) {NAN, NAN};
    }

    errno = 0;

    const double sum_x = staz_sum(x, len);
    const double sum_y = staz_sum(y, len);

    double sum_xy = 0, sum_x_sq = 0;

    for (size_t i = 0; i < len; i++) {
        sum_xy += x[i] * y[i];
        sum_x_sq += x[i] * x[i];
    }

    const double d_of_m = len * sum_x_sq - sum_x * sum_x;
    if (d_of_m == 0) {
        errno = ZERO_DIVISION_ERROR;
        return (staz_line_equation) {NAN, NAN};
    }

    const double m = (len * sum_xy - sum_x * sum_y) / d_of_m;
    const double q = (sum_y - m * sum_x) / len;

    return (staz_line_equation) {
        m,
        q
    };
}

#ifdef __cplusplus
}
#endif

#endif /* STAZ_H */