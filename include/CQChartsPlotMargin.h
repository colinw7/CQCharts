#ifndef CQChartsPlotMargin_H
#define CQChartsPlotMargin_H

#include <CQChartsMargin.h>
#include <CQChartsGeom.h>
#include <QStringList>

class CQChartsPlot;

/*!
 * \brief plot margin
 * \ingroup Charts
 */
class CQChartsPlotMargin : public CQChartsMargin {
 public:
  CQChartsPlotMargin(const QString &str) {
    setValue(str);
  }

  CQChartsPlotMargin(const CQChartsLength &l=CQChartsLength(),
                     const CQChartsLength &t=CQChartsLength(),
                     const CQChartsLength &r=CQChartsLength(),
                     const CQChartsLength &b=CQChartsLength()) :
   CQChartsMargin(l, t, r, b) {
  }

  CQChartsPlotMargin(double l, double t, double r, double b) :
   CQChartsMargin(l, t, r, b) {
  }

  CQChartsPlotMargin(const CQChartsLength &l) :
   CQChartsMargin(l, l, l, l) {
  }

  CQChartsPlotMargin(double l) :
   CQChartsMargin(l) {
  }

  void set(const CQChartsLength &l, const CQChartsLength &t,
           const CQChartsLength &r, const CQChartsLength &b) {
    CQChartsMargin::set(l, t, r, b);
  }

  void set(const CQChartsLength &l) {
    CQChartsMargin::set(l);
  }

  bool setValue(const QString &str) {
    return CQChartsMargin::setValue(str);
  }

  CQChartsGeom::BBox adjustViewRange(const CQChartsPlot *plot, const CQChartsGeom::BBox &bbox,
                                     bool inside=false) const;
  CQChartsGeom::BBox adjustPlotRange(const CQChartsPlot *plot, const CQChartsGeom::BBox &bbox,
                                     bool inside=false) const;
};

#endif
