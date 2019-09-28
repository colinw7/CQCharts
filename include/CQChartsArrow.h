#ifndef CQChartsArrow_H
#define CQChartsArrow_H

#define DEBUG_LABELS 1

#include <CQChartsObjData.h>
#include <QObject>
#include <QPointF>
#include <QPolygonF>

class CQChartsView;
class CQChartsPlot;
class CQCharts;
class CQChartsPaintDevice;

/*!
 * \brief class for arrow object
 * \ingroup Charts
 */
class CQChartsArrow : public QObject,
 public CQChartsObjShapeData<CQChartsArrow> {
  Q_OBJECT

  Q_PROPERTY(bool           visible        READ isVisible       WRITE setVisible       )
  Q_PROPERTY(QPointF        from           READ from            WRITE setFrom          )
  Q_PROPERTY(QPointF        to             READ to              WRITE setTo            )

//Q_PROPERTY(bool           relative       READ isRelative      WRITE setRelative      )
  Q_PROPERTY(CQChartsLength lineWidth      READ lineWidth       WRITE setLineWidth     )

  Q_PROPERTY(bool           fhead          READ isFHead         WRITE setFHead         )
  Q_PROPERTY(bool           thead          READ isTHead         WRITE setTHead         )
  Q_PROPERTY(double         angle          READ angle           WRITE setAngle         )
  Q_PROPERTY(double         frontAngle     READ frontAngle      WRITE setFrontAngle    )
  Q_PROPERTY(double         tailAngle      READ angle           WRITE setTailAngle     )
  Q_PROPERTY(double         backAngle      READ backAngle       WRITE setBackAngle     )
  Q_PROPERTY(double         frontBackAngle READ frontBackAngle  WRITE setFrontBackAngle)
  Q_PROPERTY(double         tailBackAngle  READ tailBackAngle   WRITE setTailBackAngle )
  Q_PROPERTY(CQChartsLength length         READ length          WRITE setLength        )
  Q_PROPERTY(CQChartsLength frontLength    READ frontLength     WRITE setFrontLength   )
  Q_PROPERTY(CQChartsLength tailLength     READ tailLength      WRITE setTailLength    )
  Q_PROPERTY(bool           lineEnds       READ isLineEnds      WRITE setLineEnds      )
  Q_PROPERTY(bool           frontLineEnds  READ isFrontLineEnds WRITE setFrontLineEnds )
  Q_PROPERTY(bool           tailLineEnds   READ isTailLineEnds  WRITE setTailLineEnds  )

#if DEBUG_LABELS
  Q_PROPERTY(bool           debugLabels    READ isDebugLabels   WRITE setDebugLabels   )
#endif

  CQCHARTS_SHAPE_DATA_PROPERTIES

 public:
  using ColorInd = CQChartsUtil::ColorInd;

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

//bool isRelative() const { return data_.isRelative(); }
//void setRelative(bool b) { data_.setRelative(b); emit dataChanged(); }

  const CQChartsLength &lineWidth() const { return data_.lineWidth(); }
  void setLineWidth(const CQChartsLength &l) { data_.setLineWidth(l); emit dataChanged(); }

  //---

  bool isFHead() const { return data_.isFHead(); }
  void setFHead(bool b) { data_.setFHead(b); emit dataChanged(); }

  bool isTHead() const { return data_.isTHead(); }
  void setTHead(bool b) { data_.setTHead(b); emit dataChanged(); }

  //---

  double angle() const { return data_.angle(); }
  void setAngle(double r) { data_.setAngle(r); emit dataChanged(); }

  double frontAngle() const { return data_.frontAngle(); }
  void setFrontAngle(double r) { data_.setFrontAngle(r); emit dataChanged(); }

  double tailAngle() const { return data_.tailAngle(); }
  void setTailAngle(double r) { data_.setTailAngle(r); emit dataChanged(); }

  //---

  double backAngle() const { return data_.backAngle(); }
  void setBackAngle(double r) { data_.setBackAngle(r); emit dataChanged(); }

  double frontBackAngle() const { return data_.frontBackAngle(); }
  void setFrontBackAngle(double r) { data_.setFrontBackAngle(r); emit dataChanged(); }

  double tailBackAngle() const { return data_.tailBackAngle(); }
  void setTailBackAngle(double r) { data_.setTailBackAngle(r); emit dataChanged(); }

  //---

  const CQChartsLength &length() const { return data_.length(); }
  void setLength(const CQChartsLength &l) { data_.setLength(l); emit dataChanged(); }

  const CQChartsLength &frontLength() const { return data_.frontLength(); }
  void setFrontLength(const CQChartsLength &l) { data_.setFrontLength(l); emit dataChanged(); }

  const CQChartsLength &tailLength() const { return data_.tailLength(); }
  void setTailLength(const CQChartsLength &l) { data_.setTailLength(l); emit dataChanged(); }

  //---

  bool isLineEnds() const { return data_.isTailLineEnds(); }
  void setLineEnds(bool b) { data_.setLineEnds(b); emit dataChanged(); }

  bool isFrontLineEnds() const { return data_.isFrontLineEnds(); }
  void setFrontLineEnds(bool b) { data_.setFrontLineEnds(b); emit dataChanged(); }

  bool isTailLineEnds() const { return data_.isTailLineEnds(); }
  void setTailLineEnds(bool b) { data_.setTailLineEnds(b); emit dataChanged(); }

  //---

#if DEBUG_LABELS
  bool isDebugLabels() const { return debugLabels_; }
  void setDebugLabels(bool b) { debugLabels_ = b; }
#endif

  //---

  const CQChartsArrowData &data() const { return data_; }
  void setData(const CQChartsArrowData &data) { data_ = data; emit dataChanged(); }

  //---

  void draw(CQChartsPaintDevice *device) const;

  void draw(CQChartsPaintDevice *device, const QPen &pen, const QBrush &brush) const;

  //---

  bool contains(const QPointF &p) const;

  //---

  void write(std::ostream &os, const QString &varName) const;

 private:
  using Points = QVector<QPointF>;

 private:
  void init();

  void drawContents(const QPen &pen, const QBrush &brush) const;

  void drawPolygon(const Points &points, double w, bool filled, bool stroked,
                   const QPen &pen, const QBrush &brush) const;

  void drawLine(const QPointF &point1, const QPointF &point2, double width,
                const QPen &pen, const QBrush &brush) const;

#if DEBUG_LABELS
  struct PointLabel {
    PointLabel(const QPointF &point, const QString &text, bool above) :
     point(point), text(text), above(above) {
    }

    QPointF point;
    QString text;
    bool    above { false };
  };

  using PointLabels = std::vector<PointLabel>;

  void drawPointLabel(const QPointF &point, const QString &text, bool above) const;
#endif

  static double pointLineDistance(const QPointF &p, const QPointF &p1, const QPointF &p2);

 signals:
  void dataChanged();

 private:
  struct Line {
    QPointF p1;
    QPointF p2;
    bool    valid { false };

    Line() { }

    Line(const QPointF &p1, const QPointF &p2) :
     p1(p1), p2(p2), valid(true) {
    }

    void reset() { valid = false; }

    double distance(const QPointF &p) const {
      return CQChartsArrow::pointLineDistance(p, p1, p2);
    }
  };

  struct Polygon {
    QPolygonF points;
    bool      valid { false };

    Polygon() { }

    Polygon(const Points &points) :
     points(points), valid(true) {
    }

    void reset() { valid = false; }
  };

  //---

  CQChartsView*                view_        { nullptr }; //!< parent view
  CQChartsPlot*                plot_        { nullptr }; //!< parent plot
  bool                         visible_     { true };    //!< is visible
  QPointF                      from_        { 0, 0 };    //!< start point
  QPointF                      to_          { 1, 1 };    //!< end point
  CQChartsArrowData            data_;                    //!< arrow data
  mutable CQChartsPaintDevice* device_;                  //!< paint device
#if DEBUG_LABELS
  bool                         debugLabels_ { false };
#endif

  // inside data
  mutable Line    frontLine1_;
  mutable Line    frontLine2_;
  mutable Line    endLine1_;
  mutable Line    endLine2_;
  mutable Line    midLine_;
  mutable Polygon frontPoly_;
  mutable Polygon tailPoly_;
  mutable Polygon arrowPoly_;
};

#endif
