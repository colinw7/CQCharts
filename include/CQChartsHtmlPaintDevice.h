#ifndef CQChartsHtmlPaintDevice_H
#define CQChartsHtmlPaintDevice_H

#include <CQChartsPaintDevice.h>

/*!
 * \brief Paint Device to output Html Web Page
 * \ingroup Charts
 */
class CQChartsHtmlPaintDevice : public CQChartsPaintDevice {
 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQChartsHtmlPaintDevice(View *view, std::ostream &os);
  CQChartsHtmlPaintDevice(Plot *plot, std::ostream &os);

  std::ostream &os() const { return *os_; }

  //---

  QPen pen() const override;

  QBrush brush() const override;

  const QFont &font() const override;

  void resetData();

  void setTransformRotate(const Point &p, double angle) override;

  const QTransform &transform() const override;

  void setRenderHints(QPainter::RenderHints, bool) override;

  //---

  void setColorNames() override;
  void setColorNames(const QString &strokeName, const QString &fillName) override;

  void resetColorNames() override;

  const QString &strokeStyleName() const { return strokeStyleName_; }
  void setStrokeStyleName(const QString &s) { strokeStyleName_ = s; }

  const QString &fillStyleName() const { return fillStyleName_; }
  void setFillStyleName(const QString &s) { fillStyleName_ = s; }

  //---

  void createButton(const BBox &rect, const QString &text, const QString &id,
                    const QString &clickProc);

 protected:
  struct Data {
    QPen       pen;
    QBrush     brush;
    QFont      font;
    bool       hasFont { false };
    QTransform transform;
    Point      transformPoint;
    double     transformAngle { 0.0 };

    void reset() {
      pen            = QPen(Qt::NoPen);
      brush          = QBrush(Qt::NoBrush);
      font           = QFont();
      hasFont        = false;
      transform      = QTransform();
      transformPoint = Point();
      transformAngle = 0.0;
    }
  };

  using DataStack = std::vector<Data>;

  std::ostream* os_ { nullptr };
  Data          data_;
  DataStack     dataStack_;
  QString       strokeStyleName_;
  QString       fillStyleName_;
};

#endif
