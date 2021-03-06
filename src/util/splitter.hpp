/*
 * splitter.hpp
 */
#ifndef SPLITTER_HPP
#define SPLITTER_HPP
#include <string>
#include <iterator>
#include <cstddef>
class Splitter {
public:
  typedef std::string string;
  typedef std::pair<string::const_iterator, string::const_iterator> Range;
  class iterator : public std::iterator<std::input_iterator_tag, string, std::ptrdiff_t, 
		   const string*, const string&> {
  public:
    iterator() : _spl(0), _b(string::npos), _e(string::npos), _cur() {}
    iterator(const Splitter& spl, string::size_type i) 
      : _spl(&spl), _b(string::npos), _e(i), _cur() {get_next();}
    iterator(const iterator& it) : _spl(it._spl), _b(it._b), _e(it._e), _cur(it._cur) {}
    ~iterator() {}
    reference operator*() { set_cur(); return _cur;}
    pointer operator->() { set_cur(); return &_cur;}
    iterator& operator++() { get_next(); return *this;}
    iterator operator++(int) { iterator tmp(*this); operator++(); return tmp;}
    bool operator==(const iterator& it) const { return (_b == it._b);}
    bool operator!=(const iterator& it) const { return !(*this == it);}
    iterator& operator=(const iterator& it) 
    { _spl = it._spl; _b = it._b; _e = it._e; _cur = it._cur; return *this;}
    Range range() const { return make_pair(pos_to_iter(_b), pos_to_iter(_e));}
  private:
    const Splitter*   _spl;
    string::size_type _b;
    string::size_type _e;
    string            _cur;
    string::const_iterator pos_to_iter(const string::size_type& pos) const {
      const string& s = _spl->str();
      return (pos == string::npos ? s.end() : (s.begin() + pos));
    }
    void set_cur() { if (_cur.empty()) { _cur.assign(pos_to_iter(_b), pos_to_iter(_e));}}
    void get_next() {
      _cur.clear();
      if (_e == string::npos) {
	_b = string::npos;
	return;
      }
      const string& s = _spl->str();
      const string& d = _spl->delim();
      _b = s.find_first_not_of(d, _e);
      if (_b == string::npos) {
	_e = string::npos;
	return;
      }
      _e = s.find_first_of(d, _b);
    }
  };
  Splitter(const string& str, const string& delim) : _str(str), _delim(delim) {}
  Splitter(const Splitter& spl) : _str(spl._str), _delim(spl._delim) {}
  ~Splitter() {}
  iterator begin() const { return iterator(*this, 0);}
  iterator end() const { return iterator(*this, string::npos);}
  const string& str() const { return _str;}
  const string& delim() const { return _delim;}
private:
  const string& _str;
  const string  _delim;
  Splitter();
};
class Splitter1 {
public:
  typedef std::string string;
  typedef std::pair<string::const_iterator, string::const_iterator> Range;
  class iterator : public std::iterator<std::input_iterator_tag, string, std::ptrdiff_t, 
		   const string*, const string&> {
  public:
    iterator() : _spl(0), _b(string::npos), _e(string::npos), _cur() {}
    iterator(const Splitter1& spl, string::size_type i) 
      : _spl(&spl), _b(i), _e(string::npos), _cur() { get_first();}
    iterator(const iterator& it) : _spl(it._spl), _b(it._b), _e(it._e), _cur(it._cur) {}
    ~iterator() {}
    reference operator*() { set_cur(); return _cur;}
    pointer operator->() { set_cur(); return &_cur;}
    iterator& operator++() { get_next(); return *this;}
    iterator operator++(int) { iterator tmp(*this); operator++(); return tmp;}
    bool operator==(const iterator& it) const { return (_b == it._b);}
    bool operator!=(const iterator& it) const { return !(*this == it);}
    iterator& operator=(const iterator& it) 
    { _spl = it._spl; _b = it._b; _e = it._e; _cur = it._cur; return *this;}
    Range range() const { return make_pair(pos_to_iter(_b), pos_to_iter(_e));}
  private:
    const Splitter1*   _spl;
    string::size_type  _b;
    string::size_type  _e;
    string             _cur;
    string::const_iterator pos_to_iter(const string::size_type& pos) const {
      const string& s = _spl->str();
      return (pos == string::npos ? s.end() : (s.begin() + pos));
    }
    void set_cur() { if (_cur.empty()) { _cur.assign(pos_to_iter(_b), pos_to_iter(_e));}}
    void get_first() {
      _cur.clear();
      if (_b == string::npos || _spl->str().size() < _b) {
	_b = string::npos;
	_e = string::npos;
	return;
      }
      const string& s = _spl->str();
      const string& d = _spl->delim();
      _e = s.find_first_of(d, _b);
    }
    void get_next() {
      _cur.clear();
      if (_e == string::npos) {
	_b = string::npos;
	return;
      }
      const string& s = _spl->str();
      const string& d = _spl->delim();
      _b = (_e + 1); // _b may be s.size() <- not equal to end()
      _e = s.find_first_of(d, _b);
    }
  };
  Splitter1(const string& str, const string& delim) : _str(str), _delim(delim) {}
  Splitter1(const Splitter1& other) : _str(other._str), _delim(other._delim) {}
  ~Splitter1() {}
  iterator begin() const { return iterator(*this, 0);}
  iterator end() const { return iterator(*this, string::npos);}
  const string& str() const { return _str;}
  const string& delim() const { return _delim;}
private:
  const string& _str;
  const string  _delim;
  Splitter1();
};
#endif
