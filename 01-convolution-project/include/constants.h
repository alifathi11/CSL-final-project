#pragma once 

// ========================================================================== 
// ========================= Program Test Mode ==============================
// ==========================================================================
#define RUN_MODE_FUNCTIONAL_TEST      1
#define RUN_MODE_SPEED_TEST           2
#define RUN_MODE_NONE                -1

#define RUN_MODE_FUNCTIONAL_TEST_STR       "Functional Test"
#define RUN_MODE_SPEED_TEST_STR                 "Speed Test"

// ========================================================================== 
// ============================= Engine Mode ================================
// ==========================================================================
#define ENGINE_MODE_BASELINE           1
#define ENGINE_MODE_SSE                2   
#define ENGINE_MODE_AVX                3
#define ENGINE_MODE_NONE              -1

#define ENGINE_MODE_BASELINE_STR    "Baseline"
#define ENGINE_MODE_SSE_STR             "SSE"
#define ENGINE_MODE_AVX_STR             "AVX"

// ========================================================================== 
// ============================= Color Mode =================================
// ==========================================================================
#define COLOR_MODE_RGB                1
#define COLOR_MODE_GRAYSCALE          2
#define COLOR_MODE_NONE              -1

#define COLOR_MODE_RGB_STR               "RGB"
#define COLOR_MODE_GRAYSCALE_STR   "Grayscale"

#define CHANNELS_RGB                  3
#define CHANNELS_GRAYSCALE            1

// ========================================================================== 
// ====================== Functions Return Code =============================
// ==========================================================================
#define CODE_SUCCESS                     0
#define CODE_VALIDATION_OK               1

#define CODE_FAILURE                    -1
#define CODE_FAILURE_INVALID_ARG        -2
#define CODE_FAILURE_READ_INPUT         -3
#define CODE_FAILURE_WRITE_OUTPUT       -4
#define CODE_FAILURE_INVALID_INPUT      -5
#define CODE_FAILURE_OUT_OF_RANGE       -6  
#define CODE_FAILURE_NOT_SUPPORTED      -7
#define CODE_FAILURE_FILE_NOT_FOUND     -8
#define CODE_FAILURE_NOT_IMPLEMENTED    -9
#define CODE_FAILURE_ARG_REQUIRED      -10

// ========================================================================== 
// ============================= Log Level ==================================
// ==========================================================================
#define LOG_LEVEL_ERROR              "[ERROR]"
#define LOG_LEVEL_DEBUG              "[DEBUG]"
#define LOG_LEVEL_WARNING          "[WARNING]"
#define LOG_LEVEL_TIMING            "[TIMING]"

// ========================================================================== 
// ============================= Kernel Type ================================
// ==========================================================================
#define KERNEL_TYPE_SHARPEN          1
#define KERNEL_TYPE_BOX_BLUR         2
#define KERNEL_TYPE_GAUSSIAN_BLUR    3
#define KERNEL_TYPE_SOBEL_X          4
#define KERNEL_TYPE_SOBEL_Y          5
#define KERNEL_TYPE_NONE            -1

#define KERNEL_TYPE_SHARPEN_STR               "Sharpen"
#define KERNEL_TYPE_BOX_BLUR_STR             "Box Blur"
#define KERNEL_TYPE_GAUSSIAN_BLUR_STR   "Gaussian Blur"
#define KERNEL_TYPE_SOBEL_X_STR               "Sobel X"
#define KERNEL_TYPE_SOBEL_Y_STR               "Sobel Y"

// ========================================================================== 
// ============================= Kernel Size ================================
// ==========================================================================
#define KERNEL_SIZE_3                3
#define KERNEL_SIZE_5                5
#define KERNEL_SIZE_7                7

#define KERNEL_SIZE_3_STR            "3 × 3"
#define KERNEL_SIZE_5_STR            "5 × 5"
#define KERNEL_SIZE_7_STR            "7 × 7"

/******************************** MISC *********************************/

// ========================================================================== 
// ============================= Temp Buf Size ==============================
// ==========================================================================
#define SML_BUF_SIZE                64
#define MED_BUF_SIZE               256
#define LRG_BUF_SIZE              1024