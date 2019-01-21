#ifndef CQChartsReadLine_H
#define CQChartsReadLine_H

#include <CQChartsTest.h>
#include <CReadLine.h>

class CQChartsReadLine : public CReadLine {
 public:
  CQChartsReadLine(CQChartsTest *test) :
   test_(test) {
  }

  void timeout() {
    test_->timeout();
  }

 private:
  CQChartsTest *test_;
};

#endif
