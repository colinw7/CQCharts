#ifndef CQChartsPlacer_H
#define CQChartsPlacer_H

#include <CQChartsGeom.h>

// Treemap Placer
// Circle Pack (Bubble)

// Input :
//  N        : all value 1
//  N Values : area
//  BBox     : shape

// Output :
//  Array of shapes (BBox, Ellipse, Polygon, Segment (Pie), ...)

// CQChartsAnnotationGroup::doLayout

class CQChartsPlacer {
 public:
  using BBox  = CQChartsGeom::BBox;
  using Size  = CQChartsGeom::Size;
  using Point = CQChartsGeom::Point;

  using BBoxes = std::vector<BBox>;

 public:
  CQChartsPlacer() { }

  virtual ~CQChartsPlacer() { }

  const Point &center() const { return center_; }
  void setCenter(const Point &p) { center_ = p; }

  const Size &size() const { return size_; }
  void setSize(const Size &s) { size_ = s; }

  double margin() const { return margin_; }
  void setMargin(double r) { margin_ = r; }

  virtual void place(int n) = 0;

  const BBoxes &bboxes() const { return bboxes_; }

  const BBox &bbox(size_t i) const { return bboxes_[i]; }

 protected:
  // default (-1, -1) -> (1, 1)
  Point  center_ { 0.0, 0.0 };
  Size   size_   { 2.0, 2.0 };
  double margin_ { 0.0 };

  BBoxes bboxes_;
};

//---

class CQChartsHPlacer : public CQChartsPlacer {
 public:
  CQChartsHPlacer() { }

  void place(int n) override {
    bboxes_.clear();

    double m = margin();

    double xc = center().x;
    double yc = center().y;

    double w = size().width ();
    double h = size().height();

    double dx = w/n - m;

    double xl = xc - w/2.0;
    double yb = yc - h/2.0;

    for (int i = 0; i < n; ++i) {
      double x1 = xl + i*(dx + m) + m/2.0;
      double y1 = yb;
      double x2 = x1 + dx;
      double y2 = y1 + h;

      bboxes_.emplace_back(x1, y1, x2, y2);
    }
  }
};

class CQChartsVPlacer : public CQChartsPlacer {
 public:
  CQChartsVPlacer() { }

  void place(int n) override {
    bboxes_.clear();

    double m = margin();

    double xc = center().x;
    double yc = center().y;

    double w = size().width ();
    double h = size().height();

    double dy = h/n - m;

    double xl = xc - w/2.0;
    double yb = yc - h/2.0;

    for (int i = 0; i < n; ++i) {
      double x1 = xl;
      double y1 = yb + i*(dy + m) + m/2.0;
      double x2 = x1 + w;
      double y2 = y1 + dy;

      bboxes_.emplace_back(x1, y1, x2, y2);
    }
  }
};

class CQChartsGridPlacer : public CQChartsPlacer {
 public:
  CQChartsGridPlacer() { }

  void place(int n) override {
    bboxes_.clear();

    int nx, ny;

    CQChartsUtil::countToSquareGrid(n, nx, ny);

    double m = margin();

    double xc = center().x;
    double yc = center().y;

    double w = size().width ();
    double h = size().height();

    double dx = w/nx - m;
    double dy = h/ny - m;

    double xl = xc - w/2.0;
    double yb = yc - h/2.0;

    for (int i = 0; i < n; ++i) {
      int ix = i % nx;
      int iy = i / nx;

      double x1 = xl + ix*(dx + m) + m/2.0;
      double y1 = yb + iy*(dy + m) + m/2.0;
      double x2 = x1 + dx;
      double y2 = y1 + dy;

      bboxes_.emplace_back(x1, y1, x2, y2);
    }
  }
};

#endif
