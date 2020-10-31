#ifndef CQChartsArrow_H
#define CQChartsArrow_H

#define DEBUG_LABELS 1

#include <CQChartsObjData.h>
#include <CQChartsGeom.h>
#include <QObject>

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

  // visible
  Q_PROPERTY(bool visible READ isVisible WRITE setVisible)

  // start/end point
  Q_PROPERTY(CQChartsGeom::Point from     READ from       WRITE setFrom)
  Q_PROPERTY(CQChartsGeom::Point to       READ to         WRITE setTo)
//Q_PROPERTY(bool                relative READ isRelative WRITE setRelative)

  // line width
  Q_PROPERTY(CQChartsLength lineWidth READ lineWidth WRITE setLineWidth)

  // front head
  Q_PROPERTY(bool           frontVisible   READ isFrontVisible  WRITE setFrontVisible)
  Q_PROPERTY(HeadType       frontType      READ frontType       WRITE setFrontType)
  Q_PROPERTY(CQChartsAngle  frontAngle     READ frontAngle      WRITE setFrontAngle)
  Q_PROPERTY(CQChartsAngle  frontBackAngle READ frontBackAngle  WRITE setFrontBackAngle)
  Q_PROPERTY(CQChartsLength frontLength    READ frontLength     WRITE setFrontLength)
  Q_PROPERTY(bool           frontLineEnds  READ isFrontLineEnds WRITE setFrontLineEnds)

  // tail head
  Q_PROPERTY(bool           tailVisible    READ isTailVisible   WRITE setTailVisible)
  Q_PROPERTY(HeadType       tailType       READ tailType        WRITE setTailType)
  Q_PROPERTY(CQChartsAngle  tailAngle      READ tailAngle       WRITE setTailAngle)
  Q_PROPERTY(CQChartsAngle  tailBackAngle  READ tailBackAngle   WRITE setTailBackAngle)
  Q_PROPERTY(CQChartsLength tailLength     READ tailLength      WRITE setTailLength)
  Q_PROPERTY(bool           tailLineEnds   READ isTailLineEnds  WRITE setTailLineEnds)

  // front & tail head
  Q_PROPERTY(CQChartsAngle  angle     READ angle      WRITE setAngle)
  Q_PROPERTY(CQChartsAngle  backAngle READ backAngle  WRITE setBackAngle)
  Q_PROPERTY(CQChartsLength length    READ length     WRITE setLength)
  Q_PROPERTY(bool           lineEnds  READ isLineEnds WRITE setLineEnds)

  // rectilinear
  Q_PROPERTY(bool rectilinear READ isRectilinear WRITE setRectilinear)

#if DEBUG_LABELS
  Q_PROPERTY(bool debugLabels READ isDebugLabels WRITE setDebugLabels)
#endif

  CQCHARTS_SHAPE_DATA_PROPERTIES

  Q_ENUMS(HeadType)

 public:
  enum class HeadType {
    NONE     = int(CQChartsArrowData::HeadType::NONE),
    TRIANGLE = int(CQChartsArrowData::HeadType::TRIANGLE),
    STEALTH  = int(CQChartsArrowData::HeadType::STEALTH),
    DIAMOND  = int(CQChartsArrowData::HeadType::DIAMOND),
    LINE     = int(CQChartsArrowData::HeadType::LINE)
  };

  using View        = CQChartsView;
  using Plot        = CQChartsPlot;
  using Angle       = CQChartsAngle;
  using Length      = CQChartsLength;
  using ArrowData   = CQChartsArrowData;
  using PenBrush    = CQChartsPenBrush;
  using ColorInd    = CQChartsUtil::ColorInd;
  using Point       = CQChartsGeom::Point;
  using GeomPolygon = CQChartsGeom::Polygon;
  using PaintDevice = CQChartsPaintDevice;

 public:
  CQChartsArrow(View *view, const Point &from=Point(0, 0), const Point &to=Point(1, 1));
  CQChartsArrow(Plot *plot, const Point &from=Point(0, 0), const Point &to=Point(1, 1));

  CQCharts *charts() const;

  View *view() const { return view_; }
  Plot *plot() const { return plot_; }

  //---

  // get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; emit dataChanged(); }

  //---

  // get/set from/to point
  const Point &from() const { return from_; }
  void setFrom(const Point &v) { from_ = v; emit dataChanged(); }

  const Point &to() const { return to_; }
  void setTo(const Point &v) { to_ = v; emit dataChanged(); }

