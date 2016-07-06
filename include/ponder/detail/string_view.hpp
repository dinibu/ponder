#pragma once

#ifndef PONDER_STRING_VIEW_HPP
#define PONDER_STRING_VIEW_HPP

/*
 Boost Software License - Version 1.0 - August 17th, 2003
 
 Permission is hereby granted, free of charge, to any person or organization
 obtaining a copy of the software and accompanying documentation covered by
 this license (the "Software") to use, reproduce, display, distribute,
 execute, and transmit the Software, and to prepare derivative works of the
 Software, and to permit third-parties to whom the Software is furnished to
 do so, all subject to the following:
 
 The copyright notices in the Software and this entire statement, including
 the above license grant, this restriction and the following disclaimer,
 must be included in all copies of the Software, in whole or in part, and
 all derivative works of the Software, unless such copies or derivative
 works are solely in the form of machine-executable object code generated by
 a source language processor.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 */

#include <cstddef> // ptrdiff_t
#include <string>
#include <limits>
#include <iterator>
#include <stdexcept>
#include <algorithm>

namespace ponder {
namespace detail {
    
#define CONSTEXPR_BACKUP CONSTEXPR
#undef CONSTEXPR
    
#if defined(_HAS_CONSTEXPR) ||  __cplusplus >= 201103L
#   define CONSTEXPR constexpr
#else
#   define CONSTEXPR
#endif
#if __cplusplus >= 201402L
#   define CONSTEXPR_CPP14 constexpr
#else
#   define CONSTEXPR_CPP14
#endif
    
template<typename T>
class pointer_iterator :public std::iterator<std::random_access_iterator_tag, T>
{
public:
    typedef T* pointer;
    typedef T& reference;
    typedef std::ptrdiff_t difference_type;
    pointer_iterator() :p_(0) {}
    pointer_iterator(T* x) :p_(x) {}
    pointer_iterator(const pointer_iterator& it) : p_(it.p_) {}
    
    reference operator*() const { return *p_; }
    pointer_iterator operator+ (difference_type n) const { return pointer_iterator(p_ + n); }
    pointer_iterator& operator+= (difference_type n) { p_ += n; return *this; }
    pointer_iterator operator- (difference_type n) const { return pointer_iterator(p_ - n); }
    pointer_iterator& operator-= (difference_type n) { p_ -= n; return *this; }
    
    difference_type operator- (pointer_iterator o) const { return p_ - o.p_; }
    
    pointer_iterator& operator++() { p_++; return *this; }
    pointer_iterator  operator++(int) { pointer_iterator tmp(*this); operator++(); return tmp; }
    pointer_iterator& operator--() { p_--; return *this; }
    pointer_iterator  operator--(int) { pointer_iterator tmp(*this); operator--(); return tmp; }
    bool operator==(const pointer_iterator& rhs) { return p_ == rhs.p_; }
    bool operator!=(const pointer_iterator& rhs) { return p_ != rhs.p_; }
    bool operator<(const pointer_iterator& rhs) { return p_ < rhs.p_; }
    bool operator>(const pointer_iterator& rhs) { return rhs < *this; }
    bool operator<=(const pointer_iterator& rhs) { return !(*this > rhs); }
    bool operator>=(const pointer_iterator& rhs) { return !(*this < rhs); }
private:
    pointer p_;
};


template<typename CharT, typename Traits = std::char_traits<CharT> >
class basic_string_view
{
public:
    
    typedef Traits traits_type;
    typedef CharT value_type;
    typedef CharT* pointer;
    typedef const CharT* const_pointer;
    typedef CharT& reference;
    typedef const CharT& const_reference;
    
    typedef pointer_iterator<const CharT> const_iterator;
    typedef const_iterator iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef const_reverse_iterator reverse_iterator;
    
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    
#if defined(_HAS_CONSTEXPR) ||  __cplusplus >= 201103L
    static constexpr size_type npos = size_type(-1);
#else
    static const size_type npos = size_type(-1);
#endif
    
