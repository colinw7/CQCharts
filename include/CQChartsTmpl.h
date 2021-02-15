#ifndef CQChartsTmpl_H
#define CQChartsTmpl_H

template<typename T>
class ComparatorBase {
 public:
  friend bool operator< (const ComparatorBase &lhs, const ComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) <  0; }
  friend bool operator<=(const ComparatorBase &lhs, const ComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) <= 0; }
  friend bool operator> (const ComparatorBase &lhs, const ComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) >  0; }
  friend bool operator>=(const ComparatorBase &lhs, const ComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) >= 0; }
  friend bool operator==(const ComparatorBase &lhs, const ComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) == 0; }
  friend bool operator!=(const ComparatorBase &lhs, const ComparatorBase &rhs) {
    return cmp(static_cast<const T &>(lhs), static_cast<const T &>(rhs)) != 0; }
};

#endif
