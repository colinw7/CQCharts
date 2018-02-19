#ifndef CQChartsArrow_H
#define CQChartsArrow_H

#include <CQChartsData.h>
#include <QObject>
#include <QPointF>

class CQChartsPlot;
class QPainter;

class CQChartsArrow : public QObject {
 public:
  Q_OBJECT

  Q_PROPERTY(bool           visible     READ isVisible  WRITE setVisible    )
  Q_PROPERTY(QPointF        from        READ from       WRITE setFrom       )
  Q_PROPERTY(QPointF        to          READ to         WRITE setTo         )
  Q_PROPERTY(bool           relative    READ isRelative WRITE setRelative   )
  Q_PROPERTY(CQChartsLength length      READ length     WRITE setLength     )
  Q_PROPERTY(double         angle       READ angle      WRITE setAngle      )
  Q_PROPERTY(double         backAngle   READ backAngle  WRITE setBackAngle  )
  Q_PROPERTY(bool           fhead       READ isFHead    WRITE setFHead      )
  Q_PROPERTY(bool           thead       READ isTHead    WRITE setTHead      )
  Q_PROPERTY(bool           filled      READ isFilled   WRITE setFilled     )
  Q_PROPERTY(bool           stroked     READ isStroked  WRITE setStroked    )
  Q_PROPERTY(bool           empty       READ isEmpty    WRITE setEmpty      )
  Q_PROPERTY(CQChartsLength lineWidth   READ lineWidth  WRITE setLineWidth  )
  Q_PROPERTY(bool           labels      READ hasLabels  WRITE setLabels     )
  Q_PROPERTY(CQChartsColor  fillColor   READ fillColor  WRITE setFillColor  )
  Q_PROPERTY(CQChartsColor  strokeColor READ fillColor  WRITE setStrokeColor)

 public:
  CQChartsArrow(CQChartsPlot *plot, const QPointF &from=QPointF(0,0),
                const QPointF &to=QPointF(1,1));

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

  bool isFilled() const { return data_.fill.visible; }
  void setFilled(bool b) { data_.fill.visible = b; emit dataChanged(); }

  bool isStroked() const { return data_.stroke.visible; }
  void setStroked(bool b) { data_.stroke.visible = b; emit dataChanged(); }

  bool isEmpty() const { return data_.empty; }
  void setEmpty(bool b) { data_.empty = b; emit dataChanged(); }

  const CQChartsLength &lineWidth() const { return data_.stroke.width; }
  void setLineWidth(const CQChartsLength &l) { data_.stroke.width = l; emit dataChanged(); }

  bool hasLabels() const { return data_.labels; }
  void setLabels(bool b) { data_.labels = b; emit dataChanged(); }

  const CQChartsColor &fillColor() const { return data_.fill.color; }
  void setFillColor(const CQChartsColor &v) { data_.fill.color = v; emit dataChanged(); }

  const CQChartsColor &strokeColor() const { return data_.stroke.color; }
  void setStrokeColor(const CQChartsColor &v) { data_.stroke.color = v; emit dataChanged(); }

  const QColor &labelColor() const { return data_.labelColor; }
  void setLabelColor(const QColor &v) { data_.labelColor = v; emit dataChanged(); }

  void setData(const CQChartsArrowData &data) { data_ = data; emit dataChanged(); }

  void draw(QPainter *painter);

 private:
  void drawPolygon(const std::vector<QPointF> &points, double w, bool filled, bool stroked);

  void drawLine(const QPointF &point1, const QPointF &point2, double width, bool mapping);

  void drawPointLabel(const QPointF &point, const QString &text, bool above, bool mapping);

 signals:
  void dataChanged();

 private:
  CQChartsPlot*     plot_    { nullptr };
  bool              visible_ { true };
  QPointF           from_    { 0, 0 };
  QPointF           to_      { 1, 1 };
  CQChartsArrowData data_;
  QPainter*         painter_ { nullptr };
};

#endif
