#ifndef CQChartsStatsPaintDevice_H
#define CQChartsStatsPaintDevice_H

#include <CQChartsPaintDevice.h>
#include <CQChartsQuadTree.h>

/*!
 * \brief Paint Device to output statistics
 * \ingroup Charts
 */
class CQChartsStatsPaintDevice : public CQChartsPaintDevice {
 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 private:
  enum class DrawType {
    NONE,
    PATH,
    RECT,
    ELLIPSE,
    POLYGON,
    POLYLINE,
    LINE,
    POINT,
    IMAGE,
    TEXT
  };

  struct DrawData;

 public:
  CQChartsStatsPaintDevice(View *view);
  CQChartsStatsPaintDevice(Plot *plot);

  Type type() const override { return Type::STATS; }

  const QFont &font() const override { return font_; }
  void setFont(const QFont &font, bool /*scale*/=false) override { font_ = font; }

  void setTransformRotate(const Point & /*p*/, double /*angle*/) override { }

  const QTransform &transform() const override { return transform_; }
  void setTransform(const QTransform &t, bool /*combine*/) override { transform_ = t; }

  void fillPath  (const QPainterPath &, const QBrush &) override;
  void strokePath(const QPainterPath &, const QPen &) override;
  void drawPath(const QPainterPath &path) override;

  void fillRect(const BBox &bbox) override;
  void drawRect(const BBox &bbox) override;

  void drawEllipse(const BBox &bbox, const Angle &a=Angle()) override;

  void drawPolygon (const Polygon &) override;
  void drawPolyline(const Polygon &) override;

  void drawLine(const Point &, const Point &) override;

  void drawPoint(const Point &) override;

  void drawText(const Point &p, const QString &text) override;
  void drawTransformedText(const Point &p, const QString &text) override;

  void drawImage(const Point &, const Image &) override;
  void drawImageInRect(const BBox &, const Image &, bool, const Angle &) override;

  void setPainterFont(const Font &) override { }

  void print(const BBox &bbox) const;

  BBox bbox() const;

 private:
  void addText(const Point &p, const QString &text);

  static void printDrawData(DrawData *drawData);

  static QString typeToString(const DrawType &drawType);

 private:
  struct DrawData {
    DrawType type { DrawType::NONE };
    BBox     bbox;

    DrawData() = default;

    DrawData(const DrawType &type, const BBox &bbox) :
     type(type), bbox(bbox) {
    }

    BBox rect() const { return bbox; }
  };

  using QuadTree  = CQChartsQuadTree<DrawData, BBox>;
  using TypeCount = std::map<DrawType, int>;

  static TypeCount s_typeCount;

  QFont      font_;
  QTransform transform_;

  QuadTree quadTree_;
};

#endif
