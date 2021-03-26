#ifndef CQChartsLineCap_H
#define CQChartsLineCap_H

#include <CQChartsEnum.h>
#include <QPen>

/*!
 * \brief line cap
 * \ingroup Charts
 */
class CQChartsLineCap : public CQChartsEnum<CQChartsLineCap> {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsLineCap();

  explicit CQChartsLineCap(const Qt::PenCapStyle &cap);

  explicit CQChartsLineCap(const QString &str);

 ~CQChartsLineCap() { }

  //---

  const Qt::PenCapStyle &cap() const { return cap_; }

  //---

  bool isValid() const { return true; }

  const Qt::PenCapStyle &type() const { return cap_; }
  void setType(const Qt::PenCapStyle &cap) { cap_ = cap; }

  //--

  QString toString() const override;
  bool fromString(const QString &str) override;

  //---

  QStringList enumNames() const override;

 private:
  Qt::PenCapStyle cap_ { Qt::SquareCap };
};

//------

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsLineCap)

#endif
