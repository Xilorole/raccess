/*
 * author: hisanori kiryu
 */
#ifndef UTIL__UTIL_HPP
#define UTIL__UTIL_HPP

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cmath>
#include <cctype>
#include <climits>
#include <cfloat>
#include <cstring>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <locale>

#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <set>

#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <limits>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
using namespace std;
// using namespace __gnu_cxx;

#include <ext/hash_map>
#include <ext/hash_set>
// for hash_map<string,*>
namespace __gnu_cxx {
  template< typename CharT, typename Traits, typename Alloc >
  struct hash< const std::basic_string<CharT, Traits, Alloc> > {
    std::size_t operator()(const std::basic_string<CharT, Traits, Alloc>& s) const {
      const std::collate<CharT>& c = std::use_facet<std::collate<CharT> >(std::locale());
      return c.hash(s.c_str(), s.c_str() + s.length());
    }
  };
  template< typename CharT, typename Traits, typename Alloc >
  struct hash< std::basic_string<CharT, Traits, Alloc> >
    : hash< const std::basic_string<CharT, Traits, Alloc> > {};
}

#include "error.hpp"
#include "array.hpp"
#include "matrix.hpp"
#include "splitter.hpp"
#include "str_conv.hpp"
#include "alpha.hpp"
#include "string_index.hpp"
#include "proc_fun.hpp"
#include "str_dump.hpp"
#include "bucket.hpp"

inline std::vector<std::string>&
splitv(std::vector<std::string>& v, 
	const std::string& str, const std::string& delim = " \f\n\r\t\v") {
  Splitter spl(str, delim); v.clear();
  std::copy(spl.begin(), spl.end(), std::back_inserter(v));
  return v;
}
inline std::vector<std::string>
splitv(const std::string& str, const std::string& delim = " \f\n\r\t\v")
{ std::vector<std::string> v; return splitv(v, str, delim);}

inline std::vector<std::string>&
split1v(std::vector<std::string>& v, 
	const std::string& str, const std::string& delim = " \f\n\r\t\v") {
  Splitter1 spl(str, delim); v.clear();
  std::copy(spl.begin(), spl.end(), std::back_inserter(v));
  return v;
}
inline std::vector<std::string>
split1v(const std::string& str, const std::string& delim = " \f\n\r\t\v")
{ std::vector<std::string> v; return split1v(v, str, delim);}

template <typename T> inline std::vector<T>&
splitvt(std::vector<T>& v, const std::string& str, const std::string& delim = " \f\n\r\t\v") {
  Splitter spl(str, delim); v.clear();
  std::transform(spl.begin(), spl.end(), std::back_inserter(v), stot_fun_t<T>());
  return v;
}
template <typename T> inline std::vector<T>
splitvt(const std::string& str, const std::string& delim = " \f\n\r\t\v") 
{ std::vector<T> v; return splitvt<T>(v, str, delim);}
template <typename T> inline std::vector<T>&
split1vt(std::vector<T>& v, const std::string& str, const std::string& delim = " \f\n\r\t\v") {
  Splitter1 spl(str, delim); v.clear();
  std::transform(spl.begin(), spl.end(), std::back_inserter(v), stot_fun_t<T>());
  return v;
}
template <typename T> inline std::vector<T>
split1vt(const std::string& str, const std::string& delim = " \f\n\r\t\v") 
{ std::vector<T> v; return split1vt<T>(v, str, delim);}

//cannot be used for tab delimited lines
template<typename T> Array<T> tokenize(const string& str) {
  istringstream iss(str);
  return Array<T>(istream_iterator<T>(iss), istream_iterator<T>());
}

// inline void srandom(unsigned int n) { srand(n);} // only unsigned int precision is recognized
// inline size_t random(size_t n) { return (size_t)(n * (rand() / (1.0 + (double) RAND_MAX)));}
// inline double random_real() { return (rand() / (1.0 + (double) RAND_MAX));}
/* Warn: Mt19937 class is non-thread safe */
#include "mt19937.hpp" 
inline void srandom(unsigned int n) { Mt19937<unsigned int, int>::init_genrand(n);}
inline double random_real() { return Mt19937<unsigned int, int>::genrand_real2();} // [0, 1) interval
inline size_t random(size_t n) { return (size_t)(n * random_real());} // n should not exceed int_max
class Random { public: size_t operator()(size_t n) const { return random(n);}};

template <typename V, typename V1> void bootstrap(V& v, V1 const& v1) {
  for (typename V::iterator i = v.begin(); i != v.end(); ++i) { (*i) = v1[random(v1.size())];}
}
// template <typename V> void bootstrap(V& v, size_t n) 
// { for (typename V::iterator i = v.begin(); i != v.end(); ++i) { (*i) = random(n);}}
template <typename Iter> Iter random_sample_n(Iter first, Iter last, size_t n) {
  size_t m = std::distance(first, last); Check(n <= m);
  while (n-- > 0) { swap(*first, *(first + random(m))); --m; ++first;}
  return first;
}
template <typename RandIter, typename RandomNumberGenerator>
void random_subset(RandIter first, RandIter middle, RandIter last, RandomNumberGenerator rng) {
  std::size_t n = std::distance(first, middle);
  std::size_t m = std::distance(first, last);
  while (n-- > 0) {
    std::swap(*first, *(first + rng(m)));
    --m;
    ++first;
  }
}

inline std::string indent(const std::string& str0, int n) {
  const std::string indent_str(n, ' ');
  const std::string nl = "\n";
  std::istringstream iss(str0);
  std::ostringstream oss;
  std::string buf;
  while (std::getline(iss, buf)) {
    oss << indent_str << buf << nl;
  }
  return oss.str();
}
template <typename T> T round(T t) {
  if (t > 0) return floor(t + 0.5);
  if (t < 0) return ceil(t - 0.5);
  return 0;
}
inline std::string chomp(const std::string& s) {
  const std::string::size_type n = s.size();
  if (n > 0) {
    switch (s[n-1]) {
    case '\n':
      if (n > 1 && s[n-2] == '\r') {
	return s.substr(0, n-2);
      } else {
	return s.substr(0, n-1);
      }
      break;
    case '\r':
      return s.substr(0, n-1);
      break;
    }
  }
  return s;
}
inline std::string& chomp_1(std::string& s) {
  const std::string::size_type n = s.size();
  if (n > 0) {
    switch (s[n-1]) {
    case '\n':
      if (n > 1 && s[n-2] == '\r') {
	s.erase((n-2), 2);
      } else {
	s.erase((n-1), 1);
      }
      break;
    case '\r':
      s.erase((n-1), 1);
      break;
    }
  }
  return s;
}

// value[i] <= value[i+1] is assumed
// less_than_query.call(i) returns the boolean value (value[i] < query)
// let Sh:={i in [b, e)|query <= value[i]},
//     Sl:={i in [b, e)|value[i] < query}
// if not Sh.empty? then return min Sh else return e
template <typename T, typename Pred>
T lower_bound_1(T b, T e, Pred less_than_query) {// first_of_f in sequence tttttfffffff
  T lo = b;
  T hi = e;
  //assume lo <= hi
  while (lo != hi) {
    // lo < hi
    T mid = (lo + (hi-lo)/2);
    // because lo < hi, lo <= mid < hi is satisfied
    if (less_than_query(mid)) { // return (value[mid] < query) 
      lo = (mid+1); // lo += ((hi-lo)/2 + 1);
    } else {
      hi = mid;     // hi -= ((hi-lo)/2 + (hi-lo)%2);
    }
    // 0 <= lo <= hi <= n
    // (hi_new - lo_new) < (hi_old-lo_old)
    // lo <= max{i|value[i] < query} + 1
    // min{i|query<=value[i]} <= hi
  }
  // 0 <= lo == hi <= n
  // lo <= max{i|value[i] < query} + 1
  // min{i|query<=value[i]} <= hi
  return hi;
}
// value[i] <= value[i+1] is assumed
// return min{i in [b,e)| query < value[i]} if the set is not empty, else return e
template <typename T, typename Pred>
T upper_bound_1(T b, T e, Pred greater_than_query) {// first_of_t in sequence fffffttttt
  T lo = b;
  T hi = e;
  while (lo != hi) {
    //// mid = hi - (hi-lo)/2
    //// lo < mid <= hi is satisfied
    T mid = lo + (hi-lo)/2;
    // because lo < hi, lo <= mid < hi is satisfied
    if (greater_than_query(mid)) { // return (query < value[mid])
      hi = mid;
    } else {
      lo = (mid+1);
    }
  }
  return hi;
}

template <typename Pairs, typename Key>
struct find_by_key_less_than_query {
  const Pairs& _p; const Key& _k; 
  find_by_key_less_than_query(const Pairs& p, const Key& k) : _p(p), _k(k) {}
  bool operator()(int i) const {return (_p[i].first < _k);}
};
template <typename Pairs, typename Key, typename Value>
const Value& find_by_key(const Pairs& p, const Key& k, const Value& v0 = Value()) {
  find_by_key_less_than_query<Pairs, Key> pred(p, k);
  std::size_t i = lower_bound_1((std::size_t)0, p.size(), pred);
  return (((i < p.size()) && (p[i].first == k)) ? p[i].second : v0);
}

template <typename T, typename Block>
void each_multi_index(T rank, T n, Block& block) {
  Check(rank > 0 && n > 0);
  vector<T> a(rank);
  fill(a.begin(), a.end(), 0);
  T i = 0;
  T z = 0;
  while (true) {
    if (n <= a[i]) {
      if (i == 0) break;
      a[i] = 0;
      z /= n;
      ++a[--i];
    } else if (i == (rank-1)) {
      z *= n;
      z += a[i];
      if (block(a, z) != 0) break;
      z /= n;
      ++a[i];
    } else {
      z *= n;
      z += a[i];
      ++i;
    }
  }
}
template <typename T, typename Block> void each_prefix(T rank, T n, Block block) {
  Check(rank > 0 && n > 0); // root is not included
  vector<T> a(rank);
  vector<T> stride(rank);
  fill(a.begin(), a.end(), 0);
  for (T i = (rank - 1); i >= 0; --i) { 
    if (i == (rank - 1)) { stride[i] = (i == (rank - 1) ? 1 : (n * stride[i + 1]));}
  }
  T i = 0;
  T z = 0;
  while (true) {
    if (n <= a[i]) {
      if (i == 0) break;
      a[i] = 0;
      z /= n;
      ++a[--i];
    } else if (i == (rank-1)) {
      z *= n;
      z += a[i];
      if (block(a, (i + 1), z) != 0) break;
      z /= n;
      ++a[i];
    } else {
      z *= n;
      z += a[i];
      if (block(a, (i + 1), z * stride[i]) != 0) break; // zero filled for unused part
      ++i;
    }
  }
}
template <typename T, typename Block>  void each_sum_leq(T rank, T tot, Block block) {
  Check(rank > 0 && tot > 0);
  vector<T> a(rank);
  T n = tot;
  T d = 0;
  while (true) { // correct ?
    if (n < a[d]) {
      if (d == 0) break;
      a[d] = 0;
      n += a[--d];
      ++a[d];
    } else if (d == (rank - 1)) {
      if (block(a) != 0) break;
      ++a[d];
    } else {
      n -= a[d++];
    }
  }
}
struct upcase {char operator()(char c) const {return (char)(std::toupper((int)c));}};
struct downcase {char operator()(char c) const {return (char)(std::tolower((int)c));}};

