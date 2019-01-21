#ifndef CQArrow_H
#define CQArrow_H

#include <CQChartsObjData.h>
#include <QObject>
#include <QPointF>

class CQChartsView;
class CQChartsPlot;
class CQCharts;
class QPainter;

class CQChartsArrow : public QObject,
 public CQChartsObjShapeData<CQChartsArrow> {
  Q_OBJECT

  Q_PROPERTY(bool           visible   READ isVisible  WRITE setVisible  )
  Q_PROPERTY(QPointF        from      READ from       WRITE setFrom     )
  Q_PROPERTY(QPointF        to        READ to         WRITE setTo       )
  Q_PROPERTY(bool           relative  READ isRelative WRITE setRelative )
  Q_PROPERTY(CQChartsLength length    READ length     WRITE setLength   )
  Q_PROPERTY(double         angle     READ angle      WRITE setAngle    )
  Q_PROPERTY(double         backAngle READ backAngle  WRITE setBackAngle)
  Q_PROPERTY(bool           fhead     READ isFHead    WRITE setFHead    )
  Q_PROPERTY(bool           thead     READ isTHead    WRITE setTHead    )
  Q_PROPERTY(bool           lineEnds  READ isLineEnds WRITE setLineEnds )
  Q_PROPERTY(CQChartsLength lineWidth READ lineWidth  WRITE setLineWidth)

  CQCHARTS_SHAPE_DATA_PROPERTIES

 public:
  CQChartsArrow(CQChartsView *view, const QPointF &from=QPointF(0,0),
                const QPointF &to=QPointF(1,1));
  CQChartsArrow(CQChartsPlot *plot, const QPointF &from=QPointF(0,0),
                const QPointF &to=QPointF(1,1));

  CQCharts *charts() const;

  CQChartsView* view() const { return view_; }
  CQChartsPlot* plot() const { return plot_; }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; emit dataChanged(); }

  const QPointF &from() const { return from_; }
  void setFrom(const QPointF &v) { from_ = v; emit dataChanged(); }

  const QPointF &to() const { return to_; }
  void setTo(const QPointF &v) { to_ = v; emit dataChanged(); }

  bool isRelative() const { return data_.relative; }
  void setRelative(bool b) { data_.relative = b; emit dataChanged(); }

  const CQChartsLength &length() const { return data_.length; }
  void setLength(const CQChartsLength &l) { data_.length = l; emit dataChanged(); }

  double angle() const { return data_.angle; }
  void setAngle(double r) { data_.angle = r; emit dataChanged(); }

  double backAngle() const { return data_.backAngle; }
  void setBackAngle(double r) { data_.backAngle = r; emit dataChanged(); }

  bool isFHead() const { return data_.fhead; }
  void setFHead(bool b) { data_.fhead = b; emit dataChanged(); }

  bool isTHead() const { return data_.thead; }
  void setTHead(bool b) { data_.thead = b; emit dataChanged(); }

  bool isLineEnds() const { return data_.lineEnds; }
  void setLineEnds(bool b) { data_.lineEnds = b; emit dataChanged(); }

  const CQChartsLength &lineWidth() const { return data_.lineWidth; }
  void setLineWidth(const CQChartsLength &l) { data_.lineWidth = l; emit dataChanged(); }

  void setData(const CQChartsArrowData &data) { data_ = data; emit dataChanged(); }

  void draw(QPainter *painter);

 private:
  void drawPolygon(const std::vector<QPointF> &points, double w, bool filled);

  void drawLine(const QPointF &point1, const QPointF &point2, double width, bool mapping);

#if 0
  void drawPointLabel(const QPointF &point, const QString &text, ool above, bool mapping);
#endif

 signals:
  void dataChanged();

 private:
  CQChartsView*     view_    { nullptr };
  CQChartsPlot*     plot_    { nullptr };
  bool              visible_ { true };
  QPointF           from_    { 0, 0 };
  QPointF           to_      { 1, 1 };
  CQChartsArrowData data_;
  QPainter*         painter_ { nullptr };
};

#endif
