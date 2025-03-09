# Staz

Staz is a portable, lightweight C library for blazing fast statistical computations on numeric arrays.

## Overview

Staz provides efficient implementations of common statistical operations for numeric data analysis. The library is designed to be lightweight, portable, and easy to use.

## Features

- **Core Statistical Functions**:
  - Mean calculations (arithmetic, geometric, harmonic, quadratic, etc.)
  - Variance and deviation analysis
  - Median and mode identification
  - Range calculations (standard, interquartile, percentile)
  - Quantile computation

- **Data Relationships**:
  - Covariance calculation
  - Pearson correlation coefficient
  - Linear regression

- **Data Visualization Support**:
  - Boxplot metrics generation

- **Robust Error Handling**:
  - Comprehensive error detection and reporting
  - Detailed error messages

## Quick Start

### Including Staz in Your Project

Staz is a header-only library. Simply include `staz.h` in your project:

```c
#include "staz.h"
```

### Basic Usage Examples

#### Calculating Basic Statistics

```c
#include <stdio.h>
#include "staz.h"

int main() {
    double data[] = {12.5, 10.0, 13.2, 15.7, 9.8};
    size_t len = 5;

    // Calculate arithmetic mean
    double mean = staz_mean(ARITHMETICAL, data, len);
    printf("Arithmetic Mean: %.2f\n", mean);

    // Calculate median
    double median = staz_median(data, len);
    printf("Median: %.2f\n", median);

    // Calculate standard deviation
    double std_dev = staz_deviation(D_STANDARD, data, len);
    printf("Standard Deviation: %.2f\n", std_dev);

    return 0;
}
```

#### Analyzing Data Relationships

```c
#include <stdio.h>
#include "staz.h"

int main() {
    double x[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double y[] = {2.1, 3.9, 6.2, 8.1, 9.8};
    size_t len = 5;

    // Calculate correlation
    double correlation = staz_correlation(x, y, len);
    printf("Correlation: %.4f\n", correlation);

    // Perform linear regression
    line_equation line = linear_regression(x, y, len);
    printf("Linear Regression: y = %.2fx + %.2f\n", line.m, line.q);

    return 0;
}
```

#### Error Handling

```c
#include <stdio.h>
#include "staz.h"

int main() {
    double data[] = {1.0, 2.0, 3.0, 0.0, 5.0};
    size_t len = 5;

    // Try to calculate harmonic mean (which will fail due to division by zero)
    double mean = staz_mean(HARMONICAL, data, len);
    
    if (isnan(mean)) {
        printf("Error: %s\n", staz_strerrno());
        staz_printerrno();
    }

    return 0;
}
```

## API Reference

### Mean Calculations

- `staz_mean(mean_type mtype, double* nums, size_t len)`: Calculate various types of means
  - Supported types: ARITHMETICAL, GEOMETRICAL, HARMONICAL, QUADRATICAL, EXTREMES, TRIMEAN, MIDHINGE

### Statistical Dispersion

- `staz_variance(double* nums, size_t len)`: Calculate population variance
- `staz_deviation(deviation_type dtype, double* nums, size_t len)`: Calculate deviation metrics
  - Supported types: D_STANDARD, D_RELATIVE, D_MAD_AVG, D_MAD_MED
- `staz_range(range_type rtype, double* nums, size_t len)`: Calculate range values
  - Supported types: R_STANDARD, R_INTERQUARTILE, R_PERCENTILE

### Position Statistics

- `staz_median(double* nums, size_t len)`: Calculate median value
- `staz_mode(const double* nums, size_t len)`: Find the most frequent value
- `staz_quantile(int mtype, size_t posx, double* nums, size_t len)`: Calculate specific quantiles

### Relationships

- `staz_covariance(double* x, double* y, size_t len)`: Calculate covariance between two arrays
- `staz_correlation(double* x, double* y, size_t len)`: Calculate Pearson correlation coefficient
- `linear_regression(const double* x, const double* y, size_t len)`: Perform linear regression

### Data Visualization Support

- `staz_boxplot(double* nums, size_t len)`: Generate boxplot metrics

### Error Handling

- `staz_geterrno()`: Get the current error code
- `staz_strerrno()`: Get a string description of the current error
- `staz_printerrno()`: Print the current error to stderr

## Error Codes

Staz uses the following error codes:

- `NO_ERROR`: No error occurred
- `MEMORY_ALLOCATION_ERROR`: Dynamic memory allocation failed
- `INVALID_PARAMETERS_ERROR`: Invalid arguments provided to a function
- `ZERO_DIVISION_ERROR`: Division by zero attempted
- `MATH_DOMAIN_ERROR`: Mathematical domain error (e.g., negative root)
- `NAN_ERROR`: Calculation with NaN values
- `RANGEOUT_ERROR`: Number out of valid range for the operation
- `UNKNOWN_ERROR`: Unspecified error