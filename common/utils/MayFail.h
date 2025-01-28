/*
    THIS IS A HEADER-ONLY FILE
*/

#ifndef MAYFAIL_H
#define MAYFAIL_H

#include <iostream>
#include <optional>
#include <variant>
#include <map>
#include <any>

#define MAYFAIL_CHECK(err) \
    if (err) { \
        throw std::runtime_error("bad MayFail access (" + err.value().fmt + ")"); \
    }

template <typename T>
T* MF__move_to_heap(T obj) {
    return new T(std::move(obj));
}

struct Error {
    int code;
    std::string fmt;

    std::map<std::string, std::any> _info;

    Error() = default;
    Error(int code, std::string fmt) : code(code), fmt(fmt){}
    operator int() const {return code;}
};
#define ERR(x) Error{atoi(#x), "ERR-"#x}

template <typename T>
struct MayFail;

template <>
class MayFail<void> {
  public:
    std::optional<Error> err;

    explicit MayFail(const std::optional<Error>& err) : err(err){}
    MayFail(const Error& err) : err(err){} // convenient inside functions returning MayFail<void>

    bool has_error() const {return !!err;}
    Error error() const {return err.value();} // throws if no err

    operator bool() const {return !err;} // convenient when calling functions returning MayFail<void>
};

#define OK() MayFail<void>(std::nullopt)

template <typename T>
class MayFail : public MayFail<void> {
  public:
    T val; // accessible for simple unwrapping regardless of err

    MayFail() : MayFail<void>(std::nullopt){}
    MayFail(const T& val) : MayFail<void>(std::nullopt), val(val){}
    explicit MayFail(const T& val, const std::optional<Error>& err) : MayFail<void>(err), val(val){} // keep it explicit

    T value() const {
        MAYFAIL_CHECK(err);
        return val;
    }

    explicit operator T() const {return value();}
};

template <typename T>
struct MayFail_;

template <typename T>
class MayFail<MayFail_<T>> : public MayFail<void> {
  public:
    MayFail_<T> val;

    MayFail() : MayFail<void>(std::nullopt){}
    MayFail(const MayFail_<T>& val) : MayFail<void>(std::nullopt), val(val){}
    explicit MayFail(const T& val) : MayFail<void>(std::nullopt), val(MayFail_<T>(val)){} // keep it explicit
    explicit MayFail(const MayFail_<T>& val, const std::optional<Error>& err) : MayFail<void>(err), val(val){} // keep it explicit

    MayFail_<T> value() const {
        MAYFAIL_CHECK(err);
        return val;
    }

    explicit operator T() const {return (T)value();} // the explicit cast to T needs to be provided..
                                                     // ..by each individual MayFail_<> specializations
};

// Template type aliases to semantically indicate when a MayFail 100% contains a malformed entity.
// Constructors Malformed() and Malformed(const T&) should never be called.
// NOTE: originally, we had a template function named Malformed calling the MayFail constructor..
// ..that sets val and err, but now we want an actual type so that we can use it as a union field..
// ..in the parsing function result.
template <typename T> using Malformed = MayFail<T>;
template <typename T> using Malformed_ = MayFail_<T>;

template <typename T>
T unwrap(const MayFail_<T>& mf_) {
    return (T)mf_;
}

template <typename T>
MayFail_<T> wrap(const T& t) {
    return MayFail_<T>(t);
}

template <typename R, typename T>
MayFail<R> mayfail_cast(const MayFail<T>& inputMayfail) {
    return MayFail(R(inputMayfail.val), inputMayfail.err);
}

template <typename R, typename... Targs>
MayFail<R> mayfail_cast(const std::variant<Targs...>& inputMayfailVariant) {
    return std::visit(
        [](auto inputMayfail){
            return MayFail(R(inputMayfail.val), inputMayfail.err);
        },
        inputMayfailVariant
    );
}

template <typename R, typename T>
MayFail<R> mayfail_convert(const MayFail<T>& inputMayfail) {
    return MayFail(R(MF__move_to_heap(inputMayfail.val)), inputMayfail.err);
}

#endif // MAYFAIL_H
