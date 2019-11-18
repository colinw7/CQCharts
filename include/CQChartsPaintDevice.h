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
    SCRIPT,
    SVG
  };

 public:
  CQChartsPaintDevice(CQChartsView *view) : view_(view) { }
  CQChartsPaintDevice(CQChartsPlot *plot) : plot_(plot) { }
  CQChartsPaintDevice() { }

  virtual Type type() const = 0;

  virtual bool isInteractive() const { return false; }

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
//virtual void drawArc(const QRectF &, double, double) { }

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

  virtual void setColorNames() { }
  virtual void setColorNames(const QString &, const QString &) { }

  virtual void resetColorNames() { }

  //---

  QRectF  windowToPixel(const QRectF  &r) const;
  QRectF  pixelToWindow(const QRectF  &r) const;
  QPointF windowToPixel(const QPointF &p) const;
  QPointF pixelToWindow(const QPointF &p) const;

  QPolygonF windowToPixel(const QPolygonF &p) const;

  QPainterPath windowToPixel(const QPainterPath &p) const;

  QSizeF pixelToWindowSize(const QSizeF &s) const;

  double lengthPixelWidth (const CQChartsLength &w) const;
  double lengthPixelHeight(const CQChartsLength &h) const;

  double lengthWindowWidth (const CQChartsLength &w) const;
  double lengthWindowHeight(const CQChartsLength &h) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  bool isInvertX() const;
  bool isInvertY() const;

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

  bool isInteractive() const override { return true; }

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
//void drawArc(const QRectF &rect, double a1, double a2) override;

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

class CQChartsHtmlPainter : public CQChartsPaintDevice {
 public:
  CQChartsHtmlPainter(CQChartsView *view, std::ostream &os);
  CQChartsHtmlPainter(CQChartsPlot *plot, std::ostream &os);

  std::ostream &os() const { return *os_; }

  //---

  QPen pen() const override;

  QBrush brush() const override;

  const QFont &font() const override;

  void resetData();

  void setTransformRotate(const QPointF &p, double angle) override;

  const QTransform &transform() const override;

  void setRenderHints(QPainter::RenderHints, bool) override;

  //---

  void setColorNames() override;
  void setColorNames(const QString &strokeName, const QString &fillName) override;

  void resetColorNames() override;

  const QString &strokeStyleName() const { return strokeStyleName_; }
  void setStrokeStyleName(const QString &v) { strokeStyleName_ = v; }

  const QString &fillStyleName() const { return fillStyleName_; }
  void setFillStyleName(const QString &v) { fillStyleName_ = v; }

  //---

  void createButton(const QRectF &rect, const QString &text, const QString &id,
                    const QString &clickProc);

 protected:
  struct Data {
    QPen       pen;
    QBrush     brush;
    QFont      font;
    bool       hasFont { false };
    QTransform transform;
    QPointF    transformPoint;
    double     transformAngle { 0.0 };

