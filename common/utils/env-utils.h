#ifndef ENV_UTILS_H
#define ENV_UTILS_H

#include <string>

inline std::string env_or_default(const std::string& var_name, const std::string& default_val) {
    auto var = std::getenv(var_name.c_str());
    if (!var) {
        return default_val;
    }
    auto var_str = std::string(var);
    return var_str != ""? var_str : default_val;
}

inline std::optional<std::string> env_get(const std::string& var_name) {
    auto var = std::getenv(var_name.c_str());
    if (!var) {
        return std::nullopt;
    }
    return std::string(var);
}

#endif // ENV_UTILS_H
