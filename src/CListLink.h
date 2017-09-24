#ifndef CLIST_LINK_H
#define CLIST_LINK_H

#include <CStateIterator.h>

template<typename T>
class CListLink {
 public:
  typedef CListLink<T> ListLink;

  CListLink() {
    next = 0;
    prev = 0;
  }

  void addTo(T **head) {
    T *th = (T *) this;

    if (*head) {
      th     ->next       = *head;
      th     ->prev       = (*head)->prev;
      (*head)->prev       = th;
      th     ->prev->next = th;
    }
    else {
      *head = th;

      (*head)->next = (*head)->prev = th;
    }
  }

  void removeFrom(T **head) {
    T *th = (T *) this;

    if (*head) {
      if      (*head == (*head)->next)
        *head = 0;
      else if (th == *head)
        (*head) = (*head)->next;

      th->next->prev = th->prev;
      th->prev->next = th->next;
    }
  }

  class IteratorState {
   public:
    IteratorState(const ListLink *list=0) :
     list_(list), p_(list), end_(false) {
      end_ = (list_ == 0);
    }

    void next() {
      assert(p_ != 0);

      p_ = p_->next;

      end_ = (p_ == list_);
    }

    const T *contents() const {
      assert(p_ != 0);

      return (const T *) p_;
    }

    friend bool operator==(const IteratorState &lhs, const IteratorState &rhs) {
      if (lhs.end_ == rhs.end_) return true;
      if (lhs.end_ != rhs.end_) return false;

      return (lhs.p_ == rhs.p_);
    }

   private:
    const ListLink *list_;
    const ListLink *p_;
    bool            end_;
  };

  typedef CInputIterator<IteratorState, T *> iterator;

  iterator begin() const { return iterator(IteratorState(this)); }
  iterator end  () const { return iterator(IteratorState(0));}

 public:
  T *next;
  T *prev;
};

#endif
