#ifndef CQChartsRectPlacer_H
#define CQChartsRectPlacer_H

#include <CQChartsQuadTree.h>
#include <CQChartsTmpl.h>

#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <iostream>

class CQChartsRectPlacer {
 public:
  class Rect : public CQChartsPrintBase<Rect> {
   public:
    Rect() = default;

    Rect(double xmin, double ymin, double xmax, double ymax) :
     set_(true), xmin_(xmin), ymin_(ymin), xmax_(xmax), ymax_(ymax) {
    }

    double xmin() const { return xmin_; }
    double ymin() const { return ymin_; }
    double xmax() const { return xmax_; }
    double ymax() const { return ymax_; }

    double xmid() const { return (xmin_ + xmax_)/2.0; }
    double ymid() const { return (ymin_ + ymax_)/2.0; }

    double width () const { return xmax_ - xmin_; }
    double height() const { return ymax_ - ymin_; }

    //---

    bool isSet() const { return set_; }

    double getXMin() const { return xmin_; }
    double getYMin() const { return ymin_; }
    double getXMax() const { return xmax_; }
    double getYMax() const { return ymax_; }

    const Rect &rect() const { return *this; }

    //---

    bool inside(const Rect &rect) const {
      if (! set_ || ! rect.set_) return false;

      return ((rect.xmin() >= xmin() && rect.xmax() <= xmax()) &&
              (rect.ymin() >= ymin() && rect.ymax() <= ymax()));
    }

    //---

    void print(std::ostream &os) const {
      os << xmin_ << " " << ymin_ << " " << xmax_ << " " << ymax_;
    }

   private:
    bool   set_  { false };
    double xmin_ { 0.0 };
    double ymin_ { 0.0 };
    double xmax_ { 0.0 };
    double ymax_ { 0.0 };
  };

  using Rects = std::vector<Rect>;

  //---

  class RectData {
   public:
    RectData() = default;

    virtual ~RectData() = default;

    virtual const Rect &rect() const = 0;
    virtual void setRect(const Rect &r) = 0;

    //---

    double getXMin() const { return rect().getXMin(); }
    double getYMin() const { return rect().getYMin(); }
    double getXMax() const { return rect().getXMax(); }
    double getYMax() const { return rect().getYMax(); }

    //---

    virtual std::string name() const { return ""; }
  };

  using RectDatas = std::vector<RectData *>;

  //---

  class Grid {
   public:
    using ValueSet = std::set<double>;
    using QuadTree = CQChartsQuadTree<RectData, Rect>;

   public:
    Grid() = default;

    void clear() {
      tree_.reset();

      clearXYVals();
    }

    void clearXYVals() {
      xvals_.clear();
      yvals_.clear();
    }

    //---

    bool rectEmpty(const Rect &rect) {
      return ! tree_.isDataTouchingRect(rect);
    }

    void getTouchingRects(const Rect &rect, RectDatas &rectDatas) {
      QuadTree::DataList dataList;

      tree_.dataTouchingRect(rect, dataList);

      for (const auto &data : dataList)
        rectDatas.push_back(data);
    }

    //---

    Rect lrect(const Rect &rect, double x) {
      auto px = std::upper_bound(xvals_.rbegin(), xvals_.rend(), x,
                                 [](double a, double b){ return a >= b; });
      double xmax = (px == xvals_.rend() ? x : *px);

      return Rect(xmax - rect.width() - tol(), rect.ymin(), xmax - tol(), rect.ymax());
    }

    Rect rrect(const Rect &rect, double x) {
      auto px = std::lower_bound(xvals_.begin(), xvals_.end(), x);
      double xmin = (px == xvals_.end() ? x : *px);

      return Rect(xmin + tol(), rect.ymin(), xmin + rect.width() + tol(), rect.ymax());
    }

    Rect brect(const Rect &rect, double y) {
      auto py = std::upper_bound(yvals_.rbegin(), yvals_.rend(), y,
                                 [](double a, double b){ return a >= b; });
      double ymax = (py == yvals_.rend() ? y : *py);

      return Rect(rect.xmin(), ymax - rect.height() - tol(), rect.xmax(), ymax - tol());
    }

    Rect trect(const Rect &rect, double y) {
      auto py = std::lower_bound(yvals_.begin(), yvals_.end(), y);
      double ymin = (py == yvals_.end() ? y : *py);

      return Rect(rect.xmin(), ymin + tol(), rect.xmax(), ymin + rect.height() + tol());
    }

    //---

