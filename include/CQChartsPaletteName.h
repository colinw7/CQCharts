#ifndef CQChartsPaletteName_H
#define CQChartsPaletteName_H

#include <cassert>
#include <QColor>
#include <iostream>

class QObject;

/*!
 * \brief class to contain palette name
 * \ingroup Charts
 */
class CQChartsPaletteName {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  //! default constructor
  CQChartsPaletteName(const QString &name="") :
   name_(name) {
  }

  bool isValid() const { return name_ != ""; }

  //! get/set name
  const QString &name() const { return name_; }
  void setName(const QString &name) { name_  = name; }

  //---

  //! color to/from string for QVariant
  QString toString() const { return name(); }

  bool fromString(const QString &s) { setName(s); return true; }

  //---

  //! operator ==
  friend bool operator==(const CQChartsPaletteName &lhs, const CQChartsPaletteName &rhs) {
    return (lhs.name_ == rhs.name_);
  }

  //! operator !=
  friend bool operator!=(const CQChartsPaletteName &lhs, const CQChartsPaletteName &rhs) {
    return ! operator==(lhs, rhs);
  }

 private:
  QString name_; //!< name
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPaletteName)

#endif