// Pair cannot be a const type so as to differentiate Pair& from const Pair&
template <typename Pair>
struct select1st {
  typename Pair::first_type& operator()(Pair& p) const {return p.first;}
  const typename Pair::first_type& operator()(const Pair& p) const {return p.first;}
};
template <typename Pair>
struct select2nd {
  typename Pair::second_type& operator()(Pair& p) const {return p.second;}
  const typename Pair::second_type& operator()(const Pair& p) const {return p.second;}
};
template <typename T> struct ref_to_ptr {T* operator()(T& t) const {return &t;}};
template <typename T> struct const_ref_to_ptr {const T* operator()(const T& t) const {return &t;}};
template <typename T> struct ptr_to_ref {T& operator()(T* t) const {return *t;}};
template <typename T> struct const_ptr_to_ref {const T& operator()(const T* t) const {return *t;}};

template <typename Iter, typename T, typename Block>
T iter_inject(Iter first, Iter last, T t, Block block) {
  for (; first != last; ++first) {t = block(t, *first);}
  return t;
}

template <typename Block> struct block_each_with_index {
  const Block& _block; mutable int _index;
  block_each_with_index(const Block& block) : _block(block), _index(0) {}
  template <typename Arg> int operator()(Arg a) const {return _block(a, _index++);}
  template <typename Arg1, typename Arg2> 
  int operator()(Arg1 a1, Arg2 a2) const {return _block(a1, a2, _index++);}
  template <typename Arg1, typename Arg2, typename Arg3> 
  int operator()(Arg1 a1, Arg2 a2, Arg3 a3) const {return _block(a1, a2, a3, _index++);}
};
template <typename T, typename Block> struct block_inject {
  const Block& _block; T& _t;
  block_inject(T& t, const Block& block) : _block(block), _t(t) {}
  template <typename Arg> int operator()(Arg a) const {return _block(_t, a);}
  template <typename Arg1, typename Arg2> 
  int operator()(Arg1 a1, Arg2 a2) const {return _block(_t, a1, a2);}
  template <typename Arg1, typename Arg2, typename Arg3> 
  int operator()(Arg1 a1, Arg2 a2, Arg3 a3) const {return _block(_t, a1, a2, a3);}
};
template <typename Each, typename T, typename Block> 
T inject(Each each, const T& t0, const Block block) {
  T t = t0; each(block_inject<T, Block>(t, block)); return t;}
template <typename Each, typename Block> 
int each_with_index(Each each, Block block) {return each(block_each_with_index<Block>(block));}

template <typename Iter> std::pair<double, double> meansd(Iter first, Iter last) {
  double n  = 0.0;
  double mu = 0.0;
  double sd = 0.0;
  for (; first != last; ++first) {
    const double& x = (*first - mu);
    n  += 1.0;
    mu += (x / n);
    sd += (x * x * ((n - 1.0) / n));
  }
  if (n > 1.0) { sd = std::sqrt(sd / (n - 1.0));}
  return std::make_pair(mu, sd);
}
inline void vmeansd(vector<double>& mu, vector<double>& sd, const vector< vector<double> >& x) {
  if (x.empty()) { mu.clear(); sd.clear(); return;}
  int n = x.size();
  int m = x[0].size();
  mu.assign(m, 0.0);
  sd.assign(m, 0.0);
  for (int i = 0; i < n; ++i) {
    double num = (i + 1);
    const vector<double>& xi = x[i];
    for (int j = 0; j < m; ++j) {
      double d = (xi[j] - mu[j]);
      mu[j] += (d / num);
      sd[j] += (d * d * ((num - 1.0) / num));
    }
  }
  for (int j = 0; j < m; ++j) { sd[j] = sqrt(sd[j] / ((double)n - 1.0));}
}
template <typename Iter> std::pair<double, double> medianad(Iter first, Iter last) {
  if (first == last) return make_pair(0.0, 0.0);
  double mu = 0.0;
  double ad = 0.0;
  std::vector<double> v(first, last);
  typedef std::vector<double>::iterator Iter1;
  Iter1 i = (v.begin() + v.size() / 2);
  std::nth_element(v.begin(), i, v.end());
  if (v.size() % 2 == 1L) {
    mu = (*i);
  } else {
    Iter1 i1 = std::max_element(v.begin(), i);
    mu = 0.5 * ((*i1) + (*i));
  }
  for (Iter1 j = v.begin(); j != i; ++j) {
    ad += (mu - (*j));
  }
  for (Iter1 j = i; j != v.end(); ++j) {
    ad += ((*j) - mu);
  }
  ad /= (double)v.size();
  return std::make_pair(mu, ad);
}
template <typename Iter> std::pair<double, double> 
standardize(Iter first, Iter last, const std::pair<double, double>& p) {
  const double& fac = (1.0 / p.second);
  for (; first != last; ++first) { (*first) = ((*first) - p.first) * fac;}
  return p;
}
template <typename Iter> std::pair<double, double> vstandardize(Iter first, Iter last) 
{ return standardize(first, last, meansd(first, last));}
template <typename V> std::pair<double, double> 
vstandardize(V& v) { return vstandardize(v.begin(), v.end());}
template <typename T> class series {
public:
  series(const T& start) : next(start) {}
  T operator()() { return next++;}
private:
  T next;
};

template <typename Iter, typename Iter1> double kl_divergence(Iter b, Iter e, Iter1 b1) {
  double z = 0; // assuming prob normalized
  for (; b != e; ++b, ++b1) {
    if (*b <= 0) continue;
    if (*b1 <= 0) return HUGE_VAL;
    z += (*b) * std::log((*b) / (double)(*b1));
  }
  return std::max<double>(0.0, z);
}
template <typename V, typename V1> double kl_divergence(const V& v, const V1& v1) 
{ return kl_divergence(v.begin(), v.end(), v1.begin());}

template <typename Iter, typename Iter1> double kl_divergence_bits(Iter b, Iter e, Iter1 b1) {
  double z = 0; // assuming prob normalized
  for (; b != e; ++b, ++b1) {
    if (*b <= 0) continue;
    if (*b1 <= 0) return HUGE_VAL;
    z += (*b) * std::log((*b) / (double)(*b1));
  }
  z *= (1.0 / std::log(2.0));
  return std::max<double>(0.0, z);
}
template <typename V, typename V1> double kl_divergence_bits(const V& v, const V1& v1) 
{ return kl_divergence_bits(v.begin(), v.end(), v1.begin());}

inline std::string strip(const std::string& s) {
  const std::string& ws = " \f\n\r\t\v";
  const std::string::size_type& b = s.find_first_not_of(ws);
  if (b == std::string::npos) return "";
  const std::string::size_type& e = s.find_last_not_of(ws);
  return s.substr(b, ((e - b) + 1));
}
template <typename Key, typename Value>
const Value& find_value(const std::pair<Key, Value> table[], int table_size, 
			const Key& key, const Value& default_value = Value()) {
  for (int i = 0; i < table_size; ++i) {
    if (key == table[i].first) return table[i].second;
  }
  Die("bad key"); 
  return default_value;
}

template <typename T1, typename T2> 
struct cast_fun_t {T2 operator()(const T1& t) const {return (T2)t;}};

template <typename Func> Func file_each(const string& fname, Func f) {
  ifstream fi(fname.c_str()); 
  Check(fi, "cannot open file: ", fname); 
  string s; 
  while (getline(fi, s)) {if (f(fi, s)) return f;}
  s.clear();
  f(fi, s);
  return f;
}
template <typename Block> void file_each_line(string& fname, Block& block) {
  ifstream fi(fname.c_str());
  Check(fi, "cannot open file ", fname);
  string buf;
  while (getline(fi, buf)) {
    if (block(buf) != 0) break;
  }
  fi.close();
}

template <typename T> struct identity : public unary_function<T, T> 
{ T const& operator()(T const& t) const { return t;}};
inline void save_name(ostream& fo, const string& name) { fo << name << '\n';}
inline void save_n(ostream& fo) { fo << '\n';}
template <typename T, typename Conv> 
void save_val(ostream& fo, const string& name, const T& val, Conv conv)
{ fo << name << '\t' << conv(val) << '\n';}
template <typename T> void save_val(ostream& fo, const string& name, const T& val)
{ save_val(fo, name, val, ttos_fun(val));}
template <typename T> void save_val(ostream& fo, const T& t) { ttos(fo, t); fo << '\n';}
template <typename T> void save_obj(ostream& fo, const string& name, T& obj)
{ save_name(fo, name); obj.save(fo); save_n(fo);}
template <typename T> 
void save_vec(ostream& fo, const vector<T>& v) { save_vec(fo, v, identity<T>());}
template <typename T, typename Conv> 
void save_vec(ostream& fo, const vector<T>& v, Conv conv) 
{ transform(v.begin(), v.end(), ostream_iterator<typename Conv::result_type>(fo, ","), conv);}
template <typename Iter, typename Conv> 
void save_vec(ostream& fo, Iter vb, Iter ve, Conv conv) 
{ transform(vb, ve, ostream_iterator<typename Conv::result_type>(fo, ","), conv);}
template <typename Iter, typename Conv> 
void save_vec(ostream& fo, const string& name, Iter vb, Iter ve, Conv conv) 
{ fo << name << '\t'; save_vec(fo, vb, ve, conv); save_n(fo);}
template <typename V> void save_vec(ostream& fo, const string& name, const V& v) 
{ save_vec(fo, name, v.begin(), v.end(), identity<typename V::value_type>());}
template <typename V, typename Conv> 
void save_vec_n(ostream& fo, const string& name, const V& v, Conv conv) {
  if (!name.empty()) {fo << name << "\n";} 
  transform(v.begin(), v.end(), ostream_iterator<typename Conv::result_type>(fo, "\n"), conv); 
  fo << "\n";
}
template <typename V> void save_vec_n(ostream& fo, const string& name, V& v) 
{ save_vec_n(fo, name, v, identity<typename V::value_type>());}
template <typename V> void save_vec_n(ostream& fo, const string& name, const V& v) 
{ save_vec_n(fo, name, v, identity<typename V::value_type>());}