    Rect tlrect(const Rect &rect, double x, double y) {
      auto px = std::upper_bound(xvals_.rbegin(), xvals_.rend(), x,
                                 [](double a, double b){ return a >= b; });
      double xmax = (px == xvals_.rend() ? x : *px);

      auto py = std::lower_bound(yvals_.begin(), yvals_.end(), y);
      double ymin = (py == yvals_.end() ? y : *py);

      return Rect(xmax - rect.width(), ymin, xmax, ymin + rect.height());
    }

    Rect trrect(const Rect &rect, double x, double y) {
      auto px = std::lower_bound(xvals_.begin(), xvals_.end(), x);
      double xmin = (px == xvals_.end() ? x : *px);

      auto py = std::lower_bound(yvals_.begin(), yvals_.end(), y);
      double ymin = (py == yvals_.end() ? y : *py);

      return Rect(xmin, ymin, xmin + rect.width(), ymin + rect.height());
    }

    Rect blrect(const Rect &rect, double x, double y) {
      auto px = std::upper_bound(xvals_.rbegin(), xvals_.rend(), x,
                                 [](double a, double b){ return a >= b; });
      double xmax = (px == xvals_.rend() ? x : *px);

      auto py = std::upper_bound(yvals_.rbegin(), yvals_.rend(), y,
                                 [](double a, double b){ return a >= b; });
      double ymax = (py == yvals_.rend() ? y : *py);

      return Rect(xmax - rect.width(), ymax - rect.height(), xmax, ymax);
    }

    Rect brrect(const Rect &rect, double x, double y) {
      auto px = std::lower_bound(xvals_.begin(), xvals_.end(), x);
      double xmin = (px == xvals_.end() ? x : *px);

      auto py = std::upper_bound(yvals_.rbegin(), yvals_.rend(), y,
                                 [](double a, double b){ return a >= b; });
      double ymax = (py == yvals_.rend() ? y : *py);

      return Rect(xmin, ymax - rect.height(), xmin + rect.width(), ymax);
    }

    //---

    void addRect(RectData *rect) {
      tree_.add(rect);
    }

    void addRectValues(const Rect &rect) {
      // update grid
      xvals_.insert(rect.xmin());
      yvals_.insert(rect.ymin());
      xvals_.insert(rect.xmax());
      yvals_.insert(rect.ymax());
    }

    const QuadTree &tree() const { return tree_; }

    const ValueSet &xvals() const { return xvals_; }
    const ValueSet &yvals() const { return yvals_; }

    double tol() const { return tol_; }

   private:
    QuadTree tree_;
    ValueSet xvals_;
    ValueSet yvals_;
    double   tol_ { 0.1 };
  };

 public:
  CQChartsRectPlacer();

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  void addRect(RectData *rectData);

  void addRectValues(RectData *rectData);
  void addRectValues(const Rect &rect);

  const RectDatas &rectDatas() const { return rectDatas_; }

  const Grid &grid() const { return grid_; }

  const Rect &clipRect() const { return clipRect_; }
  void setClipRect(const Rect &v) { clipRect_ = v; }

  int iterations() const { return iterations_; }
  void setIterations(int i) { iterations_ = i; }

  void clear(bool del=false) {
    if (del) {
      for (auto &rectData : rectDatas_)
        delete rectData;
    }

    rectDatas_.clear();

    clearGrid();
  }

  void clearGrid() {
    grid_.clear();
  }

  //---

  void place();

  bool stepPlace();

  bool moveAllOut();

 private:
  bool moveOut(RectData *rectData);

 private:
  struct HitRect {
    double xmin { 0.0 };
    double ymin { 0.0 };
    double xmax { 0.0 };
    double ymax { 0.0 };
  };

 private:
  void getEmptyRects(RectData *rectData, const Rects &dirRects,
                     Rects &emptyRects, RectDatas &hitRects);

  void getTouchingRects(RectData *rectData, const Rect &dirRect, RectDatas &hitRects);

  void getBestDirRect(RectData *rectData, const Rects &emptyRects, Rect &erect);

  void addDirRects(const Rect &dirRect, Rects &dirRects, const HitRect &hitRec);

  void calcHitRectRange(const RectDatas &hitRects, HitRect &hitRect, bool min, double tol=0.0);

 private:
  using QuadTree = CQChartsQuadTree<Rect, Rect>;

  bool      debug_      { false };
  Grid      grid_;
  RectDatas rectDatas_;
  Rect      clipRect_;
  int       iterations_ { 32 };
};

#endif
