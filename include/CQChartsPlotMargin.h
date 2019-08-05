#ifndef CQChartsPlotMargin_H
#define CQChartsPlotMargin_H

#include <CQChartsLength.h>
#include <CQChartsGeom.h>
#include <QStringList>

class CQChartsPlot;

/*!
 * \brief plot margin
 * \ingroup Charts
 */
class CQChartsPlotMargin {
 public:
  CQChartsPlotMargin(const QString &str) {
    setValue(str);
  }

  CQChartsPlotMargin(const CQChartsLength &l=CQChartsLength(),
                     const CQChartsLength &t=CQChartsLength(),
                     const CQChartsLength &r=CQChartsLength(),
                     const CQChartsLength &b=CQChartsLength()) {
    set(l, t, r, b);
  }

  CQChartsPlotMargin(double l, double t, double r, double b) {
     set(CQChartsLength(l, CQChartsUnits::PERCENT),
         CQChartsLength(t, CQChartsUnits::PERCENT),
         CQChartsLength(r, CQChartsUnits::PERCENT),
         CQChartsLength(b, CQChartsUnits::PERCENT));
  }

  void set(const CQChartsLength &l, const CQChartsLength &t,
           const CQChartsLength &r, const CQChartsLength &b) {
    left_   = l;
    top_    = t;
    right_  = r;
    bottom_ = b;
  }

  bool setValue(const QString &str) {
    QStringList strs = str.split(" ", QString::SkipEmptyParts);
    if (strs.length() != 4) return false;

    CQChartsLength left, top, right, bottom;

    if (! left  .setValue(strs[0], CQChartsUnits::PERCENT)) return false;
    if (! top   .setValue(strs[1], CQChartsUnits::PERCENT)) return false;
    if (! right .setValue(strs[2], CQChartsUnits::PERCENT)) return false;
    if (! bottom.setValue(strs[3], CQChartsUnits::PERCENT)) return false;

    set(left, top, right, bottom);

    return true;
  }

  const CQChartsLength &left() const { return left_; }
  void setLeft(const CQChartsLength &v) { left_ = v; }

  const CQChartsLength &top() const { return top_; }
  void setTop(const CQChartsLength &v) { top_ = v; }

  const CQChartsLength &right() const { return right_; }
  void setRight(const CQChartsLength &v) { right_ = v; }

  const CQChartsLength &bottom() const { return bottom_; }
  void setBottom(const CQChartsLength &v) { bottom_ = v; }

  CQChartsGeom::BBox adjustViewRange(const CQChartsPlot *plot, const CQChartsGeom::BBox &bbox,
                                     bool inside=false) const;
  CQChartsGeom::BBox adjustPlotRange(const CQChartsPlot *plot, const CQChartsGeom::BBox &bbox,
                                     bool inside=false) const;

 private:
  CQChartsLength left_;
  CQChartsLength top_;
  CQChartsLength right_;
  CQChartsLength bottom_;
};

#endif
