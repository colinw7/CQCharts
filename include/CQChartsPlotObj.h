#ifndef CQChartsPlotObj_H
#define CQChartsPlotObj_H

#include <QObject>
#include <QPainter>
#include <CBBox2D.h>

class CQChartsPlotObj : public QObject {
  Q_OBJECT

 public:
  CQChartsPlotObj(const CBBox2D &rect=CBBox2D()) :
   rect_(rect) {
  }

  virtual ~CQChartsPlotObj() { }

  const CBBox2D &rect() const { return rect_; }
  void setRect(const CBBox2D &r) { rect_ = r; }

  const QString &id() const { return id_; }
  void setId(const QString &v) { id_ = v; }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

  const QBrush &fill() const { return fill_; }
  void setFill(const QBrush &b) { fill_ = b; }

  const QPen &stroke() const { return stroke_; }
  void setStroke(const QPen &p) { stroke_ = p; }

  virtual bool inside(const CPoint2D &p) const { return rect_.inside(p); }

  virtual void draw(QPainter *) = 0;

 protected:
  CBBox2D rect_;
  QString id_;
  bool    inside_ { false };
  QBrush  fill_;
  QPen    stroke_;
};

#endif
