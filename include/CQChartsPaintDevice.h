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

  const CQChartsView *view() const { return view_; }
  void setView(CQChartsView *p) { view_ = p; }

  const CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *p) { plot_ = p; }

  virtual Type type() const = 0;

  virtual bool isInteractive() const { return false; }

  virtual void save() { }
  virtual void restore() { }

  virtual void setClipPath(const QPainterPath &, Qt::ClipOperation=Qt::ReplaceClip) { }
  virtual void setClipRect(const CQChartsGeom::BBox &, Qt::ClipOperation=Qt::ReplaceClip) { }

  virtual CQChartsGeom::BBox clipRect() const { return CQChartsGeom::BBox(); }

  virtual QPen pen() const { return QPen(); }
  virtual void setPen(const QPen &) { }

  virtual QBrush brush() const { return QBrush(); }
  virtual void setBrush(const QBrush &) { }

  virtual void fillPath  (const QPainterPath &, const QBrush &) { }
  virtual void strokePath(const QPainterPath &, const QPen &) { }
  virtual void drawPath  (const QPainterPath &) { }

  virtual void fillRect(const CQChartsGeom::BBox &, const QBrush &) { }
  virtual void drawRect(const CQChartsGeom::BBox &) { }

  virtual void drawEllipse(const CQChartsGeom::BBox &, const CQChartsAngle& =CQChartsAngle()) { }

//virtual void drawArc(const CQChartsGeom::BBox &, const CQChartsAngle &,
//                     const CQChartsAngle &) { }

  virtual void drawPolygon (const CQChartsGeom::Polygon &) { }
  virtual void drawPolyline(const CQChartsGeom::Polygon &) { }

  virtual void drawLine(const CQChartsGeom::Point &, const CQChartsGeom::Point &) { }

  virtual void drawPoint(const CQChartsGeom::Point &) { }

  virtual void drawText(const CQChartsGeom::Point &, const QString &) { }
  virtual void drawTransformedText(const CQChartsGeom::Point &, const QString &) { }

  virtual void drawImage(const CQChartsGeom::Point &, const QImage &) { }
  virtual void drawImageInRect(const CQChartsGeom::BBox &, const CQChartsImage &, bool = true) { }

  virtual const QFont &font() const = 0;
  virtual void setFont(const QFont &f) = 0;

  virtual void setTransformRotate(const CQChartsGeom::Point &p, double angle) = 0;

  virtual const QTransform &transform() const = 0;
  virtual void setTransform(const QTransform &t, bool combine=false) = 0;

  virtual void setRenderHints(QPainter::RenderHints, bool=true) { };

  virtual void setColorNames() { }
  virtual void setColorNames(const QString &, const QString &) { }

  virtual void resetColorNames() { }

  //---

  CQChartsGeom::BBox windowToPixel(const CQChartsGeom::BBox &r) const;
  CQChartsGeom::BBox pixelToWindow(const CQChartsGeom::BBox &r) const;

  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &p) const;
  CQChartsGeom::Point pixelToWindow(const CQChartsGeom::Point &p) const;

  CQChartsGeom::Polygon windowToPixel(const CQChartsGeom::Polygon &p) const;

  QPainterPath windowToPixel(const QPainterPath &p) const;

  CQChartsGeom::Size pixelToWindowSize(const CQChartsGeom::Size &s) const;

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
  void setClipRect(const CQChartsGeom::BBox &bbox, Qt::ClipOperation operation) override;

  CQChartsGeom::BBox clipRect() const override;

  QPen pen() const override;
  void setPen(const QPen &pen) override;

  QBrush brush() const override;
  void setBrush(const QBrush &brush) override;

  void fillPath  (const QPainterPath &path, const QBrush &brush) override;
  void strokePath(const QPainterPath &path, const QPen &pen) override;
  void drawPath  (const QPainterPath &path) override;

  void fillRect(const CQChartsGeom::BBox &bbox, const QBrush &brush) override;
  void drawRect(const CQChartsGeom::BBox &bbox) override;

  void drawEllipse(const CQChartsGeom::BBox &bbox, const CQChartsAngle &a=CQChartsAngle()) override;

