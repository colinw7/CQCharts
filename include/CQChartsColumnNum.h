#ifndef CQChartsColumnNum_H
#define CQChartsColumnNum_H

#include <QString>

/*!
 * \brief Class to store column number
 * \ingroup Charts
 */
class CQChartsColumnNum {
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

  friend bool operator==(const CQChartsColumnNum &lhs, const CQChartsColumnNum &rhs) {
    return (lhs.column_ == rhs.column_);
  }

  friend bool operator!=(const CQChartsColumnNum &lhs, const CQChartsColumnNum &rhs) {
    return ! operator==(lhs, rhs);
  }

  friend bool operator<(const CQChartsColumnNum &lhs, const CQChartsColumnNum &rhs) {
    return (lhs.column_ < rhs.column_);
  }

  //---

 private:
  int column_ { -1 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsColumnNum)

#endif
