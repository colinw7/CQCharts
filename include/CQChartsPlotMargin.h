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
  using Plot   = CQChartsPlot;
  using Length = CQChartsLength;
  using BBox   = CQChartsGeom::BBox;
  using Size   = CQChartsGeom::Size;

 public:
  CQChartsPlotMargin() = default;

  explicit CQChartsPlotMargin(const QString &str);

  CQChartsPlotMargin(const Length &l, const Length &t, const Length &r, const Length &b);

//CQChartsPlotMargin(double l, double t, double r, double b);

  explicit CQChartsPlotMargin(const Length &l);

//CQChartsPlotMargin(double l);

  void set(const Length &l, const Length &t, const Length &r, const Length &b);

  void set(const Length &l);

  bool setValue(const QString &str);

  BBox adjustViewRange(const Plot *plot, const BBox &bbox, bool inside=false) const;
  BBox adjustPlotRange(const Plot *plot, const BBox &bbox, bool inside=false) const;
};

#endif
