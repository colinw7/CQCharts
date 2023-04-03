#ifndef CQChartsInvalidator_H
#define CQChartsInvalidator_H

#include <CQChartsUtil.h>
#include <QColor>

class CQChartsColor;
class QObject;

/*!
 * \brief class to forward invalidation to object
 * \ingroup Charts
 */
class CQChartsInvalidator {
 public:
  CQChartsInvalidator(QObject *obj) :
    obj_(obj) {
  }

  virtual ~CQChartsInvalidator() = default;

  virtual void invalidate(bool reload);

 protected:
  QObject* obj_ { nullptr };
};

/*!
 * \brief class to forward color interpolation to object
 * \ingroup Charts
 */
class CQChartsInterpolator {
 public:
  CQChartsInterpolator(QObject *obj) :
    obj_(obj) {
  }

  virtual ~CQChartsInterpolator() = default;

  virtual QColor interpColor(const CQChartsColor &c, const CQChartsUtil::ColorInd &ind) const;

 protected:
  QObject* obj_ { nullptr };
};

#endif
