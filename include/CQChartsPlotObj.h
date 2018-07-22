#ifndef CQChartsPlotObj_H
#define CQChartsPlotObj_H

#include <CQChartsPlot.h>
#include <QObject>
#include <CQChartsGeom.h>
#include <QPen>
#include <QBrush>

class CQChartsPlotObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::BBox rect     READ rect       WRITE setRect    )
  Q_PROPERTY(QString            id       READ id         WRITE setId      )
  Q_PROPERTY(QString            tipId    READ tipId      WRITE setTipId   )
  Q_PROPERTY(int                colorInd READ colorInd   WRITE setColorInd)
  Q_PROPERTY(bool               visible  READ isVisible  WRITE setVisible )
  Q_PROPERTY(bool               selected READ isSelected WRITE setSelected)
  Q_PROPERTY(bool               inside   READ isInside   WRITE setInside  )
  Q_PROPERTY(QBrush             fill     READ fill       WRITE setFill    )
  Q_PROPERTY(QPen               stroke   READ stroke     WRITE setStroke  )

 public:
  using Indices = std::set<QModelIndex>;

 public:
  CQChartsPlotObj(CQChartsPlot *plot, const CQChartsGeom::BBox &rect=CQChartsGeom::BBox());

  virtual ~CQChartsPlotObj() { }

  CQChartsPlot *plot() const { return plot_; }

  const CQChartsGeom::BBox &rect() const { return rect_; }
  void setRect(const CQChartsGeom::BBox &r) { rect_ = r; }

  //---

  // unique id of object
  const QString &id() const;
  void setId(const QString &s) { id_ = s; }

  // calculate unique id of object (on demand)
  virtual QString calcId() const = 0;

  // tip id for object (string to display in tooltip)
  const QString &tipId() const;
  void setTipId(const QString &s) { tipId_ = s; }

  // calculate tip id (on demand)
  virtual QString calcTipId() const { return calcId(); }

  //---

  // get id from idColumn for index (if defined)
  bool calcColumnId(const QModelIndex &ind, QString &str) const;

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

  virtual void handleResize() { }

  virtual void selectPress() { }

  //---

  bool isSelectIndex(const QModelIndex &ind) const;

  void addSelectIndices();

  virtual void getSelectIndices(Indices &inds) const = 0;

  virtual void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const = 0;

  void addSelectIndex(Indices &inds, int row, const CQChartsColumn &column,
                      const QModelIndex &parent=QModelIndex()) const;
  void addSelectIndex(Indices &inds, const QModelIndex &ind) const;

  //---

  virtual void drawBg(QPainter *) { }
  virtual void drawFg(QPainter *) { }

  virtual void draw(QPainter *) = 0;

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
  CQChartsGroupObj(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox=CQChartsGeom::BBox());
};

#endif
