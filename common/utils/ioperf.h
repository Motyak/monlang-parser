#ifndef IOPERF_H
#define IOPERF_H

#include <iostream>
#include <exception>
#include <typeinfo>
#include <cstdlib>

// For demangling type names on GCC/Clang
#ifdef __GNUG__
#include <cxxabi.h>
#endif

static class _ioperf {
  public:
    _ioperf() {
        std::ios_base::sync_with_stdio(false);
        std::set_terminate(custom_terminate);
    }

    static void custom_terminate() {
        std::cout << std::flush;

        std::exception_ptr exptr = std::current_exception();
        if (exptr) {
            try {
                std::rethrow_exception(exptr);
            }
            catch (const std::exception& e) {
                const char* type_name = typeid(e).name();
                #ifdef __GNUG__
                int status;
                char* demangled = abi::__cxa_demangle(type_name, nullptr, nullptr, &status);
                if (status == 0) type_name = demangled;
                #endif
                std::cerr << "\nterminate called after throwing an instance of '" << type_name << "'\n";
                std::cerr << "  what():  " << e.what() << std::endl;

                #ifdef __GNUG__
                if (status == 0) free(demangled);
                #endif
            }
            catch (...) {
                std::cerr << "\nterminate called after throwing an unknown instance" << std::endl;
            }
        }
        std::abort(); // Standard behavior after terminate handler
    }
} _ioperf;

#endif // IOPERF_H
