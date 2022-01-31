#ifndef CQChartsPaletteName_H
#define CQChartsPaletteName_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

#include <QColor>
#include <QVariant>

#include <cassert>
#include <iostream>

class CQColorsPalette;

class QObject;

/*!
 * \brief class to contain palette name
 * \ingroup Charts
 */
class CQChartsPaletteName :
  public CQChartsEqBase<CQChartsPaletteName> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsPaletteName, metaTypeId)

 public:
  //! default constructor
  explicit CQChartsPaletteName(const QString &str="");

  bool isValid() const;

  //---

  //! get/set name
  const QString &name() const { return name_; }
  bool setName(const QString &name);

  double min() const { return min_; }
  void setMin(double r) { min_ = r; }

  double max() const { return max_; }
  void setMax(double r) { max_ = r; }

  //---

  //! get associated palette data
  CQColorsPalette *palette() const;

  //---

  //! color to/from string for QVariant
  QString toString() const;
  bool fromString(const QString &s);

  //---

  //! operator ==
  friend bool operator==(const CQChartsPaletteName &lhs, const CQChartsPaletteName &rhs) {
    return (lhs.name_ == rhs.name_);
  }

 private:
  QString name_;        //!< name
  double  min_ { 0.0 }; //!< min
  double  max_ { 1.0 }; //!< max
};

//---

CQUTIL_DCL_META_TYPE(CQChartsPaletteName)

#endif
