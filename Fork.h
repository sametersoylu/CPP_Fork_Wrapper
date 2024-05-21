#pragma once
#include <any>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <unistd.h>
#include <wait.h>
#include "Functional.h"

namespace Fork {

    namespace Internal {
        using pipe_t = int[2]; 
        class base_exception : public std::exception {
            protected:
            std::string msg; 

            public:
            base_exception(const char * msg) : msg(msg) {}
            base_exception(std::string msg) : msg(msg) {}
            base_exception() : msg("exception occurred.") {}

            const char * what() {
                return msg.c_str();
            }
        };

        class pipe_exception : public base_exception {
            public:
            pipe_exception() : base_exception("pipe exception occurred.") {}
            pipe_exception(const char * msg) : base_exception(msg) {}
            pipe_exception(std::string msg) : base_exception(msg) {}
        };

        class fork_exception : public base_exception {
            public:
            fork_exception() : base_exception("pipe exception occurred.") {}
            fork_exception(const char * msg) : base_exception(msg) {}
            fork_exception(std::string msg) : base_exception(msg) {}
        };

        class base_fork {
            protected:
            pid_t pid = -1;

            void fork_() {
                pid = fork(); 
                if(pid == -1) {
                    throw fork_exception(); 
                }
            }

            public: 
            template <typename ReturnType1, typename... Args1, typename ReturnType2, typename... Args2>
            ReturnType1 Invoke(FunctionWrapper<ReturnType1, Args1...> main, FunctionWrapper<ReturnType2, Args2...> child) {
                fork_(); 

                std::any res;
                if (pid == 0) {
                    child();
                    exit(1);
                } else {
                    if constexpr(!std::is_same<ReturnType1, void>::value)
                        res = main();
                    main();
                }
                if constexpr (std::is_same<ReturnType1, void>::value) {
                    return;
                }
                return std::any_cast<ReturnType1>(res);
            }

            template<typename ReturnType, typename... Args>
            void Invoke(FunctionWrapper<ReturnType, Args...> child) {
                pid = fork();
                if (pid == -1) {
                    std::cerr << "Fork error" << std::endl;
                } else if (pid == 0) {
                    child();
                    exit(1);
                }
            }
        }; 
    }

    class Fork : public Internal::base_fork {
    public:
        Fork() {}
        
        template <typename ReturnType1, typename... Args1, typename ReturnType2, typename... Args2>
        ReturnType1 operator()(FunctionWrapper<ReturnType1, Args1...> main, FunctionWrapper<ReturnType2, Args2...> child) {
            if constexpr (std::is_same_v<ReturnType1, void>) return; 
            return Internal::base_fork::Invoke(main, child);
        }

        template<typename ReturnType, typename... Args>
        void operator()(FunctionWrapper<ReturnType, Args...> child) {
            Internal::base_fork::Invoke(child);
        }
    };

    template<size_t buffer_size = 1024>
    class PipedFork : public Internal::base_fork {
        private: 
        std::string pipe_result;
        std::unique_ptr<char[]> buffer {new char[buffer_size]}; 
        Internal::pipe_t fd;

        public: 

        template<typename ReturnType, typename... Args>
        PipedFork<buffer_size>& Invoke(FunctionWrapper<ReturnType, Args...> child) {
            if(pipe(fd) == -1) {
                throw Internal::pipe_exception("pipe() failed.");
            }
            
            fork_(); 

            if(pid == 0) {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO); 
                close(fd[1]);

                child(); 

                exit(1); 
            } else {
                close(fd[1]); 
                int bytes_read; 
                while((bytes_read = read(fd[0], buffer.get(), buffer_size - 1)) > 0) {
                    buffer[bytes_read] = '\0';
                    pipe_result.append(buffer.get(), bytes_read);
                }

                close(fd[0]);
                wait(NULL); 
            }

            return *this; 
        }

        template<typename ReturnType, typename... Args>
        PipedFork<buffer_size>& operator()(FunctionWrapper<ReturnType, Args...> child) {
            PipedFork::Invoke(child);
            return *this;
        }

        std::string PipeResult() {
            const auto data = pipe_result;
            pipe_result.clear();
            return data; 
        }

        template<typename Ret_>
        Ret_ PipeResultTo() {
            std::istringstream ss(pipe_result); 
            Ret_ result; 
            ss >> result; 
            return result; 
        }

    }; 

};