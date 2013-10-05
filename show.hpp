#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <tuple>

/* TODO
    All char types to be handled correctly.
    More succint dispatch for char types, probably SFINAE-based.
*/

namespace show{

    template<typename T>
    std::string show(const T& t);

    namespace _dtl {

        template<typename T>
        constexpr bool expand_to_false(){return false;}

        template<std::size_t> struct Nat{};
        template<typename...> struct TypeList{};

        template<typename T> using IsIterable =
            TypeList<decltype(std::begin(T{})),
                     decltype(std::end(T{})),
                     decltype(++std::begin(T{})),
                     decltype(*std::begin(T{})),
                     decltype(std::begin(T{}) != std::end(T{}))>;

        template<typename T> using IsTrivial =
            decltype(std::stringstream{}.operator<<(T{}));

        struct Iterable;
        struct Trivial;
        struct Undefined;
        struct Tuple;
        struct Pair;
        struct String;
        struct Char;

        template<typename T>
        struct GetFormat{
            template<typename, typename = IsTrivial<T>>  static Trivial   check(void*);
            template<typename, typename = IsIterable<T>> static Iterable  check(void*);
            template<typename>                           static Undefined check(...);
            using type = decltype(check<T>(nullptr));
        };

        template<typename A, typename B>
        struct GetFormat<std::pair<A, B>> {using type = Pair;};

        template<typename... TS>
        struct GetFormat<std::tuple<TS...>> {using type = Tuple;};

        template<typename C, typename T, typename A>
        struct GetFormat<std::basic_string<C, T, A>> {using type = String;};

        template<typename T, size_t N>
        struct GetFormat<T[N]> {using type = Iterable;};

        template<size_t N> struct GetFormat<char[N]>     {using type = String;};
        template<>         struct GetFormat<const char*> {using type = String;};
        template<>         struct GetFormat<char*>       {using type = String;};
        template<>         struct GetFormat<char>        {using type = Char;};

        struct Iterable{
            template<typename T>
            std::string operator()(const T& t){
                std::string res{};
                auto it = std::begin(t);
                auto finish = std::end(t);
                res += "[";
                if (it != finish){
                    res += show(*it);
                    ++it;
                    for (;it != finish; ++it){
                        res += ", ";
                        res += show(*it);
                    }
                }
                res += "]";
                return res;
            }
        };

        struct Trivial{
            template<typename T>
            std::string operator()(const T& t){
                std::stringstream fmt{};
                fmt.operator<<(t);
                return fmt.str();
            }
        };

        struct String{
            template<typename T>
            std::string operator()(const T& t){
                return "\"" + std::string(t) + "\"";
            }
        };

        struct Pair{
            template<typename A, typename B>
            std::string operator()(const std::pair<A, B>& p){
                return "(" + show(p.first) + ", " + show(p.second) + ")";
            }
        };

        struct Tuple{
            template <typename... TS>
            std::string operator()(const std::tuple<TS...>& tpl) {
                std::string res{};
                res += "(";
                helper(res, tpl, Nat<sizeof...(TS) - 1>{});
                res += ")";
                return res;
            }

            template<typename T, std::size_t N>
            void helper(std::string& res, const T &tpl, Nat<N>){
                helper(res, tpl, Nat<N-1>{});
                res += ", ";
                res += show(std::get<N>(tpl));
            }

            template<typename T>
            void helper(std::string& res, const T &tpl, Nat<0>){
                res += show(std::get<0>(tpl));
            }
        };

        struct Char{
            template<typename T>
            std::string operator()(const T& t){
                return std::string("'") + t + '\'';
            }
        };

        struct Undefined{
            template<typename T>
            std::string operator()(const T&){
                static_assert(expand_to_false<T>(), "Show undefined for type.");
                return "";
            }
        };

    } // namespace _dtl

    template<typename T>
    std::string show(const T& t){
        return typename _dtl::GetFormat<T>::type{}(t);
    }

    void print(){
        std::cout << std::endl;
    }

    template<typename T, typename... TS>
    void print(const T& t, const TS&... ts){
        std::cout << show(t) << ' ';
        print(ts...);
    }

} // namespace show





