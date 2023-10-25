#ifndef CQChartsValueList_H
#define CQChartsValueList_H

#include <CQChartsTmpl.h>
#include <CQChartsVariant.h>
#include <CQUtilMeta.h>

#include <QVariant>

class CQChartsValueList :
 public CQChartsComparatorBase<CQChartsValueList>,
  public CQChartsToStringBase<CQChartsValueList> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsValueList, metaTypeId)

 public:
  using ValueList = std::vector<QVariant>;

 public:
  CQChartsValueList() = default;

  const ValueList &values() const { return valueList_; }

  uint numValues() const { return uint(valueList_.size()); }

  const QVariant &value(uint i) const {
    assert(i < uint(valueList_.size())); return valueList_[i]; }

  void clear() {
    valueList_.clear();

    updateCurrentInd();
  }

  void addValue(const QVariant &value) {
    valueList_.push_back(value);

    updateCurrentInd();
  }

  int currentInd() const { return currentInd_; }
  void setCurrentInd(int i) { currentInd_ = i; updateCurrentInd(); }

  //---

  QString toString() const;
  bool fromString(const QString &s);

  //---

  //! compare for (==, !=, <, >, <=, >=)
  friend int cmp(const CQChartsValueList &lhs, const CQChartsValueList &rhs) {
    auto n1 = lhs.valueList_.size();
    auto n2 = rhs.valueList_.size();
    if (n1 != n2) return (n1 > n2 ? 1 : -1);

    for (std::size_t i = 0; i < n1; ++i) {
      if (lhs.valueList_[i] != rhs.valueList_[i])
        return CQChartsVariant::cmp(lhs.valueList_[i], rhs.valueList_[i]);
    }

    if (lhs.currentInd_ != rhs.currentInd_)
      return (lhs.currentInd_ > rhs.currentInd_ ? 1 : -1);

    return 0;
  }

  //---

 private:
  void updateCurrentInd() {
    if      (valueList_.empty())
      currentInd_ = -1;
    else if (currentInd_ < 0)
      currentInd_ = 0;
    else if (uint(currentInd_) >= numValues())
      currentInd_ = int(numValues()) - 1;
  }

 private:
  ValueList valueList_;
  int       currentInd_ { -1 };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsValueList)

#endif
