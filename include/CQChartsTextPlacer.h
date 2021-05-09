#ifndef CQChartsTextPlacer_H
#define CQChartsTextPlacer_H

#include <CQChartsRectPlacer.h>
#include <CQChartsTextOptions.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsUtil.h>

class CQChartsTextPlacer {
 public:
  using Alpha       = CQChartsAlpha;
  using BBox        = CQChartsGeom::BBox;
  using Point       = CQChartsGeom::Point;
  using PaintDevice = CQChartsPaintDevice;

 public:
  //! draw text data
  struct DrawText : public CQChartsRectPlacer::RectData {
    using TextOptions = CQChartsTextOptions;
    using Rect        = CQChartsRectPlacer::Rect;

    DrawText(const QString &str, const Point &point, const TextOptions &options,
             const QColor &color, const Alpha &alpha, const Point &targetPoint);

    void setBBox(const BBox &bbox);

    const Rect &rect() const override;

    void setRect(const Rect &r) override;

    std::string name() const override;

    QString     str;
    Point       point;
    Point       origPoint;
    TextOptions options;
    QColor      color;
    Alpha       alpha;
    Rect        textRect;
    Rect        origRect;
    Point       targetPoint;
  };

 public:
  CQChartsTextPlacer();
 ~CQChartsTextPlacer();

  void setDebug(bool b);

  void addDrawText(DrawText *drawText);

  void place(const BBox &bbox);

  void draw(PaintDevice *device);

  void clear();

 private:
  using DrawTexts = std::vector<DrawText *>;

  DrawTexts          drawTexts_;
  CQChartsRectPlacer placer_;
};

#endif
