#ifndef CQChartsPlotObj_H
#define CQChartsPlotObj_H

#include <CQChartsPlot.h>
#include <QObject>
#include <QPainter>
#include <CQChartsGeom.h>

class CQChartsPlotObj : public QObject {
  Q_OBJECT

 public:
  CQChartsPlotObj(const CQChartsGeom::BBox &rect=CQChartsGeom::BBox()) :
   rect_(rect) {
  }

  virtual ~CQChartsPlotObj() { }

  const CQChartsGeom::BBox &rect() const { return rect_; }
  void setRect(const CQChartsGeom::BBox &r) { rect_ = r; }

  //---

  const QString &id() const {
    if (! id_)
      const_cast<CQChartsPlotObj*>(this)->id_ = calcId();

    return *id_;
  }

  void setId(const QString &s) { id_ = s; }

  virtual QString calcId() const = 0;

  virtual QString calcTipId() const { return calcId(); }

  virtual const QString &tipId() const {
    if (! tipId_)
      const_cast<CQChartsPlotObj*>(this)->tipId_ = calcTipId();

    return *tipId_;
  }

  //---

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

  const QBrush &fill() const { return fill_; }
  void setFill(const QBrush &b) { fill_ = b; }

  const QPen &stroke() const { return stroke_; }
  void setStroke(const QPen &p) { stroke_ = p; }

  virtual bool visible() const { return isVisible(); }

  virtual bool inside(const CQChartsGeom::Point &p) const { return rect_.inside(p); }

  virtual bool isIndex(const QModelIndex &) const { return false; }

  virtual void handleResize() { }

  virtual void mousePress(const CQChartsGeom::Point &) { }

  virtual void clickZoom(const CQChartsGeom::Point &) { }

  virtual void draw(QPainter *, const CQChartsPlot::Layer &) = 0;

 protected:
  using OptString = boost::optional<QString>;

  CQChartsGeom::BBox rect_;
  OptString          id_;
  OptString          tipId_;
  bool               visible_  { true };
  bool               selected_ { false };
  bool               inside_   { false };
  QBrush             fill_;
  QPen               stroke_;
};

#endif
