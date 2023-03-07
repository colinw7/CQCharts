#ifndef CQChartsArrow_H
#define CQChartsArrow_H

//#define DEBUG_LABELS 1

#include <CQChartsObjData.h>
#include <CQChartsArrowData.h>
#include <CQChartsGeom.h>

#include <QObject>
#include <QPointer>

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

  // mid head
  Q_PROPERTY(bool           midVisible    READ isMidVisible   WRITE setMidVisible)
  Q_PROPERTY(HeadType       midType       READ midType        WRITE setMidType)
  Q_PROPERTY(CQChartsAngle  midAngle      READ midAngle       WRITE setMidAngle)
  Q_PROPERTY(CQChartsAngle  midBackAngle  READ midBackAngle   WRITE setMidBackAngle)
  Q_PROPERTY(CQChartsLength midLength     READ midLength      WRITE setMidLength)
  Q_PROPERTY(bool           midLineEnds   READ isMidLineEnds  WRITE setMidLineEnds)

  // front, tail and mid head
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
  using ArrowData = CQChartsArrowData;

  enum class HeadType {
    NONE     = int(ArrowData::HeadType::NONE),
    ARROW    = int(ArrowData::HeadType::ARROW),
    TRIANGLE = int(ArrowData::HeadType::TRIANGLE),
    STEALTH  = int(ArrowData::HeadType::STEALTH),
    DIAMOND  = int(ArrowData::HeadType::DIAMOND),
    LINE     = int(ArrowData::HeadType::LINE)
  };

  using View        = CQChartsView;
  using Plot        = CQChartsPlot;
  using Angle       = CQChartsAngle;
  using Length      = CQChartsLength;
  using PenBrush    = CQChartsPenBrush;
  using ColorInd    = CQChartsUtil::ColorInd;
  using Point       = CQChartsGeom::Point;
  using BBox        = CQChartsGeom::BBox;
  using GeomPolygon = CQChartsGeom::Polygon;
  using PaintDevice = CQChartsPaintDevice;

 public:
  CQChartsArrow(View *view, const Point &from=Point(0, 0), const Point &to=Point(1, 1));
  CQChartsArrow(Plot *plot, const Point &from=Point(0, 0), const Point &to=Point(1, 1));

  CQCharts *charts() const;

  View *view() const;
  Plot *plot() const;

  //---

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b);

  //---

  //! get/set from/to point
  const Point &from() const { return from_; }
  void setFrom(const Point &v);

  const Point &to() const { return to_; }
  void setTo(const Point &v);

//bool isRelative() const { return data_.isRelative(); }
//void setRelative(bool b);

  //---

  //! get/set line width
  const Length &lineWidth() const { return data_.lineWidth(); }
  void setLineWidth(const Length &l);

  //---

  //! get/set front head data
  bool isFrontVisible() const { return data_.isFHead(); }
  void setFrontVisible(bool b);

  HeadType frontType() const { return static_cast<HeadType>(data_.fheadType()); }
  void setFrontType(const HeadType &type);

  const Angle &frontAngle() const { return data_.frontAngle(); }
  void setFrontAngle(const Angle &a);

  const Angle &frontBackAngle() const { return data_.frontBackAngle(); }
  void setFrontBackAngle(const Angle &a);

  const Length &frontLength() const { return data_.frontLength(); }
  void setFrontLength(const Length &l);

  bool isFrontLineEnds() const { return data_.isFrontLineEnds(); }
  void setFrontLineEnds(bool b);

  //---

  //! get/set tail head data
  bool isTailVisible() const { return data_.isTHead(); }
  void setTailVisible(bool b);

  HeadType tailType() const { return static_cast<HeadType>(data_.theadType()); }
  void setTailType(const HeadType &type);

  const Angle &tailAngle() const { return data_.tailAngle(); }
  void setTailAngle(const Angle &a);

  const Angle &tailBackAngle() const { return data_.tailBackAngle(); }
  void setTailBackAngle(const Angle &a);

  const Length &tailLength() const { return data_.tailLength(); }
  void setTailLength(const Length &l);

  bool isTailLineEnds() const { return data_.isTailLineEnds(); }
  void setTailLineEnds(bool b);

  //---

  //! get/set mid head data
  bool isMidVisible() const { return data_.isMidHead(); }
  void setMidVisible(bool b);

  HeadType midType() const { return static_cast<HeadType>(data_.midHeadType()); }
  void setMidType(const HeadType &type);

  const Angle &midAngle() const { return data_.midAngle(); }
  void setMidAngle(const Angle &a);

  const Angle &midBackAngle() const { return data_.midBackAngle(); }
  void setMidBackAngle(const Angle &a);

  const Length &midLength() const { return data_.midLength(); }
  void setMidLength(const Length &l);

  bool isMidLineEnds() const { return data_.isMidLineEnds(); }
  void setMidLineEnds(bool b);

  //---

  //! get/set tail, mid and head data
  const Angle &angle() const { return data_.angle(); }
  void setAngle(const Angle &a);

  const Angle &backAngle() const { return data_.backAngle(); }
  void setBackAngle(const Angle &a);

  const Length &length() const { return data_.length(); }
  void setLength(const Length &l);

  bool isLineEnds() const { return data_.isTailLineEnds(); }
  void setLineEnds(bool b);

  //---

  //! get/set is rectilinear
  bool isRectilinear() const { return rectilinear_; }
  void setRectilinear(bool b);

  //---

#if DEBUG_LABELS
  bool isDebugLabels() const { return debugLabels_; }
  void setDebugLabels(bool b) { debugLabels_ = b; }
