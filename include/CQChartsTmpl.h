#ifndef CQChartsTmpl_H
#define CQChartsTmpl_H

// implement <, <=, >, >=, == and != using cmp function
//
// cmp must be implemented as:
//   friend int cmp(const T &lhs, const T &rhs) { ... }
template<typename T>
class CQChartsComparatorBase {
 public:
  friend bool operator< (const CQChartsComparatorBase &lhs, const CQChartsComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) <  0; }
  friend bool operator<=(const CQChartsComparatorBase &lhs, const CQChartsComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) <= 0; }
  friend bool operator> (const CQChartsComparatorBase &lhs, const CQChartsComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) >  0; }
  friend bool operator>=(const CQChartsComparatorBase &lhs, const CQChartsComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) >= 0; }
  friend bool operator==(const CQChartsComparatorBase &lhs, const CQChartsComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) == 0; }
  friend bool operator!=(const CQChartsComparatorBase &lhs, const CQChartsComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) != 0; }
};

// implement != using == function
template<typename T>
class CQChartsEqBase {
 public:
  friend bool operator!=(const CQChartsEqBase &lhs, const CQChartsEqBase &rhs) {
    return ! operator==(static_cast<const T &>(lhs), static_cast<const T &>(rhs)); }
};

#include <iostream>

// implement << using print function
template<typename T>
class CQChartsPrintBase {
 public:
  friend std::ostream &operator<<(std::ostream &os, const CQChartsPrintBase &base) {
    static_cast<const T &>(base).print(os);
    return os;
  }
};

// implement << and print using toString function
template<typename T>
class CQChartsToStringBase {
 public:
  friend std::ostream &operator<<(std::ostream &os, const CQChartsToStringBase &base) {
    static_cast<const T &>(base).print(os);
    return os;
  }

  void print(std::ostream &os) const {
    os << static_cast<const T *>(this)->toString().toStdString();
  }
};

#endif
