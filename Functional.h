#pragma once
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
namespace Functional {
    template <typename ReturnType, typename... Args> class BoundArgFunction {
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
        BoundArgFunction(std::function<ReturnType(Args...)> fp) : fp(fp) {}

        BoundArgFunction<ReturnType, Args...>& BindArgs(Args &&... a) {
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

    namespace Traits {
        template<typename Signature>
        struct FunctionTraits; 

        template<typename Ret_, typename... Args_>
        struct FunctionTraits<Ret_(Args_...)> {
            using ReturnType = Ret_; 
            using Arguments = std::tuple<Args_...>; 
        }; 

        template <typename Ret_, typename ... Args_>
        struct FunctionTraits<Ret_(*)(Args_...)> : FunctionTraits<Ret_(Args_...)> {}; 
    }


    template <typename Signature>
    class Function {
        private:
        std::function<Signature> fp; 
        protected:
        using ReturnType = typename Traits::FunctionTraits<Signature>::ReturnType; 
        using Arguments = typename Traits::FunctionTraits<Signature>::Arguments; 

        public:
        Function(Signature * fp) : fp(fp) {}
        
        template<typename... Args>
        ReturnType Invoke(Args&&... args) {
            if constexpr (std::is_same_v<ReturnType, void>) {
                fp(args...);
                return;  
            }
            return fp(args...); 
        }

        template<typename... Args>
        ReturnType operator()(Args&&... args) {
            if constexpr (std::is_same_v<ReturnType, void>) {
                fp(args...);
                return;  
            }
            return fp(args...); 
        }

        auto CreateBoundFunction() {
            return BoundArgFunction{fp}; 
        }
        
    };



}