    void reset() {
      pen            = QPen(Qt::NoPen);
      brush          = QBrush(Qt::NoBrush);
      font           = QFont();
      hasFont        = false;
      transform      = QTransform();
      transformPoint = QPointF();
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

//---

class CQChartsScriptPainter : public CQChartsHtmlPainter {
 public:
  CQChartsScriptPainter(CQChartsView *view, std::ostream &os);
  CQChartsScriptPainter(CQChartsPlot *plot, std::ostream &os);

  Type type() const override { return Type::SCRIPT; }

  void save   () override;
  void restore() override;

  void setClipPath(const QPainterPath &path, Qt::ClipOperation operation) override;
  void setClipRect(const QRectF &rect, Qt::ClipOperation operation) override;

  void setPen(const QPen &pen) override;

  void setBrush(const QBrush &brush) override;

  void fillPath(const QPainterPath &path, const QBrush &brush) override;

  void strokePath(const QPainterPath &path, const QPen &pen) override;

  void drawPath(const QPainterPath &path) override;

  void fillRect(const QRectF &rect, const QBrush &brush) override;

  void drawRect(const QRectF &rect) override;

  void drawEllipse(const QRectF &rect) override;
//void drawArc(const QRectF &rect, double a1, double a2) override;

  void drawPolygon(const QPolygonF &poly) override;
  void drawPolyline(const QPolygonF &poly) override;

  void drawLine(const QPointF &p1, const QPointF &p2) override;

  void drawPoint(const QPointF &p) override;

  void drawText(const QPointF &p, const QString &text) override;
  void drawTransformedText(const QPointF &p, const QString &text) override;

  void drawImage(const QPointF &, const QImage &) override;
  void drawImageInRect(const QRectF &rect, const QImage &) override;

  void setFont(const QFont &f) override;

  void setTransform(const QTransform &t, bool combine=false) override;

  std::string context() const;
  void setContext(const std::string &context);

  //---

  static QString encodeObjId(const QString &id) {
    int len = id.length();

    QString id1;

    for (int i = 0; i < len; ++i) {
      QChar c = id[i];

      if (c.isLetterOrNumber())
        id1 += c;
      else
        id1 += '_';
    }

    return id1;
  }

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
  std::string context_;
};

//---

class CQChartsSVGPainter : public CQChartsHtmlPainter {
 public:
  CQChartsSVGPainter(CQChartsView *view, std::ostream &os);
  CQChartsSVGPainter(CQChartsPlot *plot, std::ostream &os);

  Type type() const override { return Type::SVG; }

  void save   () override;
  void restore() override;

  void setClipPath(const QPainterPath &path, Qt::ClipOperation operation) override;
  void setClipRect(const QRectF &rect, Qt::ClipOperation operation) override;

  void setPen(const QPen &pen) override;

  void setBrush(const QBrush &brush) override;

  void fillPath(const QPainterPath &path, const QBrush &brush) override;

  void strokePath(const QPainterPath &path, const QPen &pen) override;

  void drawPath(const QPainterPath &path) override;

  void fillRect(const QRectF &rect, const QBrush &brush) override;

  void drawRect(const QRectF &rect) override;

  void drawEllipse(const QRectF &rect) override;
//void drawArc(const QRectF &rect, double a1, double a2) override;

  void drawPolygon(const QPolygonF &poly) override;
  void drawPolyline(const QPolygonF &poly) override;

  void drawLine(const QPointF &p1, const QPointF &p2) override;

  void drawPoint(const QPointF &p) override;

  void drawText(const QPointF &p, const QString &text) override;
  void drawTransformedText(const QPointF &p, const QString &text) override;

  void drawImage(const QPointF &, const QImage &) override;
  void drawImageInRect(const QRectF &rect, const QImage &) override;

  void setFont(const QFont &f) override;

  void setTransform(const QTransform &t, bool combine=false) override;

  //---

  struct GroupData {
    bool    visible   { true };
    bool    onclick   { false };
    QString clickProc { "clickProc" };
    bool    hasTip    { false };
    QString tipStr;

    GroupData() { }
  };

  void startGroup(const QString &id, const GroupData &groupData=GroupData());
  void endGroup();

  //---

  static QString encodeObjId(const QString &id) {
    int len = id.length();

    QString id1;

    for (int i = 0; i < len; ++i) {
      QChar c = id[i];

      if (c.isLetterOrNumber())
        id1 += c;
      else
        id1 += '_';
    }

    return id1;
  }

  static QString encodeString(const QString &str) {
    QString str1;

    int n = str.length();

    for (int i = 0; i < n; ++i) {
      if      (str[i] == '\n')
        str1 += "\\n";
      else if (str[i] == '\'')
        str1 += "\\\'";
      else if (str[i] == '\"')
        str1 += "\\\"";
      else
        str1 += str[i];
    }

    return str1;
  };

 private:
  void addPathParts(const QPainterPath &path);

  void writePen  () const;
  void writeBrush() const;
  void writeFont () const;
};

#endif
