#pragma once
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

class None {}; 

template <typename ReturnType, typename... Args> class FunctionWrapper {
private:
    std::function<ReturnType(Args...)> fp;
    std::tuple<Args...> args;

    template <std::size_t... Is> 
    ReturnType callFunc(std::index_sequence<Is...>) {
        if constexpr (std::is_same_v<void, ReturnType>) {
            fp(std::get<Is>(args)...);
            return; 
        } else {
            return fp(std::get<Is>(args)...);
        }
    }

public:
    FunctionWrapper(std::function<ReturnType(Args...)> fp) : fp(fp) {}

    FunctionWrapper<ReturnType, Args...>& BindArgs(Args &&... a) {
        args = {a...}; 
        return *this;
    }
    
    std::tuple<Args...> GetArgs() {
        return args;
    }

    ReturnType Invoke() {
        if constexpr (std::is_same_v<void, ReturnType>) {
            callFunc(std::index_sequence_for<Args...>());
            return; 
        }
        return callFunc(std::index_sequence_for<Args...>());
    }
    ReturnType operator()() {
        if constexpr (std::is_same_v<void, ReturnType>) {
            Invoke();
            return;
        }
        return Invoke();
    }
    operator ReturnType() { return Invoke(); }
};