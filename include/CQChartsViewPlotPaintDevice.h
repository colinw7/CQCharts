#ifndef CQChartsViewPlotPaintDevice_H
#define CQChartsViewPlotPaintDevice_H

#include <CQChartsPaintDevice.h>
#include <CQChartsPlot.h>

class CQChartsViewPlotPaintDevice : public CQChartsPaintDevice {
 public:
  CQChartsViewPlotPaintDevice(CQChartsView *view, QPainter *painter);
  CQChartsViewPlotPaintDevice(CQChartsPlot *plot, QPainter *painter);
  CQChartsViewPlotPaintDevice(QPainter *painter);

  bool isInteractive() const override { return true; }

  QPainter *painter() const { return painter_; }

  void save   () override;
  void restore() override;

  void setClipPath(const QPainterPath &path, Qt::ClipOperation operation) override;
  void setClipRect(const BBox &bbox, Qt::ClipOperation operation) override;

  BBox clipRect() const override;

  QPen pen() const override;
  void setPen(const QPen &pen) override;

  QBrush brush() const override;
  void setBrush(const QBrush &brush) override;

  void fillPath  (const QPainterPath &path, const QBrush &brush) override;
  void strokePath(const QPainterPath &path, const QPen &pen) override;
  void drawPath  (const QPainterPath &path) override;

  void fillRect(const BBox &bbox, const QBrush &brush) override;
  void drawRect(const BBox &bbox) override;

  void drawEllipse(const BBox &bbox, const CQChartsAngle &a=CQChartsAngle()) override;

//void drawArc(const BBox &rect, const CQChartsAngle &a1, CQChartsAngle &a2) override;

  void drawPolygon (const Polygon &poly) override;
  void drawPolyline(const Polygon &poly) override;

  void drawLine(const Point &p1, const Point &p2) override;

  void drawPoint(const Point &p) override;

  void drawText(const Point &p, const QString &text) override;
  void drawTransformedText(const Point &p, const QString &text) override;

  void drawImage(const Point &, const QImage &) override;
  void drawImageInRect(const BBox &bbox, const CQChartsImage &image, bool stretch=true) override;

  const QFont &font() const override;
  void setFont(const QFont &f) override;

  void setTransformRotate(const Point &p, double angle) override;

  const QTransform &transform() const override;
  void setTransform(const QTransform &t, bool combine=false) override;

  void setRenderHints(QPainter::RenderHints hints, bool on) override;

 private:
  QPainter*    painter_ { nullptr };
  BBox         clipRect_;
  QPainterPath clipPath_;
};

//---

class CQChartsPixelPaintDevice : public CQChartsViewPlotPaintDevice {
 public:
  CQChartsPixelPaintDevice(QPainter *painter);

  Type type() const override { return Type::PIXEL; }

  bool invertY() const override { return true; }
};

//---

class CQChartsViewPaintDevice : public CQChartsViewPlotPaintDevice {
 public:
  CQChartsViewPaintDevice(CQChartsView *view, QPainter *painter);

  Type type() const override { return Type::VIEW; }
};

//---

class CQChartsPlotPaintDevice : public CQChartsViewPlotPaintDevice {
 public:
  CQChartsPlotPaintDevice(CQChartsPlot *plot, QPainter *painter);

  Type type() const override { return Type::PLOT; }
};

//---

class CQChartsHtmlPaintDevice : public CQChartsPaintDevice {
 public:
  CQChartsHtmlPaintDevice(CQChartsView *view, std::ostream &os);
  CQChartsHtmlPaintDevice(CQChartsPlot *plot, std::ostream &os);

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

//---

class CQChartsScriptPaintDevice : public CQChartsHtmlPaintDevice {
 public:
  CQChartsScriptPaintDevice(CQChartsView *view, std::ostream &os);
  CQChartsScriptPaintDevice(CQChartsPlot *plot, std::ostream &os);

  Type type() const override { return Type::SCRIPT; }

  void save   () override;
  void restore() override;

  void setClipPath(const QPainterPath &path, Qt::ClipOperation operation) override;
  void setClipRect(const BBox &bbox, Qt::ClipOperation operation) override;

  void setPen(const QPen &pen) override;

  void setBrush(const QBrush &brush) override;

  void fillPath  (const QPainterPath &path, const QBrush &brush) override;
  void strokePath(const QPainterPath &path, const QPen &pen) override;
  void drawPath  (const QPainterPath &path) override;

  void fillRect(const BBox &bbox, const QBrush &brush) override;
  void drawRect(const BBox &bbox) override;

  void drawEllipse(const BBox &bbox, const CQChartsAngle &a=CQChartsAngle()) override;

//void drawArc(const BBox &rect, const CQChartsAngle &a1, const CQChartsAngle &a2) override;

  void drawPolygon (const Polygon &poly) override;
  void drawPolyline(const Polygon &poly) override;

  void drawLine(const Point &p1, const Point &p2) override;

  void drawPoint(const Point &p) override;

  void drawText(const Point &p, const QString &text) override;
  void drawTransformedText(const Point &p, const QString &text) override;

  void drawImage(const Point &, const QImage &) override;
  void drawImageInRect(const BBox &bbox, const CQChartsImage &image, bool stretch=true) override;

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

class CQChartsSVGPaintDevice : public CQChartsHtmlPaintDevice {
 public:
  CQChartsSVGPaintDevice(CQChartsView *view, std::ostream &os);
  CQChartsSVGPaintDevice(CQChartsPlot *plot, std::ostream &os);

  Type type() const override { return Type::SVG; }

  void save   () override;
  void restore() override;

  void setClipPath(const QPainterPath &path, Qt::ClipOperation operation) override;
  void setClipRect(const BBox &bbox, Qt::ClipOperation operation) override;

  void setPen(const QPen &pen) override;

  void setBrush(const QBrush &brush) override;

  void fillPath  (const QPainterPath &path, const QBrush &brush) override;
  void strokePath(const QPainterPath &path, const QPen &pen) override;
  void drawPath  (const QPainterPath &path) override;

  void fillRect(const BBox &bbox, const QBrush &brush) override;
  void drawRect(const BBox &bbox) override;

  void drawEllipse(const BBox &bbox, const CQChartsAngle &a=CQChartsAngle()) override;

//void drawArc(const BBox &rect, const CQChartsAngle &a1, const CQChartsAngle &a2) override;

  void drawPolygon (const Polygon &poly) override;
  void drawPolyline(const Polygon &poly) override;

  void drawLine(const Point &p1, const Point &p2) override;

  void drawPoint(const Point &p) override;

  void drawText(const Point &p, const QString &text) override;
  void drawTransformedText(const Point &p, const QString &text) override;

  void drawImage(const Point &, const QImage &) override;
  void drawImageInRect(const BBox &bbox, const CQChartsImage &image, bool stretch=true) override;

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