inline void load_name(istream& fi, const string& name) 
{ string s; Check(getline(fi, s) && (s == name), "bad format. no line for ", name, ". got ", s);}
inline void load_lines(istream& fi, vector<string>& v) 
{ v.clear(); string s; while (getline(fi, s)) { v.push_back(s);}}
inline void load_n(istream& fi) 
{ string s; Check((getline(fi, s) && s.empty()), "bad format. no empty line");}
template <typename T, typename Conv> 
void load_val(istream& fi, const string& name, T& t, Conv conv) {
  string s; 
  Check(getline(fi, s), "no line for ", name);
  const vector<string>& a = split1v(s, "\t"); 
  if (!(a.size() == 2 && a[0] == name)) {
    for (int i = 0; i < (int)a.size(); ++i) { cerr << str_dump(a[i]) << '\n';}
    Check(a.size() == 2 && a[0] == name, "name=", name, ",a.size()=", a.size(), ",a[0]=\"", a[0], "\"");
  }
  t = conv(a[1]);
}
template <typename T> void load_val(istream& fi, const string& name, T& t) 
{ load_val(fi, name, t, stot_fun((const T&)t));}
template <typename T> void load_obj(istream& fi, const string& name, T& obj) 
{ load_name(fi, name); obj.load(fi); load_n(fi);}
template <typename V, typename Conv> void load_vec(const string& s, V& v, Conv conv) 
{ Splitter u(s, ","); v.clear(); transform(u.begin(), u.end(), back_inserter(v), conv);}
template <typename V> 
void load_vec(const string& s, V& v) { load_vec(s, v, stot_fun(typename V::value_type()));}
template <typename V, typename Conv>
void load_vec(istream& fi, const string& name, V& v, Conv conv) {
  string s; if (!getline(fi, s)) Die("no line for ", name);
  const vector<string>& a = split1v(s, "\t"); 
  Check(a.size() == 2 && a[0] == name, "name=", name, " a.size()=", a.size(), " a[0]=", a[0]);
  load_vec(a[1], v, conv);
}
template <typename V> void load_vec(istream& fi, const string& name, V& v)
{ stot_fun_t<typename V::value_type> conv; load_vec(fi, name, v, conv);}

template <typename Iter, typename Conv> 
Iter load_vec(const string& s, Iter vb, Iter ve, Conv conv) { 
  Splitter u(s, ",");
  for (Splitter::iterator i = u.begin(); i != u.end(); ++i) {
    Check(vb != ve);
    (*vb) = conv(*i); ++vb;
  }
  return vb;
}
template <typename Iter, typename Conv>
void load_vec(istream& fi, const string& name, Iter vb, Iter ve, Conv conv) {
  string s; if (!getline(fi, s)) Die("no line for ", name);
  const vector<string>& a = split1v(s, "\t");
  Check(a.size() == 2 && a[0] == name, "name=", name, ",a.size()=", a.size(), ",a[0]=\"", a[0], "\"");
  load_vec(a[1], vb, ve, conv);
}
template <typename Iter> void load_vec(istream& fi, const string& name, Iter vb, Iter ve)
{ stot_fun_t<typename iterator_traits<Iter>::value_type> conv; load_vec(fi, name, vb, ve, conv);}
// template <typename V, typename Conv> void load_vec_n(istream& fi, V& v, Conv conv) 
// { string s; v.clear(); while (getline(fi, s)) { if (s.empty()) break; v.push_back(conv(s));}}
// template <typename V> void load_vec_n(istream& fi, V& v) 
// { load_vec_n(fi, v, stot_fun(typename V::value_type()));}
template <typename V> void load_vec_n(istream& fi, V& v) { 
  string s; 
  v.clear(); 
  while (getline(fi, s)) { 
    if (s.empty()) break; 
    v.push_back(stot<typename V::value_type>(s));
  }
}
template <typename V, typename Conv>
void load_vec_n(istream& fi, const string& name, V& v, Conv conv) {
  string s;
  if (!name.empty()) {Check((getline(fi, s) && s == name), "no line for ", name);}
  v.clear(); while (getline(fi, s)) { if (s.empty()) break; v.push_back(conv(s));}
}
template <typename V> void load_vec_n(istream& fi, const string& name, V& v) 
{ load_vec_n(fi, name, v, stot_fun(typename V::value_type()));}

template <typename V>  void save_mtx(ostream& fo, const string& name, const vector<V>& m) {
  save_name(fo, name);
  for (int i = 0; i < (int)m.size(); ++i) { save_vec(fo, m[i]); save_n(fo);}
  save_n(fo);
}
template <typename V>  void load_mtx(istream& fi, const string& name, vector<V>& m) {
  load_name(fi, name);
  m.clear();
  string s; while (getline(fi, s)) {
    if (s.empty()) break;
    m.resize(m.size() + 1);
    load_vec(s, m.back());
  }
}

template <typename V> void save_vec_bytes(ostream& fo, const string& name, const V& v) {
  // v should have continuous data
  save_val(fo, name, v.size());   // ToDo: recognize different endian
  if (v.size() > 0) {
    Check(fo.write((const char*)&v[0], (v.size() * sizeof(typename V::value_type))));
  }
  save_n(fo);
  save_n(fo);
}
template <typename V> void load_vec_bytes(istream& fi, const string& name, V& v) {
  // v should have continuous data
  size_t vsize = 0; // ToDo: recognize different endian
  load_val(fi, name, vsize);
  v.resize(vsize);
  if (v.size() > 0) {
    Check(fi.read((char*)&v[0], (v.size() * sizeof(typename V::value_type))));
  }
  load_n(fi);
  load_n(fi);
}
template <typename T> void save_val_bytes(ostream& fo, const string& name, const T& t) {
  save_name(fo, name);   // ToDo: recognize different endian
  if (sizeof(T) > 0) { Check(fo.write((const char*)&t, sizeof(T)));}
  save_n(fo);
  save_n(fo);
}
template <typename T> void load_val_bytes(istream& fi, const string& name, T& t) {
  load_name(fi, name); // ToDo: recognize different endian
  if (sizeof(T) > 0) { Check(fi.read((char*)&t, sizeof(T)));}
  load_n(fi);
  load_n(fi);
}

template <typename Func> static double numer_diff_1(Func f, double x) {
  const double h = 0.0001;
  return (f(x + 0.5 * h) - f(x - 0.5 * h)) / h;
}
template <typename Func> static double numer_diff_2(Func f, double x) {
  const double h = 0.001;
  return (8.0*(f(x + 0.25*h) - f(x - 0.25*h)) - (f(x + 0.5*h) - f(x - 0.5*h))) / (3.0*h);
}
#define getter(type, name)        type const& name() const {return _##name;}
#define setter(type, name)        void set_##name(type const& value) {_##name = value;}
#define attr_reader(type, name)   type _##name; getter(type, name)
#define attr_writer(type, name)   type _##name; setter(type, name)
#define attr_accessor(type, name) type _##name; getter(type, name) setter(type, name)
#define attr(type, name)          type _##name; getter(type, name) type& name() {return _##name;}

inline string file_basename(const string& s, bool strip_suffix = false) {
  string::size_type i = s.find_last_of("/");  // only apply to unescaped names
  if (i == string::npos && (s == "." || s == "..")) return "";
  i = (i == string::npos ? 0 : (i + 1));
  string::size_type j = (strip_suffix ? s.find_last_of(".") : string::npos);
  return  s.substr(i, (j != string::npos && (i < j)) ? (j - i) : string::npos);
}
inline string file_dirname(const string& s) {
  string::size_type i = s.find_last_of("/");  // only apply to unescaped names
  if (i == string::npos) { return (s == ".." ? ".." : ".");}
  else { return s.substr(0, i);}
}

template <typename Block> int file_each_fasta(const string& fname, Block block) {
  ifstream fi(fname.c_str()); Check(fi, "cannot open file ", fname);
  return each_fasta(fi, block);
}
template <typename Block> int each_fasta(istream& fi, Block block) {
  string buf;
  string name;
  string seq;
  bool   first = true;
  while (getline(fi, buf)) {
    const string& s = strip(buf);
    if (s.empty()) continue;
    switch (s[0]) {
    case '>':
      if (!first) { int st = block(name, seq); if (st) return st;}
      else { first = false;}
      name.assign(s.begin() + 1, s.end());
      seq.clear();
      break;
    default:
      seq += s;
      break;
    }
  }
  if (!first) { int st = block(name, seq); if (st) return st;}
  return 0;
}
template <typename Iter> void print_fasta(ostream& fo, const string& name, Iter b, Iter e) {
  enum { w = 70};
  fo << '>' << name << '\n';
  int col = 0;
  for (; b != e; ++b) {
    if (col == w) {
      col = 0;
      fo << '\n';
    }
    ++col;
    fo << (*b);
  }
  fo << '\n';
}
template <typename V> void print_fasta(ostream& fo, const string& name, const V& seq) 
{ print_fasta(fo, name, seq.begin(), seq.end());}
template <typename Block> int file_each_row(const string& fname, int ncol, Block block) {
  ifstream fi(fname.c_str()); Check(fi, "cannot open file ", fname);
  return each_row(fi, ncol, block);
}
// splat ? by making ncol a template parameter
// can be achieved by wrapping block()
template <typename Block> int each_row(istream& fi, int ncol, Block block) {
  string buf; while (getline(fi, buf)) {
    if (!buf.empty() && buf[0] == '#') continue;
    const vector<string>& a = split1v(buf, "\t"); 
    Check((int)a.size() == ncol, "a.size=", a.size(), ",ncol=", ncol);
    int st = block(a); if (st) return st;
  }
  return 0;
}

template <typename Iter> void copy_circular(Iter src_b, Iter src_e, Iter dst_b, Iter dst_e) {
  Check(src_b != src_e);
  Iter i = src_b;
  while (dst_b != dst_e) {
    if (i == src_e) { i = src_b;}
    *dst_b = *i;
    ++dst_b;
    ++i;
  }
}

template <typename Iter1, typename Iter2> double cor(Iter1 b1, Iter1 e1, Iter2 b2) {
  if (b1 == e1) return 0;
  double sum_sq_x      = 0;
  double sum_sq_y      = 0;
  double sum_coproduct = 0;
  double mean_x        = (*b1++);
  double mean_y        = (*b2++);
  double n             = 1;
  while (b1 != e1) {
    const double& x       = (*b1++);
    const double& y       = (*b2++);
    const double& delta_x = (x - mean_x);
    const double& delta_y = (y - mean_y);
    const double& sweep   = (n / (n + 1));
    ++n;
    sum_sq_x += delta_x * delta_x * sweep;
    sum_sq_y += delta_y * delta_y * sweep;
    sum_coproduct += delta_x * delta_y * sweep;
    mean_x += delta_x / n;
    mean_y += delta_y / n;
  }
  // const double& pop_sd_x = std::sqrt(sum_sq_x / n);
  // const double& pop_sd_y = std::sqrt(sum_sq_y / n);
  // const double& cov_x_y = sum_coproduct / n;
  // return cov_x_y / (pop_sd_x * pop_sd_y);
  const double& sum_sq_x_y = (sum_sq_x * sum_sq_y);
  return ((sum_sq_x_y > 0) ? (sum_coproduct / sqrt(sum_sq_x_y)) : 0);
}
template <typename V1, typename V2> double cor(V1 const& v1, V2 const& v2)
{ return cor(v1.begin(), v1.end(), v2.begin());}
template <typename U, typename V, typename W>
double rank_cor(const U& v1, const U& v2, V& v1_tmp, V& v2_tmp, W& perm) {
  rank_vect(v1_tmp, v1, perm, false, true, 0.0);
  rank_vect(v2_tmp, v2, perm, false, true, 0.0);
  return cor(v1_tmp, v2_tmp);
}

