/*
    THIS IS A HEADER-ONLY FILE
*/

#ifndef LV2_COMMON_H
#define LV2_COMMON_H

#include <utils/assert-utils.h>

#include <string>
#include <optional>
#include <variant>

struct Error {
    int code;
    std::string fmt;
    inline operator int() const {
        return code;
    }
};
#define ERR(x) Error{atoi(#x), "ERR-"#x}

inline std::ostream& operator<<(std::ostream& os, Error err) {
    return os << err.fmt;
}

template <typename T>
struct MayFail;

template <>
class MayFail<void> {
  public:
    MayFail(Error err) : err(err){} // convenient inside functions returning MayFail<void>
    MayFail(std::optional<Error> err) : err(err){}

    bool has_error() const {return !!err;}
    Error error() const {return err.value();} // throws if no err

    operator bool() const {return !err;} // convenient when calling functions returning MayFail<void>

    std::optional<Error> err;
};

#define OK() return MayFail<void>(std::nullopt)

template <typename T>
class MayFail : public MayFail<void> {
  public:
    MayFail() : MayFail<void>(std::nullopt){}
    MayFail(T val) : MayFail<void>(std::nullopt), val(val){}
    explicit MayFail(T val, std::optional<Error> err) : MayFail<void>(err), val(val){} // keep it explicit

    T value() const {
        ASSERT (!err);
        return val;
    }
    explicit operator T() const {return val;}

    T val; // accessible for simple unwrapping regardless of err
};

template <typename T>
MayFail<T> Malformed(T val, Error err) {
    return MayFail(val, err);
}

template <typename T>
struct MayFail_;

template <typename T>
class MayFail<MayFail_<T>> : public MayFail<void> {
  public:
    MayFail() : MayFail<void>(std::nullopt){}
    MayFail(MayFail_<T> val) : MayFail<void>(std::nullopt), val(val){}
    explicit MayFail(T val) : MayFail<void>(std::nullopt), val(MayFail_<T>(val)){} // keep it explicit
    explicit MayFail(MayFail_<T> val, std::optional<Error> err) : MayFail<void>(err), val(val){} // keep it explicit

    MayFail_<T> value() const {
        ASSERT (!err);
        return val;
    }
    explicit operator T() const {return (T)val;} // the explicit cast to T needs to be provided..
                                                 // ..by each individual MayFail_<> specializations

    MayFail_<T> val;
};

template <typename T>
T unwrap(const MayFail_<T>&); //useless ?

template <typename T>
MayFail_<T> wrap(const T&); //useless?

template <typename R, typename T>
MayFail<R> mayfail_convert(MayFail<T> inputMayfail) {
    return MayFail(R(move_to_heap(inputMayfail.val)), inputMayfail.err);
}

#endif // LV2_COMMON_H
