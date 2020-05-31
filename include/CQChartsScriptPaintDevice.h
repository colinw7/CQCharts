#ifndef CQChartsScriptPaintDevice_H
#define CQChartsScriptPaintDevice_H

#include <CQChartsHtmlPaintDevice.h>

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

  void fillRect(const BBox &bbox) override;
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

#endif
