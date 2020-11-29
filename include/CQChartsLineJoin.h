#ifndef CQChartsLineJoin_H
#define CQChartsLineJoin_H

#include <CQChartsEnum.h>
#include <QPen>

/*!
 * \brief line join
 * \ingroup Charts
 */
class CQChartsLineJoin : public CQChartsEnum<CQChartsLineJoin> {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsLineJoin();

  CQChartsLineJoin(const Qt::PenJoinStyle &join);

  explicit CQChartsLineJoin(const QString &str);

 ~CQChartsLineJoin() { }

  //---

  const Qt::PenJoinStyle &join() const { return join_; }

  //---

  bool isValid() const { return true; }

  const Qt::PenJoinStyle &type() const { return join_; }
  void setType(const Qt::PenJoinStyle &join) { join_ = join; }

  //--

  QString toString() const override;
  bool fromString(const QString &str) override;

  //---

  QStringList enumNames() const override;

 private:
  Qt::PenJoinStyle join_ { Qt::MiterJoin };
};

//------

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsLineJoin)

#endif