    CONSTEXPR basic_string_view() : start_(0),end_(0) {}
    CONSTEXPR basic_string_view(const basic_string_view& other)
        :   start_(other.start_),end_(other.end_){}
    template<class Allocator>
    basic_string_view(const std::basic_string<CharT, Traits, Allocator>& str) :start_(0),end_(0){
        if (!str.empty())
        {
            start_ = &str[0];
            end_ = start_ + str.size();
        }
    }
    CONSTEXPR basic_string_view(const CharT* s, size_type count) : start_(s),end_(s+count) {}
    CONSTEXPR basic_string_view(const CharT* s) : start_(s), end_(s + strlen(s)) {}
    
    basic_string_view& operator=(const basic_string_view& view) {
        this->start_ = view.start_; this->end_ = view.end_; return *this;
    }
    
    CONSTEXPR const_iterator begin() const { return start_; }
    CONSTEXPR const_iterator cbegin() const { return start_; }
    
    CONSTEXPR const_iterator end() const { return end_; }
    CONSTEXPR const_iterator cend() const { return end_; }
    
    CONSTEXPR const_reverse_iterator rbegin() const { return const_reverse_iterator(end_); }
    CONSTEXPR const_reverse_iterator rend() const { return const_reverse_iterator(start_); }
    
    CONSTEXPR const_reference operator[](size_type pos) const
    {
#if __cplusplus >= 201402L
        assert(pos < size());
#endif
        return *(start_ + pos);
    }
    CONSTEXPR const_reference at(size_type pos) const
    {
        return pos >= size()? throw std::out_of_range("basic_string_view at out of range")
                            : *(start_ + pos);
    }
    CONSTEXPR const_reference front() const { return *start_; }
    CONSTEXPR const_reference back() const { return *end_; }
    CONSTEXPR const_pointer data() const { return &(*start_); }
    
    CONSTEXPR size_type size() const { return std::distance(begin(), end()); }
    CONSTEXPR size_type length() const { return std::distance(begin(), end()); }
    CONSTEXPR size_type max_size() const { return std::numeric_limits<size_type>::max()/ 2; }
    CONSTEXPR bool empty() const { return size() == 0; }
    CONSTEXPR_CPP14 void remove_prefix(size_type n)
    {
        assert(n < size());
        start_ += n;
    }
    CONSTEXPR_CPP14 void remove_suffix(size_type n)
    {
        assert(n < size());
        end_ -= n;
    }
    CONSTEXPR_CPP14 void swap(basic_string_view& v)
    {
        std::swap(start_,v.start_);
        std::swap(end_,v.end_);
    }
#if __cplusplus >= 201103L
    template<class Allocator = std::allocator<CharT> >
    std::basic_string<CharT, Traits, Allocator>
    to_string(const Allocator& a = Allocator()) const
    {
        return std::basic_string<CharT, Traits, Allocator>(begin(),end());
    }
    // Explicit conversion disallows assignment conversion.
    template<class Allocator>
    /*explicit*/ operator std::basic_string<CharT, Traits, Allocator>() const
    {
        return std::basic_string<CharT, Traits, Allocator>(begin(), end());
    }
#else
    std::basic_string<CharT, Traits>
    to_string() const
    {
        return std::basic_string<CharT, Traits>(begin(),end());
    }
    template<class Allocator>
    operator std::basic_string<CharT, Traits, Allocator>() const
    {
        return std::basic_string<CharT, Traits, Allocator>(begin(), end());
    }
#endif
    size_type copy(CharT* dest, size_type count, size_type pos = 0) const
    {
        if (pos >= size()) { throw std::out_of_range("basic_string_view::copy out of range"); }
        for (int i = 0; i < size();++i)
        {
            dest[i] = operator[](i + pos);
        }
    }
    CONSTEXPR basic_string_view
    substr(size_type pos = 0, size_type count = npos) const
    {
        return pos >= size() ? throw std::out_of_range("basic_string_view::substr out of range"):
        (count > size() - pos) ? substr(pos,size() - pos) : basic_string_view(data() + pos, count);
    }
    CONSTEXPR_CPP14 int compare(basic_string_view v) const
    {
        const int r = traits_type::compare(data(), v.data(), std::min(size(), v.size()));
        return r==0 ? static_cast<int>(size()-v.size()) : r;
    }
    CONSTEXPR_CPP14 int compare(size_type pos1, size_type count1,
                                basic_string_view v) const
    {return substr(pos1, count1).compare(v);}
    CONSTEXPR_CPP14 int compare(size_type pos1, size_type count1, basic_string_view v,
                                size_type pos2, size_type count2) const
    {return substr(pos1, count1).compare(v.substr(pos2, count2));}
    CONSTEXPR_CPP14 int compare(const CharT* s) const
    { return compare(basic_string_view(s)); }
    CONSTEXPR_CPP14 int compare(size_type pos1, size_type count1,const CharT* s) const
    {
        return substr(pos1, count1).compare(basic_string_view(s));
    }
    CONSTEXPR_CPP14 int compare(size_type pos1, size_type count1,
                                const CharT* s, size_type count2) const
    {
        return substr(pos1, count1).compare(basic_string_view(s, count2));
    }
    