inline double t_value_for_cor(double nsamples, double cor) 
{ return (cor * sqrt((nsamples - 2) / (1 - cor * cor)));} // degrees of freedom nu = (nsamples - 2)

template <typename V1, typename V2>
pair<double, double> regress_one_dim(V1 const& y, V2 const& x) {
  Check(y.size() == x.size());
  typename V2::const_iterator b1 = x.begin();
  typename V2::const_iterator e1 = x.end();
  typename V1::const_iterator b2 = y.begin();
  if (b1 == e1) return make_pair(0.0, 0.0);
  double sum_sq_x      = 0;
  // double sum_sq_y      = 0;
  double sum_coproduct = 0;
  double mean_x        = (*b1++);
  double mean_y        = (*b2++);
  double n = 1;
  while (b1 != e1) {
    ++n;
    const double& sweep   = (n - 1) / n;
    const double& x       = (*b1++);
    const double& y       = (*b2++);
    const double& delta_x = (x - mean_x);
    const double& delta_y = (y - mean_y);
    sum_sq_x += delta_x * delta_x * sweep;
    // sum_sq_y += delta_y * delta_y * sweep;
    sum_coproduct += delta_x * delta_y * sweep;
    mean_x += delta_x / n;
    mean_y += delta_y / n;
  }
  const double& var_x = (sum_sq_x / n);
  // const double& var_y = (sum_sq_y / n);
  const double& cov_x_y = (sum_coproduct / n);
  if (var_x == 0) return make_pair(0.0, mean_y);
  const double& scale_x = (cov_x_y / var_x);
  const double& shift_x = (mean_y - scale_x * mean_x);
  return make_pair(scale_x, shift_x);
}

template <typename Iter, typename Iter1> void vadd(Iter vb, Iter ve, Iter1 ub) 
{ transform(vb, ve, ub, vb, plus<typename iterator_traits<Iter>::value_type>());}
template <typename Iter, typename Iter1> void vsub(Iter vb, Iter ve, Iter1 ub) 
{ transform(vb, ve, ub, vb, minus<typename iterator_traits<Iter>::value_type>());}
template <typename Iter, typename Iter1> void vmul(Iter vb, Iter ve, Iter1 ub) 
{ transform(vb, ve, ub, vb, multiplies<typename iterator_traits<Iter>::value_type>());}
template <typename Iter, typename Iter1> void vdiv(Iter vb, Iter ve, Iter1 ub) 
{ transform(vb, ve, ub, vb, divides<typename iterator_traits<Iter>::value_type>());}
template <typename Iter, typename Iter1> void vinv(Iter vb, Iter ve, Iter1 ub) 
{ transform(vb, ve, ub, vb, bind1st(divides<typename iterator_traits<Iter>::value_type>(), 1));}
template <typename Iter, typename T> void vshift(Iter vb, Iter ve, const T& t) 
{ transform(vb, ve, vb, bind2nd(plus<typename iterator_traits<Iter>::value_type>(), t));}
template <typename Iter, typename T> void vscale(Iter vb, Iter ve, const T& t) 
{ transform(vb, ve, vb, bind2nd(multiplies<typename iterator_traits<Iter>::value_type>(), t));}
template <typename Iter, typename Iter1> 
typename iterator_traits<Iter>::value_type vdot(Iter vb, Iter ve, Iter1 ub) 
{ return inner_product(vb, ve, ub, (typename iterator_traits<Iter>::value_type)0);}
template <typename Iter, typename Iter1>
typename iterator_traits<Iter>::value_type vdif2(Iter vb, Iter ve, Iter1 ub) {
  typename iterator_traits<Iter>::value_type t = 0; 
  while (vb != ve) {
    typename iterator_traits<Iter>::value_type const& t1 = ((*vb) - (*ub)); ++vb; ++ub;
    t += (t1 * t1);
  }
  return t;
}
template <typename Iter> typename iterator_traits<Iter>::value_type vsum_abs(Iter vb, Iter ve) 
{ typename iterator_traits<Iter>::value_type t = 0; for (; vb != ve; ++vb) { t += abs(*vb);}; return t;}
template <typename Iter> typename iterator_traits<Iter>::value_type vsum(Iter vb, Iter ve) 
{ return accumulate(vb, ve, (typename iterator_traits<Iter>::value_type) 0);}
template <typename Iter> typename iterator_traits<Iter>::value_type vmax(Iter vb, Iter ve) 
{ Check(vb != ve); return *max_element(vb, ve);}
template <typename Iter> typename iterator_traits<Iter>::value_type vmin(Iter vb, Iter ve) 
{ Check(vb != ve); return *min_element(vb, ve);}
template <typename Iter> 
pair<typename iterator_traits<Iter>::value_type, typename iterator_traits<Iter>::value_type> 
vminmax(Iter vb, Iter ve) { 
  Check(vb != ve);
  Iter imin = vb;
  Iter imax = vb;
  for(; vb != ve; ++vb) { if (*vb < *imin) { imin = vb;} else if (*imax < *vb) { imax = vb;}}
  return make_pair(*imin, *imax);
}
template <typename Iter1, typename Iter2, typename T>
void vscale_add(Iter1 vb, Iter1 ve, Iter2 ub, T const& t) 
{ while (vb != ve) { (*vb) += t * (*ub); ++vb; ++ub;}}
template <typename Iter, typename Iter1, typename Iter2>
void vmul_add(Iter vb, Iter ve, Iter1 vb1, Iter2 vb2)
{ for (; vb != ve; ++vb) { (*vb) += ((*vb1) * (*vb2)); ++vb1; ++vb2;}}
template <typename Iter1, typename Iter2, typename Iter3>
void vdistribute(Iter1 dst, Iter2 pb, Iter2 pe, Iter3 src) 
{ while (pb != pe) { *(dst + (*pb)) = *src; ++src; ++pb;}}
template <typename Iter1, typename Iter2, typename Iter3>
void vselect(Iter1 dst, Iter3 src, Iter2 pb, Iter2 pe) 
{ while (pb != pe) { *dst = *(src + (*pb)); ++dst; ++pb;}}
template <typename Iter, typename Iter1, typename Block> 
void vcollect(Iter vb, Iter ve, Iter1 ub, Block block) 
{ while (vb != ve) { (*vb) = block(*ub); ++vb; ++ub;}}
template <typename Iter, typename T> 
void vfill(Iter vb, Iter ve, T const& t) { std::fill(vb, ve, t);}
template <typename Iter>
void vshuffle(Iter vb, Iter ve) { Random r; std::random_shuffle(vb, ve, r);}
template <typename Iter, typename Iter1>
void vcopy(Iter b, Iter e, Iter1 b1) { while (b != e) { (*b) = (*b1); ++b; ++b1;}}

template <typename T> T logt(T t) { return std::log(t);}
template <typename T> T expt(T t) { return std::exp(t);}
template <typename Iter> void vlog(Iter vb, Iter ve) 
{ transform(vb, ve, vb, ptr_fun(logt<typename iterator_traits<Iter>::value_type>));}
template <typename Iter> void vexp(Iter vb, Iter ve) 
{ transform(vb, ve, vb, ptr_fun(expt<typename iterator_traits<Iter>::value_type>));}
template <typename Iter, typename T> void vidx(Iter vb, Iter ve, T const& t = 0)
{ T t1 = t; while (vb != ve) { (*vb) = t1; ++vb; ++t1;}}

template <typename Iter, typename Iter1> typename iterator_traits<Iter>::value_type 
vrelerr(Iter vb, Iter ve, Iter1 ub, double eps) { // symmetric
  typedef typename iterator_traits<Iter>::value_type ValT;
  ValT tn = 0; 
  ValT td = eps;
  for (; vb != ve; ++vb, ++ub) {
    const ValT& z1 = (*vb);
    const ValT& z2 = (*ub);
    const ValT& dz = (z1 - z2);
    td += sqr(z1);
    td += sqr(z2);
    tn += sqr(dz);
  }
  return sqrt(tn / td);
}
template <typename Iter, typename Iter1>  typename iterator_traits<Iter>::value_type 
vrelerr1(Iter vb, Iter ve, Iter1 ub, double eps) { // asymmetric
  typedef typename iterator_traits<Iter>::value_type ValT;
  ValT tn = 0; 
  ValT td = eps;
  for (; vb != ve; ++vb, ++ub) {
    const ValT& z1 = (*vb);
    const ValT& z2 = (*ub);
    const ValT& dz = (z1 - z2);
    td += sqr(z1);
    // td += sqr(z2);
    tn += sqr(dz);
  }
  return sqrt(tn / td);
}
template <typename V, typename V1> typename V::value_type vrelerr(V const& v, V1 const& u, double eps) 
{ return vrelerr(v.begin(), v.end(), u.begin(), eps);}
template <typename V, typename V1> typename V::value_type vrelerr1(V const& v, V1 const& u, double eps) 
{ return vrelerr1(v.begin(), v.end(), u.begin(), eps);}

template <typename Iter> typename iterator_traits<Iter>::value_type 
vradius(Iter first, Iter last) { return vdot(first, last, first);}
template <typename V> typename V::value_type
vradius(const V& v) { return vradius(v.begin(), v.end());}
template <typename Iter> typename iterator_traits<Iter>::value_type 
vdist(Iter first, Iter last) { return std::sqrt(vdiff2(first, last));}
template <typename V> typename V::value_type
vdist(const V& v) { return vdist(v.begin(), v.end());}

