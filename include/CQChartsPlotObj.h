#ifndef CQChartsPlotObj_H
#define CQChartsPlotObj_H

#include <CQChartsPlot.h>
#include <QObject>
#include <CQChartsGeom.h>
#include <QPen>
#include <QBrush>

class CQChartsPlotObj : public QObject {
  Q_OBJECT

 public:
  CQChartsPlotObj(CQChartsPlot *plot, const CQChartsGeom::BBox &rect=CQChartsGeom::BBox());

  virtual ~CQChartsPlotObj() { }

  const CQChartsGeom::BBox &rect() const { return rect_; }
  void setRect(const CQChartsGeom::BBox &r) { rect_ = r; }

  //---

  const QString &id() const;
  void setId(const QString &s) { id_ = s; }

  virtual QString calcId() const = 0;

  const QString &tipId() const;
  void setTipId(const QString &s) { tipId_ = s; }

  virtual QString calcTipId() const { return calcId(); }

  bool columnId(const QModelIndex &ind, QString &str) const;

  //---

  virtual int colorInd() const { return colorInd_; }
  void setColorInd(int i) { colorInd_ = i; }

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

  //---

  virtual bool visible() const { return isVisible(); }

  //---

  // is point inside (override if not simple rect shape)
  virtual bool inside(const CQChartsGeom::Point &p) const { return rect_.inside(p); }

  // is rect touching (override if not simple rect shape)
  virtual bool touching(const CQChartsGeom::BBox &r) const { return rect_.overlaps(r); }

  virtual bool isIndex(const QModelIndex &) const { return false; }

  virtual void handleResize() { }

  virtual void selectPress() { }

  virtual void addSelectIndex() { }

  virtual void draw(QPainter *, const CQChartsPlot::Layer &) = 0;

 protected:
  using OptString = boost::optional<QString>;

  CQChartsPlot*      plot_     { nullptr }; // parent plot
  CQChartsGeom::BBox rect_;                 // bbox
  OptString          id_;                   // id
  OptString          tipId_;                // tip id
  int                colorInd_ { -1 };      // color index
  bool               visible_  { true };    // is visible
  bool               selected_ { false };   // is selected
  bool               inside_   { false };   // is mouse inside
  QBrush             fill_;                 // fill brush
  QPen               stroke_;               // stroke pen
};

//------

class CQChartsGroupObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsGroupObj(CQChartsPlot *plot);
};

#endif
