#ifndef CQChartsColumnNum_H
#define CQChartsColumnNum_H

#include <CQChartsTmpl.h>
#include <QString>

/*!
 * \brief Class to store column number
 * \ingroup Charts
 */
class CQChartsColumnNum :
  public CQChartsComparatorBase<CQChartsColumnNum> {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsColumnNum() = default;

  explicit CQChartsColumnNum(int column); // data

  explicit CQChartsColumnNum(const QString &s); // parsed

  //--

  bool isValid() const { return column_ >= 0; }

  //--

  int column() const { return column_; }

  //--

  QString toString() const;

  bool fromString(const QString &s);

  //---

  int cmp(const CQChartsColumnNum &c) const {
    if (column_ < c.column_) return -1;
    if (column_ > c.column_) return  1;
    return 0;
  }

  friend int cmp(const CQChartsColumnNum &c1, const CQChartsColumnNum &c2) {
    return c1.cmp(c2);
  }

  //---

 private:
  int column_ { -1 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsColumnNum)

#endif
