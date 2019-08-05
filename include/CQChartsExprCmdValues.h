#ifndef CQChartsExprCmdValues_H
#define CQChartsExprCmdValues_H

#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <QVariant>
#include <vector>

/*!
 * \brief expression model command values
 * \ingroup Charts
 */
class CQChartsExprCmdValues {
 public:
  using Values = std::vector<QVariant>;

 public:
  CQChartsExprCmdValues(const Values &values) :
   values_(values) {
    eind_ = numValues() - 1;
  }

  int ind() const { return ind_; }

  bool hasValues() const { return ! values_.empty(); }

  int numValues() const { return values_.size(); }

  QVariant popValue() { QVariant value = values_.back(); values_.pop_back(); return value; }

  bool getInt(int &i) {
    if (ind_ > eind_) return false;

    bool ok;

    long i1 = CQChartsVariant::toInt(values_[ind_], ok);

    if (ok) {
      i = i1;

      ++ind_;
    }

    return ok;
  }

  bool getReal(double &r) {
    if (ind_ > eind_) return false;

    bool ok;

    double r1 = values_[ind_].toDouble(&ok);

    if (ok) {
      r = r1;

      ++ind_;
    }

    return ok;
  }

  bool getStr(QString &s) {
    if (ind_ > eind_) return false;

    s = values_[ind_++].toString();

    return true;
  }

  bool getBool(bool &b) {
    if (ind_ > eind_) return false;

    b = values_[ind_].toBool();

    ++ind_;

    return true;
  }

  //---

 private:
  Values values_;
  int    ind_  { 0 };
  int    eind_ { 0 };
};

#endif