    CONSTEXPR size_type find(basic_string_view v, size_type pos = 0) const
    {
        return pos >= size() ? npos:
        find_to_pos(std::search(begin() + pos, end(), v.begin(), v.end()));
    }
    CONSTEXPR size_type find(CharT c, size_type pos = 0) const
    {return find(basic_string_view(&c, 1), pos);}
    CONSTEXPR size_type find(const CharT* s, size_type pos, size_type count) const
    {return find(basic_string_view(s, count), pos);}
    CONSTEXPR size_type find(const CharT* s, size_type pos = 0) const
    {return find(basic_string_view(s), pos);}
    
    CONSTEXPR size_type rfind(basic_string_view v, size_type pos = npos) const
    {
        return pos > size() ? rfind(v, size()) :
        rfind_to_pos(std::search(const_reverse_iterator(begin() + pos), rend(),
                                 v.rbegin(), v.rend()), v.size());
    }
    CONSTEXPR size_type rfind(CharT c, size_type pos = npos) const
    {return rfind(basic_string_view(&c, 1), pos);}
    CONSTEXPR size_type rfind(const CharT* s, size_type pos, size_type count) const
    {return rfind(basic_string_view(s, count), pos);}
    CONSTEXPR size_type rfind(const CharT* s, size_type pos = npos) const
    {return rfind(basic_string_view(s), pos);}
    
    
    CONSTEXPR size_type find_first_of(basic_string_view v, size_type pos = 0) const
    {
        return pos >= size()? npos: v.find(*(start_ + pos)) != npos?pos:find_first_of(v, pos + 1);
    }
    CONSTEXPR size_type find_first_of(CharT c, size_type pos = 0) const
    { return find_first_of(basic_string_view(&c, 1), pos); }
    CONSTEXPR size_type find_first_of(const CharT* s, size_type pos, size_type count) const
    { return find_first_of(basic_string_view(s, count), pos); }
    CONSTEXPR size_type find_first_of(const CharT* s, size_type pos = 0) const
    { return find_first_of(basic_string_view(s), pos);}
    
    CONSTEXPR size_type find_last_of(basic_string_view v, size_type pos = npos) const
    {
        return pos == 0 ? npos:
        pos >= size() ? find_last_of(v,size()-1) :
        v.find(*(start_ + pos)) != npos ? pos : find_last_of(v, pos - 1);
    }
    CONSTEXPR size_type find_last_of(CharT c, size_type pos = npos) const
    { return find_last_of(basic_string_view(&c, 1), pos); }
    CONSTEXPR size_type find_last_of(const CharT* s, size_type pos, size_type count) const
    { return find_last_of(basic_string_view(s, count), pos); }
    CONSTEXPR size_type find_last_of(const CharT* s, size_type pos = npos) const
    { return find_last_of(basic_string_view(s), pos); }
    
