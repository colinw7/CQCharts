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

class CQChartsAxisTextPlacer {
 public:
  using PaintDevice = CQChartsPaintDevice;
  using TextOptions = CQChartsTextOptions;
  using Angle       = CQChartsAngle;
  using BBox        = CQChartsGeom::BBox;
  using Point       = CQChartsGeom::Point;

 public:
  //! draw text data
  struct DrawText {
    DrawText(const Point &p, const BBox &bbox, const QString &text) :
     p(p), bbox(bbox), text(text) {
    }

    DrawText(const Point &p, const BBox &bbox, const QString &text,
             const Angle &angle, Qt::Alignment align) :
     p(p), bbox(bbox), text(text), angle(angle), align(align) {
    }

    Point         p;
    BBox          bbox;
    QString       text;
    Angle         angle;
    Qt::Alignment align   { Qt::AlignHCenter };
    bool          visible { true };
  };

  using DrawTexts = std::vector<DrawText>;

 public:
  CQChartsAxisTextPlacer() { }

  void clear();

  bool empty() const { return drawTexts_.empty(); }

  void addDrawText(const DrawText &drawText) {
    drawTexts_.push_back(drawText);
  }

  const DrawTexts &drawTexts() const { return drawTexts_; }

  void autoHide();

  void draw(PaintDevice *device, const TextOptions &textOptions, bool showBoxes=false);

 private:
  DrawTexts drawTexts_;
};

#endif
