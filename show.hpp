#pragma once

#include <type_traits>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <tuple>

namespace show{

template<typename T>
struct Show {
    static std::string show(const T&){
        static_assert(expand_to_false<T>(), "no instance for class Show");
    }
private:
    template<typename>
    static constexpr bool expand_to_false(){return false;}
};

template<typename T>
struct Show_default{
    static std::string show(const T& x){
        std::stringstream out;
        out << x;
        return out.str();
    }
};

template<typename T>
std::string show(T&& x){
    return Show<T>::show(std::forward<T>(x));
};

template<typename T>
struct Show_fwd_iterable{
    static std::string show(const T& t){
        std::string res{};
        auto it = std::begin(t);
        auto finish = std::end(t);
        res += "[";
        if (it != finish){
            res += Show<decltype(*it)>::show(*it);
            ++it;
            for (;it != finish; ++it){
                res += ", ";
                res += Show<decltype(*it)>::show(*it);
            }
        }
        res += "]";
        return res;
    }
};

template<typename T>
struct Show_tuple{
    static std::string show(const T& t){
        std::string res{};
        res += '(';
        show_tuple_help(res, t, Nat<std::tuple_size<T>::value>{});
        res += ')';
        return res;
    }
private:
    template<size_t> struct Nat{};

    template<size_t i>
    static void show_tuple_help(std::string& res, const T& t, Nat<i>){
        show_tuple_help(res, t, Nat<i-1>{});
        res += Show<decltype(std::get<i-1>(t))>::show(std::get<i-1>(t));
        if (i != std::tuple_size<T>::value)
            res += ", ";
    }
    static void show_tuple_help(std::string&, const T&, Nat<0>){}
};

template<typename T> struct Show<const T>   : public Show<T> {};
template<typename T> struct Show<T&>        : public Show<T> {};
template<typename T> struct Show<T&&>       : public Show<T> {};
template<typename T> struct Show<const T&>  : public Show<T> {};
template<typename T> struct Show<const T&&> : public Show<T> {};

template<> struct Show<char>:           public Show_default<char> {};
template<> struct Show<char16_t>:       public Show_default<char16_t> {};
template<> struct Show<char32_t>:       public Show_default<char32_t> {};
template<> struct Show<wchar_t>:        public Show_default<wchar_t> {};
template<> struct Show<unsigned char>:  public Show_default<unsigned char> {};
template<> struct Show<short>:          public Show_default<short> {};
template<> struct Show<unsigned short>: public Show_default<unsigned short> {};
template<> struct Show<int>:            public Show_default<int> {};
template<> struct Show<unsigned>:       public Show_default<unsigned> {};
template<> struct Show<long>:           public Show_default<long> {};
template<> struct Show<unsigned long>:  public Show_default<unsigned long> {};
template<> struct Show<float>:          public Show_default<float> {};
template<> struct Show<double>:         public Show_default<double> {};
template<> struct Show<long double>:    public Show_default<long double> {};

template<size_t N, typename T>
struct Show<T[N]> : public Show_fwd_iterable<T[N]>{};

template<typename T, typename A>
struct Show<std::vector<T, A>> : public Show_fwd_iterable<std::vector<T, A>> {};

template<typename C, typename T, typename A>
struct Show<std::basic_string<C, T, A>> {
    static std::string show(const std::basic_string<C, T, A>& x){
        return '"' + x + '"';
    }
};

template<typename A, typename B>
struct Show<std::pair<A, B>> {
    static std::string show(const std::pair<A, B>& p){
        return "(" + Show<A>::show(p.first) + ", " + Show<B>::show(p.second) + ")";
    }
};

template<size_t N>
struct Show<char[N]> {
    static std::string show(char const(&s)[N]){
        return std::string("\"") + s + '"';
    }
};

template<>
struct Show<bool>{
    static std::string show(const bool& x){
        return x ? "true" : "false";
    }
};

template<typename...TS>
struct Show<std::tuple<TS...>> : public Show_tuple<std::tuple<TS...>> {};


void print(){
    std::cout << '\n';
}

template<typename T, typename...TS>
void print(T&& x, TS&&... xs){
    std::cout << show(std::forward<T>(x)) << " ";
    print(std::forward<TS>(xs)...);
}

} // namespace show