#endif

  //---

  bool isSolid() const;

  //---

  //! get/set arrow data
  const ArrowData &data() const { return data_; }
  void setData(const ArrowData &data);

  //---

  void connectDisconnectDataChanged(bool b, const QObject *obj, const char *slotName) const;

  //---

  //! draw arrow
  void draw(PaintDevice *device) const;

  void draw(PaintDevice *device, const PenBrush &penBrush) const;

  //---

  //! get drawn path
  const QPainterPath &drawnPath() const { return drawData_.path; }

  //! is point inside arrow
  bool contains(const Point &p) const;

  //---

  //! write to script
  void write(std::ostream &os, const QString &varName) const;

  //---

 public:
  //! arrow angle data (caches cos/sin)
  struct ArrowAngle {
    double angle { 0.0 }; //!< radians
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

  static void drawArrow(PaintDevice *device, const Point &from, const Point &to,
                        const ArrowData &data, const Length &strokeWidth, bool rectilinear,
                        const PenBrush &penBrush);

  static void selfPath(PaintDevice *device, QPainterPath &path, const BBox &rect,
                       bool fhead, bool thead, double lw);

  static void pathAddArrows(PaintDevice *device, const QPainterPath &path,
                            const ArrowData &arrowData, double lw, const Length &arrowLen,
                            QPainterPath &arrowPath);
  static void pathAddArrows(PaintDevice *device, const QPainterPath &path,
                            const ArrowData &arrowData, double lw,
                            const Length &frontLen, const Length &tailLen,
                            QPainterPath &arrowPath);

  static void addWidthToPoint(const Point &p, const ArrowAngle &a, double lw,
                              Point &p1, Point &p2);

  static Point movePointOnLine  (const Point &p, const ArrowAngle &a, double d);
  static Point movePointPerpLine(const Point &p, const ArrowAngle &a, double d);

  static bool intersectLine(const Point &l1s, const Point &l1e,
                            const Point &l2s, const Point &l2e, Point &pi, bool &inside);

 private:
  //! line
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

  //! polygon
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
#endif

  //! draw data
  struct DrawData {
    Line    frontLine1;
    Line    frontLine2;
    Line    endLine1;
    Line    endLine2;
    Line    midLine;
    Polygon frontPoly;
    Polygon tailPoly;
    Polygon midPoly;
    Polygon arrowPoly;

    QPainterPath path; //!< draw path

#if DEBUG_LABELS
    PointLabels pointLabels;
    bool        debugLabels { false };
#endif

    void reset() {
      frontLine1.reset();
      frontLine2.reset();
      endLine1  .reset();
      endLine2  .reset();
      midLine   .reset();
      frontPoly .reset();
      tailPoly  .reset();
      midPoly   .reset();
      arrowPoly .reset();

      path = QPainterPath();

#if DEBUG_LABELS
      pointLabels.clear();
#endif
    }
  };

 private:
  struct GenHeadData {
    double      len        { 0.0 };
    ArrowAngle  angle;
    ArrowAngle  backAngle;
    double      lineLen    { 0.0 };
    bool        isLineEnds { false };
    bool        isPoints   { false };
    Point       headMid;
    Point       headMid1, headMid2;
    GeomPolygon headPoints1;
    GeomPolygon headPoints2;
    Point       tipPoint1, tipPoint2;
    Point       backLine1, backLine2;
  };

 private:
  void init();

  static void drawContents(PaintDevice *device, const Point &from, const Point &to,
                           const ArrowData &data, const Length &strokeWidth, bool rectilinear,
                           const PenBrush &penBrush, DrawData &drawData);

  static void drawPolygon(PaintDevice *device, const GeomPolygon &points, double w,
                          bool filled, bool stroked, const PenBrush &penBrush,
                          QPainterPath &path);

  static void drawLine(PaintDevice *device, const Point &point1, const Point &point2,
                       double width, const PenBrush &penBrush);

  static double pointLineDistance(const Point &p, const Point &p1, const Point &p2);

  static void calcHeadPolyData(const Point &startPoint, const Point &startPointI,
                               const ArrowAngle &lineAngle, bool linePoly, double linePixelWidth,
                               GenHeadData &frontData, const GenHeadData &tailData,
                               DrawData &drawData);
  static void calcTailPolyData(const Point &endPoint, const Point &endPointI,
                               const ArrowAngle &lineAngle, bool linePoly, double linePixelWidth,
                               const GenHeadData &frontData, GenHeadData &tailData,
                               DrawData &drawData);
  static void calcMidPolyData(const Point &startPoint, const Point &endPoint,
                              const ArrowAngle &lineAngle, bool linePoly, bool swapped,
                              double linePixelWidth, const Point &headMidR1, const Point &headMidR2,
                              const GenHeadData &frontData, const GenHeadData &tailData,
                              GenHeadData &midData, DrawData &drawData);

  static void updateFrontLinePoly(GenHeadData &frontData, const GenHeadData &tailData);
  static void updateTailLinePoly (const GenHeadData &frontData, GenHeadData &tailData);
  static void updateMidLinePoly  (GenHeadData &midData, const GenHeadData &frontData,
                                  const GenHeadData &tailData);

 Q_SIGNALS:
  void dataChanged();

 private:
  using ViewP = QPointer<View>;
  using PlotP = QPointer<Plot>;

  ViewP view_; //!< parent view
  PlotP plot_; //!< parent plot

  bool visible_ { true }; //!< is visible

  Point from_ { 0, 0 }; //!< start point
  Point to_   { 1, 1 }; //!< end point

  ArrowData data_; //!< arrow data

  bool rectilinear_ { false };

#if DEBUG_LABELS
  bool debugLabels_ { false };
#endif

  mutable DrawData drawData_; //!< draw data
};

#endif
