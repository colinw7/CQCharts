#ifndef CQChartsPaintDevice_H
#define CQChartsPaintDevice_H

#include <CQChartsPlot.h>
#include <QPainter>

class CQChartsPaintDevice {
 public:
  enum class Type {
    PIXEL,
    PLOT,
    VIEW,
    SCRIPT
  };

 public:
  CQChartsPaintDevice(CQChartsView *view) : view_(view) { }
  CQChartsPaintDevice(CQChartsPlot *plot) : plot_(plot) { }
  CQChartsPaintDevice() { }

  virtual Type type() const = 0;

  virtual void save() { }
  virtual void restore() { }

  virtual void setClipPath(const QPainterPath &, Qt::ClipOperation=Qt::ReplaceClip) { }
  virtual void setClipRect(const QRectF &, Qt::ClipOperation=Qt::ReplaceClip) { }

  virtual QPen pen() const { return QPen(); }
  virtual void setPen(const QPen &) { }

  virtual QBrush brush() const { return QBrush(); }
  virtual void setBrush(const QBrush &) { }

  virtual void fillPath  (const QPainterPath &, const QBrush &) { }
  virtual void strokePath(const QPainterPath &, const QPen &) { }
  virtual void drawPath  (const QPainterPath &) { }

  virtual void fillRect(const QRectF &, const QBrush &) { }
  virtual void drawRect(const QRectF &) { }

  virtual void drawEllipse(const QRectF &) { }
  virtual void drawArc(const QRectF &, double, double) { }

  virtual void drawPolygon (const QPolygonF &) { }
  virtual void drawPolyline(const QPolygonF &) { }

  virtual void drawLine(const QPointF &, const QPointF &) { }

  virtual void drawPoint(const QPointF &) { }

  virtual void drawText(const QPointF &, const QString &) { }
  virtual void drawTransformedText(const QPointF &, const QString &) { }

  virtual void drawImage(const QPointF &, const QImage &) { }
  virtual void drawImageInRect(const QRectF &, const QImage &) { }

  virtual const QFont &font() const = 0;
  virtual void setFont(const QFont &f) = 0;

  virtual void setTransformRotate(const QPointF &p, double angle) = 0;

  virtual const QTransform &transform() const = 0;
  virtual void setTransform(const QTransform &t, bool combine=false) = 0;

  virtual void setRenderHints(QPainter::RenderHints, bool=true) { };

  QRectF  windowToPixel(const QRectF  &r) const;
  QRectF  pixelToWindow(const QRectF  &r) const;
  QPointF windowToPixel(const QPointF &p) const;
  QPointF pixelToWindow(const QPointF &p) const;

  QSizeF pixelToWindowSize(const QSizeF &s) const;

  double lengthPixelWidth (const CQChartsLength &w) const;
  double lengthPixelHeight(const CQChartsLength &h) const;

  double lengthWindowWidth (const CQChartsLength &w) const;
  double lengthWindowHeight(const CQChartsLength &h) const;

  virtual bool invertY() const { return false; }

 protected:
  CQChartsView* view_ { nullptr };
  CQChartsPlot* plot_ { nullptr };
};

//---

class CQChartsViewPlotPainter : public CQChartsPaintDevice {
 public:
  CQChartsViewPlotPainter(CQChartsView *view, QPainter *painter);
  CQChartsViewPlotPainter(CQChartsPlot *plot, QPainter *painter);
  CQChartsViewPlotPainter(QPainter *painter);

  QPainter *painter() const { return painter_; }

  void save   () override;
  void restore() override;

  void setClipPath(const QPainterPath &path, Qt::ClipOperation operation) override;
  void setClipRect(const QRectF &rect, Qt::ClipOperation operation) override;

  QPen pen() const override;
  void setPen(const QPen &pen) override;

  QBrush brush() const override;
  void setBrush(const QBrush &brush) override;

  void fillPath(const QPainterPath &path, const QBrush &brush) override;

  void strokePath(const QPainterPath &path, const QPen &pen) override;

  void drawPath(const QPainterPath &path) override;

  void fillRect(const QRectF &rect, const QBrush &brush) override;

  void drawRect(const QRectF &rect) override;

  void drawEllipse(const QRectF &rect) override;
  void drawArc(const QRectF &rect, double a1, double a2) override;

