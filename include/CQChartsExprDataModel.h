#ifndef CQChartsExprDataModel_H
#define CQChartsExprDataModel_H

#include <CQDataModel.h>

/*!
 * \brief Wrapper class for CQDataModel to remember number of rows
 * \ingroup Charts
 */
class CQChartsExprDataModel : public CQDataModel {
 public:
  CQChartsExprDataModel(int n);

  int n() const { return n_; }

 private:
  int n_ { 0 };
};

#endif
