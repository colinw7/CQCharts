#ifndef CHRTimer_H
#define CHRTimer_H

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <sys/time.h>

#define CHRTimerMgrInst CHRTimerMgr::getInstance()

struct CHRTime {
  long secs;
  long usecs;

  CHRTime() : secs(0), usecs(0) { }

  friend std::ostream &operator<<(std::ostream &os, const CHRTime &hrtime) {
    double d = hrtime.secs + hrtime.usecs/1000000.;

    os << d;

    return os;
  }

  double getSecs () { return         secs + usecs/1000000.0; }
  double getMSecs() { return    1000*secs + usecs/1000.0   ; }
  double getUSecs() { return 1000000*secs + usecs          ; }

  const CHRTime &operator+=(const CHRTime &rhs) {
    usecs += rhs.usecs;
    secs  += rhs.secs;

    if (usecs > 1000000) {
      secs  += usecs / 10000000;
      usecs  = usecs % 10000000;
    }

    return *this;
  }

  CHRTime operator+(const CHRTime &rhs) {
    CHRTime t = *this;

    t += rhs;

    return t;
  }
};

class CHRTimerMgr {
 public:
  static CHRTimerMgr *getInstance() {
    static CHRTimerMgr *instance;

    if (! instance)
      instance = new CHRTimerMgr;

    return instance;
  }

  bool isActive() const {
    if (! active_) {
      char *env = getenv("HRTIMER_ACTIVE");

      if (env == NULL) return false;

      uint len = strlen(env);

      if (len > 4) return false;

      char str[5];

      uint i = 0;

      for ( ; env[i] != '\0'; ++i) str[i] = tolower(env[i]);
      for ( ; i < 5         ; ++i) str[i] = '\0';

      if (strcmp(str, "yes" ) == 0 || strcmp(str, "on") == 0 ||
          strcmp(str, "true") == 0 || strcmp(str, "1" ) == 0)
        return true;

      return false;
    }
    else
      return active_;
  }

  void setActive(bool active=true) {
    active_ = active;
  }

  bool start(int *ind, const char *msg=NULL) {
    HRTimer &timer = timers_[ind_];

    if (timer.active) {
      std::cerr << "No timer slots remaining" << std::endl;
      return false;
    }

    *ind = ind_;

    timer.time   = getHRTime();
    timer.active = true;

    ++num_active_;

    /*----*/

    /* Find Next Timer Slot */

    ++ind_;

    int count = 1;

    if (ind_ >= MAX_TIMERS)
      ind_ = 0;

    while (timers_[ind_].active && count < MAX_TIMERS) {
      ++ind_;
      ++count;

      if (ind_ >= MAX_TIMERS)
        ind_ = 0;
    }

    if (msg) {
      std::cout << ">";

      for (int i = 0; i < num_active_ - 1; ++i) std::cout << " ";

      std::cout << msg << std::endl;
    }

    return true;
  }

  bool end(int ind, long *secs=NULL, long *usecs=NULL, const char *msg=NULL) {
    HRTimer &timer = timers_[ind];

    assert(timer.active);

    CHRTime now = getHRTime();

    CHRTime d = diffHRTime(timer.time, now);

    if (secs ) *secs  = d.secs;
    if (usecs) *usecs = d.usecs;

    timer.active = false;

    --num_active_;

    if (msg) {
      double elapsed = d.secs + d.usecs/1000000.0;

      std::cout << "<";

      for (int i = 0; i < num_active_; ++i) std::cout << " ";

      std::cout << msg << " " << elapsed << std::endl;
    }

    return true;
  }

  CHRTime elapsed(int ind) {
    HRTimer &timer = timers_[ind];

    assert(timer.active);

    return timer.time;
  }

  static CHRTime getHRTime() {
    CHRTime hrtime;

    struct timeval timeval;

    gettimeofday(&timeval, NULL);

    hrtime.secs  = timeval.tv_sec;
    hrtime.usecs = timeval.tv_usec;

    return hrtime;
  }

  static CHRTime diffHRTime(const CHRTime &hrtime1, const CHRTime &hrtime2) {
    CHRTime hrtime;

    hrtime.usecs = (hrtime2.secs - hrtime1.secs)*1000000 + (hrtime2.usecs - hrtime1.usecs);

    hrtime.secs   = hrtime.usecs / 1000000;
    hrtime.usecs %= 1000000;

    return hrtime;
  }

 private:
  CHRTimerMgr() :
   ind_(0), num_active_(0), active_(false) {
  }

 private:
  enum { MAX_TIMERS = 50 };

 private:
  struct HRTimer {
    bool      active;
    CHRTime time;

    HRTimer() : active(false) { }
  };

  int     ind_;
  int     num_active_;
  HRTimer timers_[MAX_TIMERS];
  bool    active_;
};

//------

class CScopeTimer {
 public:
  CScopeTimer(const std::string &id) :
   id_(id), timer_id_(-1) {
    if (! CHRTimerMgrInst->isActive()) return;

    if (! CHRTimerMgrInst->start(&timer_id_, id_.c_str()))
      timer_id_ = -1;
  }

 ~CScopeTimer() {
    if (timer_id_ < 0) return;

    long secs, usecs;

    CHRTimerMgrInst->end(timer_id_, &secs, &usecs, id_.c_str());
  }

 private:
  std::string id_;
  int         timer_id_;
};

//-----

class CIncrementalTimer {
 public:
  CIncrementalTimer(const std::string &id) :
   id_(id), active_(false), timer_id_(-1), elapsed_secs_(0), elapsed_usecs_(0), count_(0) {
    active_ = CHRTimerMgrInst->isActive();
  }

 ~CIncrementalTimer() {
    report();
  }

  void reset() {
    active_ = CHRTimerMgrInst->isActive();

    elapsed_secs_  = 0;
    elapsed_usecs_ = 0;
  }

  void start() {
    if (! active_) return;

    ++count_;

    if (! CHRTimerMgrInst->start(&timer_id_, NULL))
      timer_id_ = -1;
  }

  void stop() {
    if (! active_) return;

    if (timer_id_ < 0) return;

    long secs, usecs;

    CHRTimerMgrInst->end(timer_id_, &secs, &usecs, NULL);

    elapsed_secs_  += secs;
    elapsed_usecs_ += usecs;

    if (elapsed_usecs_ > 1000000) {
      elapsed_secs_  += elapsed_usecs_ / 1000000;
      elapsed_usecs_ %= 1000000;
    }
  }

  void report() {
    if (! active_) return;

    if (! count_) return;

    double elapsed  = elapsed_secs_ + elapsed_usecs_/1000000.0;
    double interval = elapsed/count_;

    std::cout << id_ << ": total=" << elapsed << " calls=" << count_ <<
                 " interval=" << interval << std::endl;
  }

 private:
  std::string id_;
  bool        active_;
  int         timer_id_;
  long        elapsed_secs_, elapsed_usecs_;
  long        count_;
};

#endif
