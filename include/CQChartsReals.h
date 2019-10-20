#ifndef CQChartsReals_H
#define CQChartsReals_H

#include <CQChartsUtil.h>
#include <QString>
#include <QPointF>
#include <iostream>

/*!
 * \brief Reals class
 * ingroup Charts
 */
class CQChartsReals {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using Reals = std::vector<double>;

 public:
  CQChartsReals(const Reals &reals=Reals()) :
   reals_(reals) {
  }

  explicit CQChartsReals(const QString &s) {
    fromString(s);
  }

  const Reals &reals() const { return reals_; }

  bool isValid() const { return true; }

  void setReals(const Reals &reals) { reals_ = reals; }

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

  friend bool operator!=(const CQChartsReals &lhs, const CQChartsReals &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsReals &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  Reals reals_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsReals)

#endif
