#ifndef ASSERT_UTILS_H
#define ASSERT_UTILS_H

#include <iostream>

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                    << " line " << __LINE__ << std::endl; \
            std::terminate(); \
        } \
    } while (false)

#define SHOULD_NOT_HAPPEN() \
    do { \
        std::cerr << "`Should not happen` raised in " << __FILE__ \
                << " line " << __LINE__ << std::endl; \
        std::terminate(); \
    } while (false)

#define TODO() \
    do { \
        std::cerr << "Missing implementation in " << __FILE__ \
                << " line " << __LINE__ << std::endl; \
        std::terminate(); \
    } while (false)

#endif // ASSERT_UTILS_H
