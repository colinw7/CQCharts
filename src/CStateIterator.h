#ifndef CITERATOR_H
#define CITERATOR_H

#include <iterator>
#include <cstddef>
//#include <CGeneric.h>

// input iterator supports:
//  . Default construct
//  . Copy construct
//  . Copy assign
//  . Increment pre/post
//  . ==, !=
//  . * (dereference to value)
//
// single pass with a shared underlying iterator value for a class
//
// i.e  II i1, i2;
//      i1 = o.begin();
//      i2 = i1;
//      ++i1
//      assert(*i1 == *i2);
//
template<class STATE, class T, class DIST=ptrdiff_t>
class CInputIterator : public std::iterator<std::input_iterator_tag,T,DIST> {
 public:
  enum { IS_PTR = std::is_pointer<T>::value };

  typedef typename std::remove_reference<T>::type     NonRefT;
  typedef typename std::remove_pointer<NonRefT>::type NonPtrT;
  typedef typename std::remove_cv<NonPtrT>::type      BareT;

  typedef typename std::conditional<IS_PTR,const BareT *,const BareT &>::type ContentsT;

  CInputIterator() :
   state_() {
  }

  CInputIterator(const STATE &state) :
   state_(state) {
  }

  CInputIterator(const CInputIterator &i) :
   state_(i.state_) {
  }

 ~CInputIterator() { }

  const CInputIterator &operator=(const CInputIterator &i) {
    state_ = i.state_;
    return *this;
  }

  const CInputIterator &operator++() { state_.next(); return *this; }

  const CInputIterator &operator++(int) { ++(*this); }

  friend bool operator==(const CInputIterator &lhs, const CInputIterator &rhs) {
    return lhs.state_ == rhs.state_;
  }

  friend bool operator!=(const CInputIterator &lhs, const CInputIterator &rhs) {
    return ! (lhs == rhs);
  }

  ContentsT operator*() const {
    return state_.contents();
  }

 protected:
  STATE state_;
};

#if 0
// output iterator supports:
//  . Default construct
//  . Copy construct
//  . Copy assign
//  . Increment pre/post
//  . ==, !=
//  . * (dereference to value)
//  . * (dereference to lvalue)
//
template<class T, class DIST=ptrdiff_t>
class COutputIterator : public std::iterator<std::output_iterator_tag,T,DIST> {
 public:
  COutputIterator();
  COutputIterator(const COutputIterator &i);

  const COutputIterator &operator=(const COutputIterator &i);

  const COutputIterator &operator++();

  const COutputIterator &operator++(int) { ++(*this); }

  friend bool operator==(const COutputIterator &lhs, const COutputIterator &rhs) const;

  friend bool operator!=(const COutputIterator &lhs, const COutputIterator &rhs) const {
    return ! (lhs == rhs);
  }

  const T &operator*() const;

  T &operator*();
};

// forward iterator supports:
//  . Default construct
//  . Copy construct
//  . Copy assign
//  . Increment pre/post
//  . ==, !=
//  . * (dereference to value)
//  . * (dereference to lvalue)
//
// each iterator is unique so changing one does not change others
template<class T, class DIST=ptrdiff_t>
class CForwardIterator : public forward_iterator<T,DIST> {
 public:
  CForwardIterator();
  CForwardIterator(const CForwardIterator &i);

  const CForwardIterator &operator=(const CForwardIterator &i);

  const CForwardIterator &operator++();

  const CForwardIterator &operator++(int) { ++(*this); }

  friend bool operator==(const CForwardIterator &lhs, const CForwardIterator &rhs) const;

  friend bool operator!=(const CForwardIterator &lhs, const CForwardIterator &rhs) const {
    return ! (lhs == rhs);
  }

  const T &operator*() const;

  T &operator*();
};

// bidirectional iterator supports:
//  . Default construct
//  . Copy construct
//  . Copy assign
//  . Increment pre/post
//  . Decrement pre/post
//  . ==, !=
//  . * (dereference to value)
//  . * (dereference to lvalue)
//
// each iterator is unique so changing one does not change others
template<class T, class DIST=ptrdiff_t>
class CBidirectionalIterator : public bidirectional_iterator<T,DIST> {
 public:
  CBidirectionalIterator();
  CBidirectionalIterator(const CBidirectionalIterator &i);

  const CBidirectionalIterator &operator=(const CBidirectionalIterator &i);

  const CBidirectionalIterator &operator++();

  const CBidirectionalIterator &operator++(int) { ++(*this); }

  const CBidirectionalIterator &operator--();

  const CBidirectionalIterator &operator--(int) { --(*this); }

  friend bool operator==(const CBidirectionalIterator &lhs,
                         const CBidirectionalIterator &rhs) const;

  friend bool operator!=(const CBidirectionalIterator &lhs,
                         const CBidirectionalIterator &rhs) const {
    return ! (lhs == rhs);
  }

  const T &operator*() const;

  T &operator*();
};

// random access iterator supports:
//  . Default construct
//  . Copy construct
//  . Copy assign
//  . Increment pre/post
//  . Decrement pre/post
//  . ==, !=
//  . * (dereference to value)
//  . * (dereference to lvalue)
//  . pointer arithmetic +, -, []
//
// each iterator is unique so changing one does not change others
template<class T, class DIST=ptrdiff_t>
class CRandomAccessIterator : public random_access_iterator<T,DIST> {
 public:
  CRandomAccessIterator();
  CRandomAccessIterator(const CRandomAccessIterator &i);

  const CRandomAccessIterator &operator=(const CRandomAccessIterator &i);

  const CRandomAccessIterator &operator++();

  const CRandomAccessIterator &operator++(int) { ++(*this); }

  const CRandomAccessIterator &operator--();

  const CRandomAccessIterator &operator--(int) { --(*this); }

  CRandomAccessIterator operator+(int d) const;
  CRandomAccessIterator operator-(int d) const;

  CRandomAccessIterator operator[](int i) const;

  friend bool operator==(const CRandomAccessIterator &lhs,
                         const CRandomAccessIterator &rhs) const;

  friend bool operator!=(const CRandomAccessIterator &lhs,
                         const CRandomAccessIterator &rhs) const {
    return ! (lhs == rhs);
  }

  const T &operator*() const;

  T &operator*();
};

// use std::reverse_iterator to define reverse iterator from iterator

class vector {
 public:
  typedef Iterator iterator;
  typedef Iterator const_iterator;

  std::reverse_iterator<iterator>       reverse_iterator;
  std::reverse_iterator<const_iterator> const_reverse_iterator;
};
#endif

#endif