template <typename V, typename V1> V& vadd(V& v, V1 const& u) 
{ vadd(v.begin(), v.end(), u.begin()); return v;}
template <typename V, typename V1> V& vsub(V& v, V1 const& u) 
{ vsub(v.begin(), v.end(), u.begin()); return v;}
template <typename V, typename V1> V& vmul(V& v, V1 const& u) 
{ vmul(v.begin(), v.end(), u.begin()); return v;}
template <typename V, typename V1> V& vdiv(V& v, V1 const& u) 
{ vdiv(v.begin(), v.end(), u.begin()); return v;}
template <typename V> V& vinv(V& v) { vinv(v.begin(), v.end()); return v;}
template <typename V, typename T> V& vshift(V& v, T const& t) 
{ vshift(v.begin(), v.end(), t); return v;}
template <typename V, typename T> V& vscale(V& v, T const& t) 
{ vscale(v.begin(), v.end(), t); return v;}
template <typename V, typename V1> typename V::value_type vdot(V const& v, V1 const& u) 
{ return vdot(v.begin(), v.end(), u.begin());}
template <typename V, typename V1> typename V::value_type vdif2(V const& v, V1 const& u) 
{ return vdif2(v.begin(), v.end(), u.begin());}
template <typename V> typename V::value_type vsum(V const& v) { return vsum(v.begin(), v.end());}
template <typename V> typename V::value_type vmax(V const& v) { return vmax(v.begin(), v.end());}
template <typename V> typename V::value_type vmin(V const& v) { return vmin(v.begin(), v.end());}
template <typename V> pair<typename V::value_type, typename V::value_type>
vminmax(V const& v) { return vminmax(v.begin(), v.end());}
template <typename V> V& vlog(V& v) { vlog(v.begin(), v.end()); return v;}
template <typename V> V& vexp(V& v) { vexp(v.begin(), v.end()); return v;}
template <typename V, typename T> 
V& vfill(V& v, T const& t) { fill(v.begin(), v.end(), t); return v;}
template <typename V, typename T> 
V& vidx(V& v, T const& t) { vidx(v.begin(), v.end(), t); return v;}
template <typename V> V& vidx(V& v) { return vidx(v, (typename V::value_type) 0);}
template <typename V, typename T> V& vidx(V& v, T const& b, T const& e) 
{ v.resize(e - b); return vidx(v, b);}
template <typename V1, typename V2, typename T>
void vscale_add(V1& v, V2 const& u, T const& t) { vscale_add(v.begin(), v.end(), u.begin(), t);}
template <typename V, typename V1, typename V2>
void vmul_add(V& v, const V1& v1, const V2& v2) { vmul_add(v.begin(), v.end(), v1.begin(), v2.begin());}
template <typename V1, typename Iter>
V1& vappend(V1& v1, Iter b, Iter e) { v1.insert(v1.end(), b, e); return v1;}
template <typename V1, typename V2>
V1& vappend(V1& v1, V2 const& v2) { return vappend(v1, v2.begin(), v2.end());}
template <typename V1, typename V2, typename V3>
V1& vdistribute(V1& dst, V2 const& perm, V3 const& src) 
{ vdistribute(dst.begin(), perm.begin(), perm.end(), src.begin()); return dst;}
template <typename V1, typename V2, typename V3>
V1& vselect(V1& dst, V3 const& src, V2 const& perm) 
{ vselect(dst.begin(), src.begin(), perm.begin(), perm.end()); return dst;}
template <typename V1, typename V2, typename Block> 
V1& vcollect(V1& v, V2 const& u, Block block) 
{ vcollect(v.begin(), v.end(), u.begin(), block); return v;}
template <typename V> V& vshuffle(V& v) { vshuffle(v.begin(), v.end()); return v;}
template <typename Iter1, typename Iter2> bool vcmp(Iter1 b1, Iter1 e1, Iter2 b2, Iter2 e2)
{ return lexicographical_compare(b1, e1, b2, e2);}
template <typename V1, typename V2> bool veql(V1 const& v1, V2 const& v2) 
{ return ((size_t)v1.size() == (size_t)v2.size() && equal(v1.begin(), v1.end(), v2.begin()));}
template <typename Iter> void vreverse(Iter b, Iter e) { std::reverse(b, e);}
template <typename V> void vreverse(V& v) { std::reverse(v.begin(), v.end());}
template <typename V> void vsort(V& v) { std::sort(v.begin(), v.end());}
template <typename V1, typename V2> bool vcmp(V1 const& v1, V2 const& v2)
{ return lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end());}
template <typename V, typename V1>
void vassign(V& v, V1 const& v1) { v.assign(v1.begin(), v1.end());}

template <typename V> void vsort_uniq(V& v) { vsort(v); v.erase(std::unique(v.begin(), v.end()), v.end());}

template <typename Iter> void vjoin(ostream& fo, Iter b, Iter e, const string& delim) 
{ if (b != e) { ttos(fo, *b); ++b;}; for (; b != e; ++b) { ttos(fo, delim); ttos(fo, *b);}}
template <typename V> void vjoin(ostream& fo, const V& v, const string& delim) { join(fo, v.begin(), v.end(), delim);}
template <typename Iter> string vjoin(Iter b, Iter e, const string& delim) { ostringstream os; vjoin(os, b, e, delim); return os.str();}
template <typename V> string vjoin(const V& v, const string& delim) { return vjoin(v.begin(), v.end(), delim);}

template <typename V1, typename Iter>
class SelectIter : public std::iterator<std::input_iterator_tag,
					typename V1::value_type, 
					typename V1::difference_type, 
					typename V1::pointer, 
					typename V1::reference> {
public:
  typedef SelectIter Self;
  SelectIter() : _v(0), _it(), _it_end() {}
  template <typename V2>
  SelectIter(V1* v, V2 const& perm) : _v(v), _it(perm.begin()), _it_end(perm.end()) {}
  template <typename V2>
  SelectIter(V1& v, V2 const& perm) : _v(&v), _it(perm.begin()), _it_end(perm.end()) {}
  SelectIter(V1& v, Iter b, Iter e) : _v(&v), _it(b), _it_end(e) {}
  SelectIter(const Self& other) : _v(other._v), _it(other._it), _it_end(other._it_end) {}
  Self& operator=(const Self& other) {
    _v = other._v;
    _it = other._it;
    _it_end = other._it_end;
    return *this;
  }
  typename V1::value_type& operator*() { return (*_v)[*_it];}
  typename V1::value_type* operator->() { return &operator*();}
  const typename V1::value_type& operator*() const { return (*_v)[*_it];}
  const typename V1::value_type* operator->() const { return &operator*();}
  Self& operator++() { get_next(); return *this;}
  Self operator++(int) { Self tmp(*this); operator++(); return tmp;}
  bool operator==(const Self& it) const { return (is_end() == it.is_end());}
  bool operator!=(const Self& it) const { return !(*this == it);}
private:
  V1*  _v;
  Iter _it;
  Iter _it_end;
  bool is_end() const { return (_it == _it_end);}
  void get_next() { ++_it;}
};

template <typename V1, typename Iter>
class SelectCIter : public std::iterator<std::input_iterator_tag,
					 typename V1::value_type, 
					 typename V1::difference_type, 
					 typename V1::pointer, 
					 typename V1::const_reference> {
public:
  typedef SelectCIter Self;
  SelectCIter() : _v(0), _it(), _it_end() {}
  template <typename V2>
  SelectCIter(const V1& v, V2 const& perm) : _v(&v), _it(perm.begin()), _it_end(perm.end()) {}
  SelectCIter(const V1& v, Iter b, Iter e) : _v(&v), _it(b), _it_end(e) {}
  SelectCIter(const Self& other) : _v(other._v), _it(other._it), _it_end(other._it_end) {}
  Self& operator=(const Self& other) {
    _v = other._v;
    _it = other._it;
    _it_end = other._it_end;
    return *this;
  }
  typename V1::const_reference operator*() const { return (*_v)[*_it];}
  const typename V1::pointer operator->() const { return &operator*();}
  Self& operator++() { get_next(); return *this;}
  Self operator++(int) { Self tmp(*this); operator++(); return tmp;}
  bool operator==(const Self& it) const { return (is_end() == it.is_end());}
  bool operator!=(const Self& it) const { return !(*this == it);}
private:
  const V1*  _v;
  Iter _it;
  Iter _it_end;
  bool is_end() const { return (_it == _it_end);}
  void get_next() { ++_it;}
};

template <typename T> class LineIter0 
  : public std::iterator<input_iterator_tag, string, ptrdiff_t, const string*, const string&> {
public:
  typedef LineIter0 Self;
  typedef long      IdxT;
  LineIter0() : _t(0), _cur(), _idx(-1) {}
  LineIter0(T& t) : _t(&t), _cur(), _idx(-1) { get_first();}
  const string& operator*() const { return _cur;}
  const string* operator->() const { return &operator*();}
  Self& operator++() { get_next(); return *this;}
  Self operator++(int) { Self tmp(*this); operator++(); return tmp;}
  bool operator==(const Self& it) const { return (is_end() == it.is_end());}
  bool operator!=(const Self& it) const { return !(*this == it);}
  void clear() { _t = 0; _cur.clear(); _idx = (-1);}
  void reset(T& t) { _t = &t; _cur.clear(); _idx = (-1); get_first();}
  bool is_end() const { return (_t == 0);}
  IdxT idx() const { return _idx;}
private:
  T*      _t;
  string  _cur;
  IdxT    _idx;
  void get_first() { get_next();}
  void get_next() { ++_idx; if (!_t || !getline(*_t, _cur)) { _t = 0; _cur.clear();}}
};
typedef LineIter0<istream> LineIter;

class RowIter : public std::iterator<std::input_iterator_tag, vector<string>, ptrdiff_t, 
		const vector<string>*, const vector<string>&> {
public:
  typedef RowIter Self;
  RowIter() : _fi(0), _ncol(0), _idx((size_t)(-1)) {}
  RowIter(istream& fi, size_t ncol) : _fi(&fi), _ncol(ncol), _idx((size_t)(-1)) { get_first();}
  reference operator*() { return _cur;}
  pointer operator->() { return &operator*();}
  Self& operator++() { get_next(); return *this;}
  // does not keep stream state. only cur value is kept.
  Self operator++(int) { Self tmp(*this); operator++(); return tmp;}
  bool operator==(const Self& it) const { return (is_end() == it.is_end());}
  bool operator!=(const Self& it) const { return !(*this == it);}
  const string& get(size_t i) const { return _cur[i];}
  const string& at(size_t i) const { return get(i);}
  size_t size() const { return _cur.size();}
  size_t idx() const { return _idx;}
private:
  istream*       _fi;
  size_t         _ncol;
  size_t         _idx;
  string         _buf;
  vector<string> _cur;
  bool is_end() const { return (_fi == 0);}
  void get_first() { get_next();}
  void get_next() { // stops at new line
    _cur.clear();
    if (!_fi || !getline(*_fi, _buf) || _buf.empty()) { _fi = 0; _buf.clear(); _idx = (size_t)(-1); return;}
    split1v(_cur, _buf, "\t"); ++_idx;
    Check((size_t)_cur.size() == _ncol, (size_t)_cur.size(), " != ", _ncol, " line:", _buf);
  }
};

class FastaIter : public std::iterator<std::input_iterator_tag,
		  pair<string, string>, std::ptrdiff_t, 
		  const pair<string, string>*, const pair<string, string>&> {
public:
  typedef FastaIter Self;
  typedef pair<string, string> Data;
  FastaIter() : _fi(0), _data(), _has_next(false) {}
  FastaIter(istream& fi) : _fi(&fi), _data(), _has_next(false) { get_first();}
  reference operator*() { return _data;}
  pointer operator->() { return &operator*();}
  Self& operator++() { get_next(); return *this;}
  // does not keep stream state. only cur value is kept.
  Self operator++(int) { Self tmp(*this); operator++(); return tmp;}
  bool operator==(const Self& it) const { return (is_end() == it.is_end());}
  bool operator!=(const Self& it) const { return !(*this == it);}
  const string& name() const { return _data.first;}
  const string& seq() const { return _data.second;}
private:
  istream*       _fi;
  Data           _data;
  bool           _has_next;
  string         _buf;
  bool is_end() const { return !_has_next;}
  void get_first() {
    _has_next = true;
    _buf.clear();
    get_next();
  }
  void get_next() {
    string& name = _data.first;
    string& seq  = _data.second;
    name.clear();
    seq.clear();
    if (!_buf.empty()) {
      Check(_buf[0] == '>');
      name = _buf.substr(1);
    } else {
      _has_next = false;
      while (getline(*_fi, _buf)) {
	if (!_buf.empty() && _buf[0] == '>') {
	  _has_next = true;
	  name = _buf.substr(1);
	  break;
	}
      }
    }
    _buf.clear();
    if (!_has_next) return;
    while (getline(*_fi, _buf)) {
      if (_buf.empty()) { continue;}
      else if (_buf[0] == '>') { break;}
      else { seq += _buf; _buf.clear();}
    }
  }
};

