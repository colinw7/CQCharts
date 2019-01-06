#ifndef CQChartsPlotObj_H
#define CQChartsPlotObj_H

#include <CQChartsPlot.h>
#include <CQChartsObj.h>
#include <CQChartsGeom.h>
#include <QPen>
#include <QBrush>

class CQChartsPlotObj : public CQChartsObj {
  Q_OBJECT

  Q_PROPERTY(QString typeName READ typeName)
  Q_PROPERTY(int     colorInd READ colorInd   WRITE setColorInd)
  Q_PROPERTY(bool    visible  READ isVisible  WRITE setVisible )
  Q_PROPERTY(QBrush  fill     READ fill       WRITE setFill    )
  Q_PROPERTY(QPen    stroke   READ stroke     WRITE setStroke  )

 public:
  using Indices = std::set<QModelIndex>;

 public:
  CQChartsPlotObj(CQChartsPlot *plot, const CQChartsGeom::BBox &rect=CQChartsGeom::BBox());

  virtual ~CQChartsPlotObj() { }

  //---

  CQChartsPlot *plot() const { return plot_; }

  virtual QString typeName() const { return ""; }

  //---

  // get id from idColumn for index (if defined)
  bool calcColumnId(const QModelIndex &ind, QString &str) const;

  //---

  virtual int colorInd() const { return colorInd_; }
  void setColorInd(int i) { colorInd_ = i; }

  //---

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

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

  //virtual void postResize() { }

  virtual void selectPress() { }

  //---

  bool isSelectIndex(const QModelIndex &ind) const;

  void addSelectIndices();

  void getHierSelectIndices(Indices &inds) const;

  virtual void getSelectIndices(Indices &inds) const = 0;

  virtual void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const = 0;

  void addSelectIndex(Indices &inds, const CQChartsModelIndex &ind) const;
  void addSelectIndex(Indices &inds, int row, const CQChartsColumn &column,
                      const QModelIndex &parent=QModelIndex()) const;
  void addSelectIndex(Indices &inds, const QModelIndex &ind) const;

  //---

  virtual void drawBg(QPainter *) const { }
  virtual void drawFg(QPainter *) const { }

  virtual void draw(QPainter *) = 0;

 protected:
  CQChartsPlot* plot_     { nullptr }; // parent plot
  int           colorInd_ { -1 };      // color index
  bool          visible_  { true };    // is visible
  QBrush        fill_;                 // fill brush
  QPen          stroke_;               // stroke pen
};

//------

class CQChartsGroupObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsGroupObj(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox=CQChartsGeom::BBox());
};

#endif
