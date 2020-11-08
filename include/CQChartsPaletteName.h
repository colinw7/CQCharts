#ifndef CQChartsPaletteName_H
#define CQChartsPaletteName_H

#include <cassert>
#include <QColor>
#include <iostream>

class CQColorsPalette;

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
  CQChartsPaletteName(const QString &name="");

  bool isValid() const;

  //! get/set name
  const QString &name() const { return name_; }
  bool setName(const QString &name);

  //---

  CQColorsPalette *palette() const;

  //---

  //! color to/from string for QVariant
  QString toString() const { return name(); }
  bool fromString(const QString &s) { return setName(s); }

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