//bool isRelative() const { return data_.isRelative(); }
//void setRelative(bool b) { data_.setRelative(b); emit dataChanged(); }

  //---

  // get/set line width
  const Length &lineWidth() const { return data_.lineWidth(); }
  void setLineWidth(const Length &l) { data_.setLineWidth(l); emit dataChanged(); }

  //---

  // get/set front head data
  bool isFrontVisible() const { return data_.isFHead(); }
  void setFrontVisible(bool b) { data_.setFHead(b); emit dataChanged(); }

  HeadType frontType() const { return (HeadType) data_.fheadType(); }
  void setFrontType(const HeadType &type) {
    data_.setFHeadType((ArrowData::HeadType) type); emit dataChanged(); }

  const Angle &frontAngle() const { return data_.frontAngle(); }
  void setFrontAngle(const Angle &a) { data_.setFrontAngle(a); emit dataChanged(); }

  const Angle &frontBackAngle() const { return data_.frontBackAngle(); }
  void setFrontBackAngle(const Angle &a) { data_.setFrontBackAngle(a); emit dataChanged(); }

  const Length &frontLength() const { return data_.frontLength(); }
  void setFrontLength(const Length &l) { data_.setFrontLength(l); emit dataChanged(); }

  bool isFrontLineEnds() const { return data_.isFrontLineEnds(); }
  void setFrontLineEnds(bool b) { data_.setFrontLineEnds(b); emit dataChanged(); }

  //---

  // get/set tail head data
  bool isTailVisible() const { return data_.isTHead(); }
  void setTailVisible(bool b) { data_.setTHead(b); emit dataChanged(); }

  HeadType tailType() const { return (HeadType) data_.theadType(); }
  void setTailType(const HeadType &type) {
    data_.setTHeadType((ArrowData::HeadType) type); emit dataChanged(); }

  const Angle &tailAngle() const { return data_.tailAngle(); }
  void setTailAngle(const Angle &a) { data_.setTailAngle(a); emit dataChanged(); }

  const Angle &tailBackAngle() const { return data_.tailBackAngle(); }
  void setTailBackAngle(const Angle &a) { data_.setTailBackAngle(a); emit dataChanged(); }

  const Length &tailLength() const { return data_.tailLength(); }
  void setTailLength(const Length &l) { data_.setTailLength(l); emit dataChanged(); }

  bool isTailLineEnds() const { return data_.isTailLineEnds(); }
  void setTailLineEnds(bool b) { data_.setTailLineEnds(b); emit dataChanged(); }

  //---

  // get/set tail & head data
  const Angle &angle() const { return data_.angle(); }
  void setAngle(const Angle &a) { data_.setAngle(a); emit dataChanged(); }

  const Angle &backAngle() const { return data_.backAngle(); }
  void setBackAngle(const Angle &a) { data_.setBackAngle(a); emit dataChanged(); }

  const Length &length() const { return data_.length(); }
  void setLength(const Length &l) { data_.setLength(l); emit dataChanged(); }

  bool isLineEnds() const { return data_.isTailLineEnds(); }
  void setLineEnds(bool b) { data_.setLineEnds(b); emit dataChanged(); }

  //---

  //! get/set is rectilinear
  bool isRectilinear() const { return rectilinear_; }
  void setRectilinear(bool b) { rectilinear_ = b; emit dataChanged(); }

  //---

