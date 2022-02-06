#ifndef CQChartsDrawObj_H
#define CQChartsDrawObj_H

#include <CQChartsGeom.h>
#include <CQChartsPenBrush.h>

class CQChartsPaintDevice;

class CQChartsDrawObj {
 public:
  using DrawObj     = CQChartsDrawObj;
  using PaintDevice = CQChartsPaintDevice;
  using PenBrush    = CQChartsPenBrush;
  using BBox        = CQChartsGeom::BBox;
  using Point       = CQChartsGeom::Point;

 public:
  CQChartsDrawObj();

  virtual ~CQChartsDrawObj();

  DrawObj *parent() const { return parent_; }

  void addChild(DrawObj *obj);

  double maxScale() const { return maxScale_; }
  void setMaxScale(double r) { maxScale_ = r; }

  virtual BBox calcBBox(PaintDevice *device) const;

  void moveScale(double dx, double dy, double s);

  void place(PaintDevice *device, const BBox &placeBBox);

  void draw(PaintDevice *);

  virtual void drawContents(PaintDevice *) { }

 protected:
  using Children = std::vector<DrawObj *>;

  DrawObj*       parent_   { nullptr };
  Children       children_;
  BBox           bbox_;
  PenBrush       penBrush_;
  mutable BBox   sbbox_;
  mutable double scale_    { 1.0 };
  double         maxScale_ { -1.0 };
};

//---

class CQChartsDrawRect : public CQChartsDrawObj {
 public:
  CQChartsDrawRect(const BBox &bbox, const PenBrush &penBrush);

  BBox calcBBox(PaintDevice *device) const override;

  void drawContents(PaintDevice *device) override;
};

//---

class CQChartsDrawText : public CQChartsDrawObj {
 public:
  CQChartsDrawText(const Point &p, const QString &str, const QFont &font,
                   const PenBrush &penBrush);

  BBox calcBBox(PaintDevice *device) const override;

  void drawContents(PaintDevice *device) override;

 protected:
  Point   p_;
  QString str_;
  QFont   font_;
};

#endif