template <typename T, typename Conv> class output_iterator 
  : public std::iterator<std::output_iterator_tag, void, void, void, void> {
public:
  output_iterator(ostream& fo, Conv conv) : _fo(fo), _conv(conv) {}
  output_iterator& operator*() { return *this;}
  output_iterator& operator++() { return *this;}
  output_iterator& operator++(int) { return *this;}
  output_iterator& operator=(const T& t) { _fo << _conv(t); return *this;}
private:
  ostream& _fo;
  Conv     _conv;
};

class CVIter : public std::iterator<std::input_iterator_tag, 
	       pair< vector<int>, vector<int> >, std::ptrdiff_t, 
	       const pair< vector<int>, vector<int> >*, 
	       const pair< vector<int>, vector<int> >&> {
public:
  typedef CVIter Self;
  CVIter() : _rem(-1) {}
  CVIter(int n, int m) { reset(n, m);}
  template <typename Iter> CVIter(Iter b, Iter e, int m) { reset(b, e, m);}
  template <typename T> CVIter(const vector<T>& perm, int m) { reset(perm, m);}
  reference operator*() { return _cur;}
  pointer operator->() { return &operator*();}
  Self& operator++() { get_next(); return *this;}
  // does not keep stream state. only cur value is kept.
  Self operator++(int) { Self tmp(*this); operator++(); return tmp;}
  bool operator==(const Self& it) const { return (is_end() == it.is_end());}
  bool operator!=(const Self& it) const { return !(*this == it);}
  void reset(int n, int m) { set_perm(n); reset0(n, m);}
  template <typename Iter> void reset(Iter b, Iter e, int m) 
  { set_perm(b, e); reset0(_perm.size(), m);}
  template <typename T> void reset(const vector<T>& v, int m) { reset(v.begin(), v.end(), m);}
  void rewind() { _rem = _bucket.number_of_non_empty_buckets(); get_next();}
  const vector<int>& trainset() const { return _cur.first;}
  const vector<int>& testset() const { return _cur.second;}
  int trainset_size() const { return _cur.first.size();}
  int testset_size() const { return _cur.second.size();}
  int trainset(int i) const { return _cur.first[i];}
  int testset(int i) const { return _cur.second[i];}
private:
  Bucket      _bucket;
  vector<int> _perm;
  int         _rem;
  value_type  _cur;
  void reset0(int n, int m) { _bucket.reset(n, m); rewind();}
  void set_perm(int n) {
    _perm.resize(n);
    generate(_perm.begin(), _perm.end(), series<int>(0));
    random_shuffle(_perm.begin(), _perm.end());
  }
  template <typename Iter> void set_perm(Iter b, Iter e) {
    _perm.assign(b, e);
    random_shuffle(_perm.begin(), _perm.end());
  }
  bool is_end() const { return (_rem == (-1));}
  void get_next() {
    _cur.first.clear();
    _cur.second.clear();
    if (_rem <= 0) { _rem = (-1); return;}
    --_rem;
    int b = _bucket.item_begin(_rem);
    int e = _bucket.item_end(_rem);
    _cur.first.assign(_perm.begin(), _perm.begin() + b);
    _cur.first.insert(_cur.first.end(), _perm.begin() + e, _perm.end());
    _cur.second.assign(_perm.begin() + b, _perm.begin() + e);
  }
};
template <typename T, typename Conv> output_iterator<T, Conv> 
oiter_fun(ostream& fo, const T& t, Conv conv) { return output_iterator<T, Conv>(fo, conv);}  

// avoid temporal name
#define ifstream_(fi, name) ifstream fi((name).c_str()); Check(fi, "cannot open file: " #name "=", (name));
#define ofstream_(fo, name) ofstream fo((name).c_str()); Check(fo, "cannot open file: " #name "=", (name));
template <typename F> void file_open(F& f, const string& s) 
{ f.open(s.c_str()); Check(f, "file_open: cannot open file=", s);}

inline double GAS_CONST_KCAL_MOL_K() { return 0.0019872;}   // (1.9872 * 0.001)
inline double TEMPERATURE_K() { return 310.15;}             // (273.15 + 37.0)        
inline double RT_KCAL_MOL() { return 0.61633008;}           // GAS_CONST_KCAL_MOL_K * TEMPERATURE_K
inline double INV_RT_KCAL_MOL() { return 1.62250721236906;} // 1.0 / RT_KCAL_MOL
inline double score_to_energy(double score) { return (-score * RT_KCAL_MOL());}
inline double energy_to_score(double energy) { return (-energy * INV_RT_KCAL_MOL());}
template <typename T> T sqr(T const& t) { return (t * t);}


// not efficient
template <typename V, typename V1> void permute_inplace(V& v, V1 const& perm) {
  for (int i = 0; i < (int)v.size(); ++i) {
    for (int j = perm[i];; j = perm[j]) {
      if (j < i) continue;
      if (j = i) break;
      swap(v[i], v[j]);
      break;
    }
  }
}
template <unsigned long max_value> struct BitSize
{ enum { value = (1 + BitSize<(max_value >> 1)>::value)};};
template <> struct BitSize<0> { enum { value = 0};};

inline bool save_gr_diff(ostream& fo, const vector<double>& gr, const vector<double>& gr1, 
			 double tol) {
  for (int i = 0; i < (int)gr.size(); ++i) {
    double d0 = tol * (1.0 + abs(gr[i]) + abs(gr1[i]));
    double d = abs(gr[i] - gr1[i]);
    if (isfinite(gr[i]) && isfinite(gr1[i]) && d < d0) {
      fo << i << ": gr:" << gr[i] << " == gr1:" << gr1[i]
	 << " dif:" << (gr[i] - gr1[i]) << endl;
    } else {
      save_vec(fo, "gr ", gr);
      save_vec(fo, "gr1", gr1);
      fo << "i:" << i << " gr:" << gr[i] << " != gr1:" << gr1[i] 
	 << " dif:" << (gr[i] - gr1[i]) << endl;
      return false;
    }
  }
  fo << "gr and gr1 are the same" << endl;
  return true;
}
template <typename Iter> bool check_pos_tot(ostream& fo, Iter b, Iter e, double tot, double tol) {
  double z = 0;
  for (Iter i = b; i != e; ++i) {
    if (*i < 0) { 
      fo << "negative val: v[" << (i - b) << "]=" << (*i) << '\n'; 
      save_vec(fo, "v", vector<double>(b, e)); 
      return false;
    }
    z += (*i);
  }
  if (abs(z - tot) > tol) { 
    fo << "unnormalized: sum:" << z << ",tot:" << tot << ",diff:" << (z - tot) << endl;
    save_vec(fo, "v", vector<double>(b, e));
    return false;
  } else {
    fo << "normalized sum:" << z << ",tot:" << tot << ",diff:" << (z - tot) << endl;
  }
  return true;
}
class DerivIter : public std::iterator<std::input_iterator_tag,
					double, 
					std::ptrdiff_t, 
					const double*, 
					const double&> {
public:
  typedef DerivIter Self;
  typedef vector<double> V;
  DerivIter() : _pv(0), _nxt(-1), _eps(0) {}
  DerivIter(const V& v, double eps) : _pv(&v), _nxt(-1), _eps(eps) { get_first();}
  const double& operator*() { return pv(_nxt - 1);}
  const double* operator->() { return &operator*();}
  Self& operator++() { get_next(); return *this;}
  Self operator++(int) { Self tmp(*this); operator++(); return tmp;}
  bool operator==(const Self& it) const { return (is_end() == it.is_end());}
  bool operator!=(const Self& it) const { return !(*this == it);}
  const V& pv() const { return (*_pv);}
  V& pv1() { return _pv1;}
  V& pv2() { return _pv2;}
  const double& eps() { return _eps;}
  const double& pv(int i) const { return (*_pv)[i];}
  double& pv1(int i) { return _pv1[i];}
  double& pv2(int i) { return _pv2[i];}
  int idx() const { return (_nxt - 1);}
  operator unsigned int() const { return (unsigned int)idx();}
private:
  const V* _pv;
  int      _nxt;
  double   _eps;
  V        _pv1;
  V        _pv2;
  bool is_end() const { return (_nxt < 0);}
  void get_first() {
    _nxt = 0;
    get_next();
  }
  void get_next() {
    if (_nxt < 0 || (int)_pv->size() <= _nxt) { _nxt = (-1); return;}
    _pv1 = pv();
    _pv1[_nxt] = (pv(_nxt) + 0.5 * _eps);
    _pv2 = pv();
    _pv2[_nxt] = (pv(_nxt) - 0.5 * _eps);
    ++_nxt;
  }
};

// df/d_prob_i => df/d_ln_prob_i
// dpb = (df/d_prob_i), pb = (prob_i)
// dpb_i <- prob_i * { df/d_prob_i - sum_j(prob_j * df/d_prob_j)}
template <typename Iter, typename Iter1> void dprob_to_dlprob(Iter dpb, Iter dpe, Iter1 pb) 
{ vmul(dpb, dpe, pb); vscale_add(dpb, dpe, pb, -vsum(dpb, dpe));}

// df/dp => df/d_ln_p = p * df/dp
template <typename T> T dparam_to_dlparam(const T& dp, const T& p) { return p * dp;}

template <typename Iter> double vprob_norm(Iter b, Iter e) // should be a forward iterator
{ const double& z = vsum(b, e); vscale(b, e, (z > 0 ? (1.0 / z) : 0.0)); return z;}
template <typename V> double vprob_norm(V& v) { return vprob_norm(v.begin(), v.end());}
inline void _logadd_(double& z, const double& z1) {
  if (z1 == (-HUGE_VAL)) return;
  const double& d = (z - z1);
  if (0 <= d) { z += log1p(std::exp(-d));} else { z = z1 + log1p(std::exp(d));}
}
template <typename Iter> typename iterator_traits<Iter>::value_type vlsum(Iter b, Iter e) 
{ double z = (-HUGE_VAL); for (; b != e; ++b) { _logadd_(z, *b);}; return z;}
template <typename V> typename V::value_type vlsum(const V& v) { return vlsum(v.begin(), v.end());}
template <typename Iter> double vlprob_norm(Iter b, Iter e) 
{ const double& z = vlsum(b, e); vshift(b, e, ((z == (-HUGE_VAL)) ? (-HUGE_VAL) : (-z))); return z;}
template <typename V> double vlprob_norm(V& v) { return vlprob_norm(v.begin(), v.end());}