#if DEBUG_LABELS
  bool isDebugLabels() const { return debugLabels_; }
  void setDebugLabels(bool b) { debugLabels_ = b; }
#endif

  //---

  bool isSolid() const;

  //---

  // get/set arrow data
  const ArrowData &data() const { return data_; }
  void setData(const ArrowData &data) { data_ = data; emit dataChanged(); }

  //---

  // draw arrow
  void draw(PaintDevice *device) const;

  void draw(PaintDevice *device, const PenBrush &penBrush) const;

  //---

  // get drawn path
  const QPainterPath &drawnPath() const { return path_; }

  // is point inside arrow
  bool contains(const Point &p) const;

  //---

  // write to script
  void write(std::ostream &os, const QString &varName) const;

  //---

  //! arrow angle data (caches cos/sin)
  struct ArrowAngle {
    double angle { 0.0 };
    double sin   { 0.0 };
    double cos   { 1.0 };

    ArrowAngle() = default;

    ArrowAngle(double angle) :
     angle(angle) {
      init();
    }

    ArrowAngle(const Point &p1, const Point &p2) {
      angle = CQChartsGeom::pointAngle(p1, p2);

      init();
    }

    void init() {
      sin = std::sin(angle);
      cos = std::cos(angle);
    }
  };

  static void pathAddArrows(Plot *plot, const QPainterPath &path,
                            const CQChartsArrowData &arrowData, QPainterPath &arrowPath);

  static void addWidthToPoint(const Point &p, const ArrowAngle &a, double lw,
                              Point &p1, Point &p2);

  static Point movePointOnLine  (const Point &p, const ArrowAngle &a, double d);
  static Point movePointPerpLine(const Point &p, const ArrowAngle &a, double d);

  static bool intersectLine(const Point &l1s, const Point &l1e,
                            const Point &l2s, const Point &l2e, Point &pi, bool &inside);

 private:
  void init();

  void drawContents(const PenBrush &penBrush) const;

  void drawPolygon(const GeomPolygon &points, double w, bool filled, bool stroked,
                   const PenBrush &penBrush) const;

  void drawLine(const Point &point1, const Point &point2, double width,
                const PenBrush &penBrush) const;

#if DEBUG_LABELS
  struct PointLabel {
    PointLabel(const Point &point, const QString &text, bool above) :
     point(point), text(text), above(above) {
    }

    Point   point;
    QString text;
    bool    above { false };
  };

  using PointLabels = std::vector<PointLabel>;

  void drawPointLabel(const Point &point, const QString &text, bool above) const;
#endif

  static double pointLineDistance(const Point &p, const Point &p1, const Point &p2);

 signals:
  void dataChanged();

 private:
  struct Line {
    Point p1;
    Point p2;
    bool  valid { false };

    Line() { }

    Line(const Point &p1, const Point &p2) :
     p1(p1), p2(p2), valid(true) {
    }

    void reset() { valid = false; }

    double distance(const Point &p) const {
      return CQChartsArrow::pointLineDistance(p, p1, p2);
    }
  };

  struct Polygon {
    GeomPolygon points;
    bool        valid { false };

    Polygon() { }

    Polygon(const GeomPolygon &points) :
     points(points), valid(true) {
    }

    void reset() { valid = false; }
  };

  //---

  View*                view_        { nullptr }; //!< parent view
  Plot*                plot_        { nullptr }; //!< parent plot
  bool                 visible_     { true };    //!< is visible
  Point                from_        { 0, 0 };    //!< start point
  Point                to_          { 1, 1 };    //!< end point
  ArrowData            data_;                    //!< arrow data
  bool                 rectilinear_ { false };
#if DEBUG_LABELS
  bool                 debugLabels_ { false };
  mutable PointLabels  pointLabels_;
#endif
  mutable PaintDevice* device_  { nullptr }; //!< paint device
  mutable QPainterPath path_;                //!< draw path

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
