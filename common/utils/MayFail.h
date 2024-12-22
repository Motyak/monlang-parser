/*
    THIS IS A HEADER-ONLY FILE
*/

#ifndef MAYFAIL_H
#define MAYFAIL_H

#include <iostream>

#define MF__ASSERT(condition) \
    if (!(condition)) { \
        std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                << " line " << __LINE__ << std::endl; \
        std::terminate(); \
    }

template <typename T>
T* MF__move_to_heap(T obj) {
    return new T(std::move(obj));
}

struct Error {
    int code;
    std::string fmt;
    operator int() const;
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
        MF__ASSERT (!err);
        return val;
    }

    explicit operator T() const {return value();}
};

template <typename T>
MayFail<T> Malformed(const T& val, const Error& err) {
    return MayFail(val, err);
}

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
        MF__ASSERT (!err);
        return val;
    }

    explicit operator T() const {return (T)value();} // the explicit cast to T needs to be provided..
                                                     // ..by each individual MayFail_<> specializations
};

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