//void drawArc(const CQChartsGeom::BBox &rect, const CQChartsAngle &a1,
//             const CQChartsAngle &a2) override;

  void drawPolygon (const CQChartsGeom::Polygon &poly) override;
  void drawPolyline(const CQChartsGeom::Polygon &poly) override;

  void drawLine(const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2) override;

  void drawPoint(const CQChartsGeom::Point &p) override;

  void drawText(const CQChartsGeom::Point &p, const QString &text) override;
  void drawTransformedText(const CQChartsGeom::Point &p, const QString &text) override;

  void drawImage(const CQChartsGeom::Point &, const QImage &) override;
  void drawImageInRect(const CQChartsGeom::BBox &bbox, const CQChartsImage &image,
                       bool stretch=true) override;

  const QFont &font() const override;
  void setFont(const QFont &f) override;

  void setTransformRotate(const CQChartsGeom::Point &p, double angle) override;

  const QTransform &transform() const override;
  void setTransform(const QTransform &t, bool combine=false) override;

  void setRenderHints(QPainter::RenderHints hints, bool on) override;

 private:
  QPainter*          painter_ { nullptr };
  CQChartsGeom::BBox clipRect_;
  QPainterPath       clipPath_;
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

  void setTransformRotate(const CQChartsGeom::Point &p, double angle) override;

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

  void createButton(const CQChartsGeom::BBox &rect, const QString &text, const QString &id,
                    const QString &clickProc);

 protected:
  struct Data {
    QPen                pen;
    QBrush              brush;
    QFont               font;
    bool                hasFont { false };
    QTransform          transform;
    CQChartsGeom::Point transformPoint;
    double              transformAngle { 0.0 };

    void reset() {
      pen            = QPen(Qt::NoPen);
      brush          = QBrush(Qt::NoBrush);
      font           = QFont();
      hasFont        = false;
      transform      = QTransform();
      transformPoint = CQChartsGeom::Point();
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
  void setClipRect(const CQChartsGeom::BBox &bbox, Qt::ClipOperation operation) override;

  void setPen(const QPen &pen) override;

  void setBrush(const QBrush &brush) override;

  void fillPath  (const QPainterPath &path, const QBrush &brush) override;
  void strokePath(const QPainterPath &path, const QPen &pen) override;
  void drawPath  (const QPainterPath &path) override;

  void fillRect(const CQChartsGeom::BBox &bbox, const QBrush &brush) override;
  void drawRect(const CQChartsGeom::BBox &bbox) override;

  void drawEllipse(const CQChartsGeom::BBox &bbox, const CQChartsAngle &a=CQChartsAngle()) override;

//void drawArc(const CQChartsGeom::BBox &rect, const CQChartsAngle &a1,
//             const CQChartsAngle &a2) override;

  void drawPolygon (const CQChartsGeom::Polygon &poly) override;
  void drawPolyline(const CQChartsGeom::Polygon &poly) override;

  void drawLine(const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2) override;

  void drawPoint(const CQChartsGeom::Point &p) override;

  void drawText(const CQChartsGeom::Point &p, const QString &text) override;
  void drawTransformedText(const CQChartsGeom::Point &p, const QString &text) override;

  void drawImage(const CQChartsGeom::Point &, const QImage &) override;
  void drawImageInRect(const CQChartsGeom::BBox &bbox, const CQChartsImage &image,
                       bool stretch=true) override;

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
  void setClipRect(const CQChartsGeom::BBox &bbox, Qt::ClipOperation operation) override;

  void setPen(const QPen &pen) override;

  void setBrush(const QBrush &brush) override;

  void fillPath  (const QPainterPath &path, const QBrush &brush) override;
  void strokePath(const QPainterPath &path, const QPen &pen) override;
  void drawPath  (const QPainterPath &path) override;

  void fillRect(const CQChartsGeom::BBox &bbox, const QBrush &brush) override;
  void drawRect(const CQChartsGeom::BBox &bbox) override;

  void drawEllipse(const CQChartsGeom::BBox &bbox, const CQChartsAngle &a=CQChartsAngle()) override;

//void drawArc(const CQChartsGeom::BBox &rect, const CQChartsAngle &a1,
//             const CQChartsAngle &a2) override;

  void drawPolygon (const CQChartsGeom::Polygon &poly) override;
  void drawPolyline(const CQChartsGeom::Polygon &poly) override;

  void drawLine(const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2) override;

  void drawPoint(const CQChartsGeom::Point &p) override;

  void drawText(const CQChartsGeom::Point &p, const QString &text) override;
  void drawTransformedText(const CQChartsGeom::Point &p, const QString &text) override;

  void drawImage(const CQChartsGeom::Point &, const QImage &) override;
  void drawImageInRect(const CQChartsGeom::BBox &bbox, const CQChartsImage &image,
                       bool stretch=true) override;

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