  void drawPolygon(const QPolygonF &poly) override;
  void drawPolyline(const QPolygonF &poly) override;

  void drawLine(const QPointF &p1, const QPointF &p2) override;

  void drawPoint(const QPointF &p) override;

  void drawText(const QPointF &p, const QString &text) override;
  void drawTransformedText(const QPointF &p, const QString &text) override;

  void drawImage(const QPointF &, const QImage &) override;
  void drawImageInRect(const QRectF &rect, const QImage &image) override;

  const QFont &font() const override;
  void setFont(const QFont &f) override;

  void setTransformRotate(const QPointF &p, double angle) override;

  const QTransform &transform() const override;
  void setTransform(const QTransform &t, bool combine=false) override;

  void setRenderHints(QPainter::RenderHints hints, bool on) override;

 private:
  void mapPath(const QPainterPath &path, QPainterPath &path1);

 private:
  QPainter* painter_ { nullptr };
};

//---

class CQChartsPixelPainter : public CQChartsViewPlotPainter {
 public:
  CQChartsPixelPainter(QPainter *painter);

  Type type() const override { return Type::PIXEL; }

  bool invertY() const override { return true; }
};

//---

class CQChartsViewPainter : public CQChartsViewPlotPainter {
 public:
  CQChartsViewPainter(CQChartsView *view, QPainter *painter);

  Type type() const override { return Type::VIEW; }
};

//---

class CQChartsPlotPainter : public CQChartsViewPlotPainter {
 public:
  CQChartsPlotPainter(CQChartsPlot *plot, QPainter *painter);

  Type type() const override { return Type::PLOT; }
};

//---

class CQChartsScriptPainter : public CQChartsPaintDevice {
 public:
  CQChartsScriptPainter(CQChartsView *view, std::ostream &os);
  CQChartsScriptPainter(CQChartsPlot *plot, std::ostream &os);

  Type type() const override { return Type::SCRIPT; }

  std::ostream &os() const { return *os_; }

  void save   () override;
  void restore() override;

  void setClipPath(const QPainterPath &path, Qt::ClipOperation operation) override;
  void setClipRect(const QRectF &rect, Qt::ClipOperation operation) override;

  QPen pen() const override;

  void setPen(const QPen &pen) override;

  QBrush brush() const override;

  void setBrush(const QBrush &brush) override;

  void fillPath(const QPainterPath &path, const QBrush &brush) override;

  void strokePath(const QPainterPath &path, const QPen &pen) override;

  void drawPath(const QPainterPath &path) override;

  void fillRect(const QRectF &rect, const QBrush &brush) override;

  void drawRect(const QRectF &rect) override;

  void drawEllipse(const QRectF &rect) override;
  void drawArc(const QRectF &rect, double a1, double a2) override;

  void drawPolygon(const QPolygonF &poly) override;
  void drawPolyline(const QPolygonF &poly) override;

  void drawLine(const QPointF &p1, const QPointF &p2) override;

  void drawPoint(const QPointF &p) override;

  void drawText(const QPointF &p, const QString &text) override;
  void drawTransformedText(const QPointF &p, const QString &text) override;

  void drawImage(const QPointF &, const QImage &) override;
  void drawImageInRect(const QRectF &rect, const QImage &) override;

  const QFont &font() const override;
  void setFont(const QFont &f) override;

  void setTransformRotate(const QPointF &p, double angle) override;

  const QTransform &transform() const override;
  void setTransform(const QTransform &t, bool combine=false) override;

  void setRenderHints(QPainter::RenderHints, bool) override;

  std::string context() const;
  void setContext(const std::string &context);

  //---

  static QString encodeString(const QString &str) {
    QString str1;

    int n = str.length();

    for (int i = 0; i < n; ++i) {
      if (str[i] == '\n')
        str1 += "\\n";
      else
        str1 += str[i];
    }

    return str1;
  };

 private:
  void addPathParts(const QPainterPath &path);

 private:
  struct Data {
    QPen       pen;
    QBrush     brush;
    QFont      font;
    QTransform transform;
    QPointF    transformPoint;
    double     transformAngle { 0 };
  };

  using DataStack = std::vector<Data>;

  std::ostream* os_ { nullptr };
  Data          data_;
  DataStack     dataStack_;
  std::string   context_;
};

#endif