    CONSTEXPR size_type  find_first_not_of(basic_string_view v, size_type pos = 0) const
    {
        return pos >=
            size() ? npos : v.find(*(start_ + pos)) == npos ? pos : find_first_of(v, pos + 1);
    }
    CONSTEXPR size_type  find_first_not_of(CharT c, size_type pos = 0) const
    { return find_first_not_of(basic_string_view(&c, 1), pos); }
    CONSTEXPR size_type  find_first_not_of(const CharT* s, size_type pos, size_type count) const
    { return find_first_not_of(basic_string_view(s, count), pos); }
    CONSTEXPR size_type find_first_not_of(const CharT* s, size_type pos = 0) const
    { return find_first_not_of(basic_string_view(s), pos); }
    
    CONSTEXPR size_type find_last_not_of(basic_string_view v, size_type pos = npos) const
    {
        return pos == 0 ? npos :
        pos >= size() ? find_last_not_of(v, size() - 1) :
        v.find(*(start_ + pos)) == npos ? pos : find_last_of(v, pos - 1);
    }
    CONSTEXPR size_type find_last_not_of(CharT c, size_type pos = npos) const
    { return find_last_not_of(basic_string_view(&c, 1), pos); }
    CONSTEXPR size_type find_last_not_of(const CharT* s, size_type pos, size_type count) const
    { return find_last_not_of(basic_string_view(s, count), pos); }
    CONSTEXPR size_type find_last_not_of(const CharT* s, size_type pos = npos) const
    { return find_last_not_of(basic_string_view(s), pos); }
    
    CONSTEXPR_CPP14 bool operator==(const basic_string_view& rhs) const {return compare(rhs) == 0;}
    CONSTEXPR_CPP14 bool operator!=(const basic_string_view& rhs) const {return compare(rhs) != 0;}
    CONSTEXPR_CPP14 bool operator<(const basic_string_view& rhs) const {return compare(rhs) < 0;}
    CONSTEXPR_CPP14 bool operator>(const basic_string_view& rhs) const {return rhs < *this;}
    CONSTEXPR_CPP14 bool operator<=(const basic_string_view& rhs) const {return !(*this > rhs);}
    CONSTEXPR_CPP14 bool operator>=(const basic_string_view& rhs) const {return !(*this < rhs);}
    
private:
    
    CONSTEXPR basic_string_view(const iterator& s, const iterator& e) : start_(s), end_(e) {}
    iterator start_;
    iterator end_;
    
    CONSTEXPR size_type find_to_pos(const_iterator it) const
    {
        return it == end() ? npos : size_type(it - begin());
    }
    CONSTEXPR size_type rfind_to_pos(const_reverse_iterator it,size_type search_size) const
    {
        return it == rend() ? npos : size_type(it.base() - begin() - search_size);
    }
    
    CONSTEXPR static const_pointer strlen_nullpos(const_pointer str)
    {
        return *str == '\0' ? str:strlen_nullpos(str + 1);
    }
    CONSTEXPR static size_type strlen(const_pointer str)
    {
        return strlen_nullpos(str) - str;
    }
};

#undef CONSTEXPR
#define CONSTEXPR CONSTEXPR_BACKUP
#undef CONSTEXPR_BACKUP
#undef CONSTEXPR_CPP14
    
typedef basic_string_view<char> string_view;
typedef basic_string_view<wchar_t> wstring_view;

static inline std::ostream& operator << (std::ostream& os, string_view const& value) {
    os << value.to_string();
    return os;
}
    
} // namespace detail
} // namespace ponder

#endif // PONDER_STRING_VIEW_HPP
