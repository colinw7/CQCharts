#ifndef CQChartsReals_H
#define CQChartsReals_H

#include <CQChartsUtil.h>
#include <CQUtilMeta.h>

#include <QString>
#include <iostream>

/*!
 * \brief Reals class
 * ingroup Charts
 *
 * container for vector of doubles with variant support
 */
class CQChartsReals :
  public CQChartsEqBase<CQChartsReals>,
  public CQChartsToStringBase<CQChartsReals> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsReals, metaTypeId)

 public:
  using Reals = std::vector<double>;

 public:
  CQChartsReals(const Reals &reals=Reals()) :
   reals_(reals) {
  }

  explicit CQChartsReals(const QString &s) {
    fromString(s);
  }

  int numReals() const { return reals_.size(); }

  const Reals &reals() const { return reals_; }
  double real(int i) const { assert(i >= 0 && i < numReals()); return reals_[i]; }

  bool isValid() const { return true; }

  void setReals(const Reals &reals) { reals_ = reals; }

  void addReal(double r) { reals_.push_back(r); }

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  friend bool operator==(const CQChartsReals &lhs, const CQChartsReals &rhs) {
    int nl = lhs.reals_.size();
    int nr = rhs.reals_.size();

    if (nl != nr) return false;

    for (int i = 0; i < nl; ++i) {
      if (lhs.reals_[i] != rhs.reals_[i])
        return false;
    }

    return true;
  }

  //---

 private:
  Reals reals_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsReals)

#endif