class Timer {
public:
  typedef pair<string, clock_t> Node;
  vector<Node> _v;
  Timer() {}
  Timer(const string& name) { push(name);}
  bool empty() const { return _v.empty();}
  size_t size() const { return _v.size();}
  Timer& operator<<(const string& name) { push(name); return *this;}
  Timer& operator>>(ostream& fo) { pop(fo); return *this;}
  void push(const string& name) { _v.push_back(make_pair(name, clock()));}
  void back(ostream& fo) {
    double dt;
    back(dt);
    fo << (_v.size() - 1) << "," << _v.back().first << "," << dt << " sec" << endl;
  }
  void back(double& dt_sec) {
    Check(!_v.empty());
    const Node& n = _v.back();
    dt_sec = (((double)clock() - (double)n.second) / (double)CLOCKS_PER_SEC);
  }
  void pop(double& dt_sec) { back(dt_sec); _v.pop_back();}
  void pop(ostream& fo) { back(fo); _v.pop_back();}
  string pop() { ostringstream os; pop(os); return os.str();}
  string back() { ostringstream os; back(os); return os.str();}
};

template <typename T, typename T1>
void transpose(vector< vector<T> >& m, const vector< vector<T1> >& m1) {
  if (m1.empty()) { m.clear(); return;}
  m.resize(m1[0].size());
  for (int j = 0; j < (int)m.size(); ++j) { m[j].resize(m1.size());}
  for (int i = 0; i < (int)m1.size(); ++i) {
    Check(m1[i].size() == m.size());
    for (int j = 0; j < (int)m.size(); ++j) { m[j][i] = m1[i][j];}
  }	
}

template <typename Iter, typename Comp> bool is_sorted(Iter b, Iter e, Comp comp) { // use copy
  if (b == e) return true;
  typename std::iterator_traits<Iter>::value_type t = *b;
  for (++b; b != e; ++b) {
    typename std::iterator_traits<Iter>::value_type t1 = *b;
    if (comp(t1, t)) return false;
    t = t1;
  }
  return true;
}
template <typename Iter> bool is_sorted(Iter b, Iter e) 
{ return is_sorted(b, e, std::less<typename std::iterator_traits<Iter>::value_type>());}

template <typename T> 
class ShiftIter : public std::iterator<output_iterator_tag, void, void, void, void> {
public:
  typedef ShiftIter Self;
  explicit ShiftIter(T& t) : _t(&t) {}
  template <typename U> Self& operator=(const U& u) { (*_t) << u; return *this;}
  Self& operator*() { return *this;}
  Self& operator++() { return *this;}
  Self&  operator++(int) { return *this;}
private:
  T* _t;
};

inline size_t file_size0(const string& fname) // c++ version
{ ifstream_(fi, fname); fi.seekg(0, std::ios::end); return fi.tellg();}

inline size_t file_size(const string& fname) // if symlink, target is searched for.
{ struct stat st; Check(!stat(fname.c_str(), &st)); return st.st_size;}

inline void file_resize(const string& fname, size_t n) { 
  const string& s = (string(" truncate -s ") + ttos(n) + " "); 
  Check(!system(s.c_str()), s);
}

// file may be directory. if fname is a symlink, its target is searched for.
inline bool file_exists(const string& fname) { struct stat st; return !stat(fname.c_str(), &st);}

inline void file_touch(const string& fname) {
  const string& cmd_touch = " touch ";
  const string& s = (cmd_touch + fname);
  Check(!system(s.c_str()), s);
}
inline void file_mv(const string& src, const string& dst) {
  const string& cmd_mv = " mv -f ";
  const string& s = (cmd_mv + src + " " + dst);
  Check(!system(s.c_str()), s);
}
inline void file_rm(const string& fname) {
  const string& cmd_rm = " rm -f ";
  const string& s = (cmd_rm + fname);
  Check(!system(s.c_str()), s);
}
inline void file_rm_rf(const string& fname) {
  const string& cmd_rm = " rm -rf ";
  const string& s = (cmd_rm + fname);
  Check(!system(s.c_str()), s);
}
inline void file_scp(const string& src, const string& dst) {
  const string& cmd_scp = " scp -B -C -q ";
  const string& s = (cmd_scp + src + " " + dst); 
  Check(!system(s.c_str()), s);
}
inline void file_cp(const string& src, const string& dst) {
  const string& cmd_cp = " cp -f ";
  const string& s = (cmd_cp + src + " " + dst); 
  Check(!system(s.c_str()), s);
}
inline void file_mkdir(const string& dname) {
  const string& cmd_mkdir = " mkdir -p ";
  const string& s = (cmd_mkdir + dname);
  Check(!system(s.c_str()), s);
}
inline void copy_stream(ostream& fo, istream& fi) { fo << fi.rdbuf();}

inline bool getline_s(const string& s, string::size_type& pos, string& buf) {
  if (pos == string::npos || s.size() <= pos) { pos = string::npos; return false;}
  const string::size_type& pos1 = s.find_first_of('\n', pos);
  if (pos1 == string::npos) {
    buf.assign(s.begin() + pos, s.end());
    pos = string::npos;
  } else {
    buf.assign(s.begin() + pos, s.begin() + pos1); // does not contain '\n'
    pos = (pos1 + 1);
  }
  return true;
}
template <typename Iter> bool getline_v(Iter& b, Iter e, string& buf) {
  if (b == e) return false;
  Iter b1 = find(b, e, '\n');
  if (b1 == e) {
    buf.assign(b, e);
    b = e;
  } else {
    buf.assign(b, b1); // does not contain '\n'
    b = b1;
    ++b;
  }
  return true;
}
inline bool getline(ifstream& fi, vector<char>& buf) {// this function needs test
  enum { BUF_SIZE = 2000};
  if (!fi) return false;
  buf.clear();
  size_t n = 0;
  while (fi) {
    buf.resize(n + BUF_SIZE);
    fi.get((char*)&buf[n], BUF_SIZE);
    size_t m = fi.gcount();
    n += m;
    buf.resize(n);
    if (fi.peek() == '\n') { fi.get(); break;}
  }
  return true;
}

template <typename Iter1, typename Iter2> // both are assumed sorted and elements are comparable
Iter1 find_first_of_1(Iter1 b1, Iter1 e1, Iter2 b2, Iter2 e2) {
  for (; b1 != e1; ++b1) {
    for (; b2 != e2; ++b2) {
      if (*b1 < *b2) break;
      if (*b2 < *b1) continue;
      return b1;
    }
    if (b2 == e2) break;
  }
  return e1;
}

#include <zlib.h>
inline bool getline_gz(gzFile fi, string& s) {
  enum { BUF_SIZE = 1024};   // macro BUFSIZ exists somewhere.
  static char buf[BUF_SIZE]; // not thread safe
  s.clear(); // cannot use s as buffer, since continuous memory is not guaranteed
  ptrdiff_t n = 0;
  z_off_t off = gztell(fi);
  while (gzgets(fi, buf, BUF_SIZE) != Z_NULL) {
    n = (gztell(fi) - off); Check(n > 0);
    s.append((const char*)buf, (const char*)buf + n);
    if (buf[n - 1] == '\n') { s.resize(s.size() - 1); break;}
    off += n;
  }
  return (n > 0);
}
inline bool getline_gz(gzFile fi, vector<char>& s) {
  enum { BUF_SIZE = 1024 };   // macro BUFSIZ exists somewhere.
  ptrdiff_t n   = 0;
  z_off_t   off = gztell(fi);
  size_t    pos = 0;
  s.resize(pos + BUF_SIZE);
  while (gzgets(fi, &s[pos], BUF_SIZE) != Z_NULL) {
    n = (gztell(fi) - off); Check(n > 0);
    if (s[pos + (n - 1)] == '\n') {
      pos += (n - 1);
      s.resize(pos + BUF_SIZE); 
      break;
    }
    off += n;
    pos += n;
    s.resize(pos + BUF_SIZE);
  }
  s.resize(s.size() - BUF_SIZE);
  return (n > 0);
}

template <typename T> T cyclic_index(T i, size_t n)
{ return (i >= 0 ? (i % n) : ((n - 1) - ((- i - 1) % n)));}


class IdPool {
public:
  typedef long         IntT;
  typedef vector<IntT> V;
  V _pool;
  IdPool(IntT start = 0) { reset(start);}
  void reset(IntT start = 0) { _pool.assign(1, start);}
  IntT create_id() {
    IntT id = 0;
    if (_pool.size() == 1) {
      id = _pool[0];
      ++_pool[0];
    } else {
      id = _pool.back();
      _pool.pop_back();
    }
    return id;
  }
  void destroy_id(IntT id) { _pool.push_back(id);}
  void save(ostream& fo) { save_vec(fo, "pool", _pool);}
  void load(istream& fi) { load_vec(fi, "pool", _pool);}
};

template <typename V> bool rank_vect_comp_asc(const V& v, int i, int j) { return v[i] < v[j];}
template <typename V> bool rank_vect_comp_desc(const V& v, int i, int j) { return v[i] > v[j];}
template <typename W, typename V> void sort_vect_index(W& w, V& v, bool asc = true) { // w[0] is rank 0 index etc
  vidx(w, (typename W::value_type)0, (typename W::value_type)v.size());               // be careful. not rank vect
  if (asc) { std::sort(w.begin(), w.end(), proc_l(rank_vect_comp_asc<V>, v));}
  else { std::sort(w.begin(), w.end(), proc_l(rank_vect_comp_desc<V>, v));}
}
template <typename U, typename V, typename W> // return zero based rank. (<- be careful for testing)
bool rank_vect(U& u, const V& v, W& w, bool no_tie, bool asc, double eps) {
  sort_vect_index(w, v);
  u.resize(w.size()); // U = vector<real> if no_tie = true and has_ties
  bool ties = false;
  if (no_tie) { 
    for (int i = 0; i < (int)u.size(); ++i) { u[w[i]] = i;}
  } else {
    int b = 0;
    int e = 0;
    for (; b < (int)u.size();) {
      e = (b + 1);
      for (; e < (int)u.size(); ++e) {
	if (asc) { if ((v[w[b]] + eps) < v[w[e]]) break;}
	else { if ((v[w[b]] - eps) > v[w[e]]) break;}
      }
      if (!ties && (e - b) > 1) { ties = true;}
      double z = ((b + (e - 1)) / 2.0); // zero based rank (0...v.size())
      for (; b < e; ++b) { u[w[b]] = z;}
    }
  }
  return ties;
}
template <typename U, typename V> // return zero based rank. (<- be careful for testing)
bool rank_vect(U& u, const V& v, bool no_tie, bool asc, double eps) 
{ vector<int> w; return rank_vect(u, v, w, no_tie, asc, eps);}
template <typename Categories, typename Scores>
std::pair<double, double> compute_auc2(const Categories& cat, const Scores& sc) {
  double zp = 0; // positive correlation
  double zn = 0; // negative correlation
  double m  = 0;
  for (int i = 0; i < (int)cat.size(); ++i) {
    if (cat[i] <= 0) continue; // cat[i] = 0 or 1
    ++m;
    for (int j = 0; j < (int)cat.size(); ++j) {
      if (cat[j] > 0) continue;
      if (sc[i] > sc[j]) { ++zp;}
      if (sc[i] < sc[j]) { ++zn;}
    }
  }
  if (m <= 0 || (double)cat.size() <= m) return make_pair(0, 0);
  return make_pair(zp / (m * (cat.size() - m)), (-zn / (m * (cat.size() - m))));
}
template <typename Categories, typename Scores>
double compute_auc(const Categories& cat, const Scores& sc) { return compute_auc2(cat, sc).first;}

