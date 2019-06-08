#ifndef CQChartsExprDataModel_H
#define CQChartsExprDataModel_H

#include <CQDataModel.h>

class CQChartsExprDataModel : public CQDataModel {
 public:
  CQChartsExprDataModel(int n);

  int n() const { return n_; }

 private:
  int n_ { 0 };
};

#endif
