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

  void invalidate(bool reload);

 private:
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

  QColor interpColor(const CQChartsColor &c, int i, int n) const;
  QColor interpColor(const CQChartsColor &c, double r) const;
  QColor interpColor(const CQChartsColor &c, const CQChartsUtil::ColorInd &ind) const;

 private:
  QObject* obj_ { nullptr };
};

#endif