template <typename V> bool compute_ks2_comp_desc(const V& v, int i, int j) { return (v[i] > v[j]);}
template <typename Categories, typename Scores, typename Perm>
std::pair<double, double> compute_ks2(const Categories& cat, Scores& sc, Perm& w) {
  vidx(w, (int)0, (int)sc.size()); 
  std::sort(w.begin(), w.end(), proc_l(compute_ks2_comp_desc<Scores>, sc));// descending ord 
  double eps  = 0.0;
  double np   = vsum(cat); // cat[i] = 0 or 1
  double nn   = (cat.size() - np);
  double fp   = (1.0 / np);
  double fn   = (1.0 / nn);
  double ksp  = 0.0;
  double ksn  = 0.0;
  double cumu = 0.0;
  int b = 0;
  int e = 0;
  for (; b < (int)w.size();) {
    for (e = (b + 1); e < (int)w.size(); ++e) { if ((sc[w[b]] - eps) > sc[w[e]]) break;}
    for (; b < e; ++b) { cumu += (cat[w[b]] > 0 ? fp : (-fn));}
    if (ksp < cumu) { ksp = cumu;}
    if (cumu < ksn) { ksn = cumu;}
  }
  return make_pair(ksp, ksn);
}
template <typename Categories, typename Scores>
std::pair<double, double> compute_ks2(const Categories& cat, Scores& sc) 
{ vector<int> w; return compute_ks2(cat, sc, w);}

template <typename T> 
class SegIter : public std::iterator<input_iterator_tag, pair<T,T>, ptrdiff_t, 
				     const pair<T,T>*, const pair<T,T>&> {
public:
  typedef SegIter   Self;
  typedef pair<T,T> Seg;
  SegIter() : _seg(), _e(), _w(), _i() {}
  SegIter(const T& b, const T& e, const T& w) : _seg(b, e), _e(e), _w(w), _i(0) { get_first();}
  const Seg& operator*() { return _seg;}
  const Seg* operator->() { return &operator*();}
  Self& operator++() { get_next(); return *this;}
  Self operator++(int) { Self tmp(*this); operator++(); return tmp;}
  bool operator==(const Self& it) const { return (is_end() == it.is_end());}
  bool operator!=(const Self& it) const { return !(*this == it);}
  T b() const { return _seg.first;}
  T e() const { return _seg.second;}
  T i() const { return _i;}
private:
  Seg _seg;
  T   _e;
  T   _w;
  T   _i;
  bool is_end() const { return (_e <= _seg.first);}
  void get_first() { _seg.second = min(_seg.first + _w, _e);}
  void get_next() { ++_i; _seg.first = _seg.second; _seg.second = min(_seg.first + _w, _e);}
};

template <typename T> 
class IdxIter : public std::iterator<input_iterator_tag, T, ptrdiff_t, const T*, const T&> {
public:
  typedef IdxIter Self;
  IdxIter() : _b(), _e() {}
  IdxIter(const T& b, const T& e) : _b(b), _e(e) { get_first();}
  const T& operator*() { return _b;}
  const T* operator->() { return &operator*();}
  Self& operator++() { get_next(); return *this;}
  Self operator++(int) { Self tmp(*this); operator++(); return tmp;}
  bool operator==(const Self& it) const { return (is_end() == it.is_end());}
  bool operator!=(const Self& it) const { return !(*this == it);}
private:
  T _b;
  T _e;
  bool is_end() const { return (_b == _e);}
  void get_first() {}
  void get_next() { ++_b;}
};

template <typename T> // does not work for negative numbers. more efficient algorithms exists.
T bit_count(T n) { T c = 0; while (n) { n &= (n - 1); ++c;}; return c;}

inline void create_argc_argv(int& argc, char**& argv, const string& command) {
  typedef char* char_ptr;
  const vector<string>& args = splitv(command, " "); // escape is not considered
  argc = args.size();
  argv = new char_ptr[argc + 2];
  if (argc > 0) {
    int n = 0;
    for (int i = 0; i < argc; ++i) { n += (args[i].size() + 1);}
    char_ptr p = new char[n];
    argv[argc] = p; // store memory position
    for (int i = 0; i < argc; ++i) {
      std::copy(args[i].begin(), args[i].end(), p);
      p[args[i].size()] = '\0';
      argv[i] = p;
      p += (args[i].size() + 1);
    }
    argv[argc + 1] = 0;
  } else {
    argv[argc] = argv[argc + 1] = 0;
  }
}
inline void destroy_argc_argv(int& argc, char**& argv) { 
  // edit sachica or gflags, modifies the memory of the last entry.
  // if some of argv[i] is set to zero then segfault will occur
  char* p = 0; for (int i = 0; argv[i] != 0; ++i) { p = argv[i];}
  if (p) { delete[] p;}
  delete[] argv;
  argc = 0;
  argv = 0;
}

inline double compute_mutual_info(int n, const vector<double>& occ) {
  vector<double> p = occ;
  vector<double> p1(n, 0);
  vector<double> p2(n, 0);
  vprob_norm(p);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      double pij = p[i * n + j];
      p1[i] += pij;
      p2[j] += pij;
    }
  }
  double z = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      double pij = p[i * n + j];
      double pi  = p1[i];
      double pj  = p2[j];
      if (pij <= 0) continue;
      z += pij * log(pij / (pi * pj));
    }
  }
  return z;
}

inline double adjust_bonferroni(double p, double ntest) { return (p * ntest);}
inline void adjust_fdr_qvals(vector<double>& vp, double ntest = (-1)) {  // vp can be truncated. then ntest should be set
  // assuming pvalues are independent // is this correct ? check! 
  // always pval <= qval so pval threshold truncation can be used as qval threshold truncation
  if (ntest < 0) { ntest = vp.size();}
  vector<int> u; sort_vect_index(u, vp, true);
  for (int i = ((int)u.size() - 2); i >= 0; --i) { // ok since vp[u[-1]] has no change
    vp[u[i]] *= (ntest / (double)(i + 1));
    if (vp[u[i + 1]] < vp[u[i]]) { vp[u[i]] = vp[u[i + 1]];}
  }
}

template <typename T> void mid_from_range(T& m, T& w, const T& b, const T& e, bool fwd) { // e: exclusive end T: integer type
  if (fwd) {
    w = (e - b);        // w = 0 1 2 3 4 5 6
    m = (b + (w / 2));  // m = 0 0 1 1 2 2 3
  } else {
    w = (e - b);                           // w = 0 1 2 3 4 5 6
    m = (b + (w > 0 ? ((w - 1) / 2) : 0)); // m = 0 0 0 1 1 2 2
  }
}
template <typename T> void mid_to_range(const T& m, const T& w, T& b, T& e, bool fwd) { // e: exclusive end T: integer type
  if (fwd) {
    b = (m - (w / 2));       // w     = 0     1      2      3     4      5
    e = (m + ((w + 1) / 2)); // [b,e] = [0,0] [0,1] [-1,1] [-1,2] [-2,2] [-2,3]
  } else {
    b = (m - (w > 0 ? ((w - 1) / 2) : 0)); // w     = 0     1      2      3     4      5     6
    e = (m + (w > 0 ? (1 + (w / 2)) : 0)); // [b,e] = [0,0] [0,1] [0,2] [-1,2] [-1,3] [-2,3] [-2,4]
  }
}
template <typename T> void width_to_margin_lr(const T& w, T& l, T& r, bool fwd) {
  long mid = 0; // signed type
  long wid = w;
  long b = 0;
  long e = 0;
  mid_to_range(mid, wid, b, e, fwd);
  l = (-b);
  r = max<long>(0, (e - 1));
}

class OptIter : public std::iterator<std::input_iterator_tag, pair<string, string>, ptrdiff_t, 
				     const pair<string, string>*, const pair<string, string>& > {
public:
  typedef OptIter              Self;
  typedef pair<string, string> Opt;
  typedef vector<Opt>          Opts;
  OptIter() : _cur(-1), _opts() {}
  OptIter(int argc, char** argv) : _cur(-1), _opts() { get_first(argc, argv);}
  reference operator*() { return _opts[_cur];}
  pointer operator->() { return &operator*();}
  Self& operator++() { get_next(); return *this;}
  Self operator++(int) { Self tmp(*this); operator++(); return tmp;}
  bool operator==(const Self& it) const { return (is_end() == it.is_end());}
  bool operator!=(const Self& it) const { return !(*this == it);}
  const string& name() const { return _opts[_cur].first;}
  const string& value() const { return _opts[_cur].second;}
private:
  int  _cur;
  Opts _opts;
  bool is_end() const { return (_cur < 0);}
  void get_first(int argc, char** argv) {
    _cur = (-1);
    _opts.clear(); 
    if (argc <= 1) return;
    for (int i = 1; i < argc; ++i) { // only allow -name=value
      string s(argv[i]);
      if (!(s.size() > 0 && s[0] == '-')) { Die("bad option: ", s);} 
      string::size_type k = s.find_first_of("=");
      if (k == string::npos) { Die("bad option: ", s);}
      string name(s.begin() + 1, s.begin() + k);
      string value(s.begin() + (k + 1), s.end());
      _opts.push_back(make_pair(name, value));
    }
    _cur = 0;
  }
  void get_next() { // stops at new line
    if (is_end()) return;
    ++_cur;
    if ((int)_opts.size() <= _cur) { _cur = (-1);}
  }
};

string float_to_name(double z) {
  string s = ttos(z);
  for (int i = 0; i < (int)s.size(); ++i) {
    switch (s[i]) {
    case '.': s[i] = 'd'; break;
    case '+': s[i] = 'p'; break;
    case '-': s[i] = 'm'; break;
    default: break;
    }
  }
  return s;
}
double float_from_name(string s) {
 for (int i = 0; i < (int)s.size(); ++i) {
   switch (s[i]) {
   case 'd': s[i] = '.'; break;
   case 'p': s[i] = '+'; break;
   case 'm': s[i] = '-'; break;
   default: break;
   }
 }
 return stot<double>(s);
}
#endif
