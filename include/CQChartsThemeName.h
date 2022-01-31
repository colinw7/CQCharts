#ifndef CQChartsThemeName_H
#define CQChartsThemeName_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>
#include <QString>

class CQColorsTheme;

/*!
 * \brief theme name
 * \ingroup Charts
 */
class CQChartsThemeName :
  public CQChartsEqBase<CQChartsThemeName>,
  public CQChartsToStringBase<CQChartsThemeName> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsThemeName, metaTypeId)

 public:
  explicit CQChartsThemeName(const QString &name="") {
    setName(name);
  }

  CQChartsThemeName(const CQChartsThemeName &rhs) :
    name_(rhs.name_), obj_(rhs.obj_) {
  }

  CQChartsThemeName &operator=(const CQChartsThemeName &rhs) {
    name_ = rhs.name_;
    obj_  = rhs.obj_;

    return *this;
  }

  //--

  const QString &name() const { return name_; }

  bool setName(const QString &name);

  //--

  CQColorsTheme *obj() const { return obj_; }

  //---

  QString toString() const { return name(); }
  bool fromString(const QString &s) { return setName(s); }

  //---

  friend bool operator==(const CQChartsThemeName &lhs, const CQChartsThemeName &rhs) {
    if (lhs.name_ != rhs.name_) return false;

    return true;
  }

  //---

 private:
  QString        name_;
  CQColorsTheme* obj_ { nullptr };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsThemeName)

#endif
