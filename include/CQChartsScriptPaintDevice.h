#ifndef CQChartsScriptPaintDevice_H
#define CQChartsScriptPaintDevice_H

#include <CQChartsHtmlPaintDevice.h>

/*!
 * \brief Paint Device to output graphics as JavaScript program
 * \ingroup Charts
 */
class CQChartsScriptPaintDevice : public CQChartsHtmlPaintDevice {
 public:
  using View  = CQChartsView;
  using Plot  = CQChartsPlot;
  using Angle = CQChartsAngle;
  using Image = CQChartsImage;

 public:
  CQChartsScriptPaintDevice(View *view, std::ostream &os);
  CQChartsScriptPaintDevice(Plot *plot, std::ostream &os);

  Type type() const override { return Type::SCRIPT; }

  void save   () override;
  void restore() override;

  void setClipPath(const QPainterPath &path, Qt::ClipOperation operation=Qt::ReplaceClip) override;
  void setClipRect(const BBox &bbox, Qt::ClipOperation operation=Qt::ReplaceClip) override;

  void setPen(const QPen &pen) override;
  void setBrush(const QBrush &brush) override;

  void fillPath  (const QPainterPath &path, const QBrush &brush) override;
  void strokePath(const QPainterPath &path, const QPen &pen) override;
  void drawPath  (const QPainterPath &path) override;

  void fillRect(const BBox &bbox) override;
  void drawRect(const BBox &bbox) override;

  void drawEllipse(const BBox &bbox, const Angle &a=Angle()) override;

//void drawArc(const BBox &rect, const Angle &a1, const Angle &a2) override;

  void drawPolygon (const Polygon &poly) override;
  void drawPolyline(const Polygon &poly) override;

  void drawLine(const Point &p1, const Point &p2) override;

  void drawPoint(const Point &p) override;

  void drawText(const Point &p, const QString &text) override;
  void drawTransformedText(const Point &p, const QString &text) override;

  void drawImage(const Point &, const QImage &) override;
  void drawImageInRect(const BBox &bbox, const Image &image, bool stretch=true,
                       const Angle &angle=Angle()) override;

  void setFont(const QFont &f, bool scale=true) override;

  void setTransform(const QTransform &t, bool combine=false) override;

  //---

  std::string context() const;
  void setContext(const std::string &context);

  const std::string &id() const { return id_; }
  void setId(const std::string &s) { id_ = s; }

  //---

  void startGroup(const QString &id, const GroupData &groupData=GroupData()) override;
  void endGroup() override;

  //---

  void writeProc(const QString &proc);

  //---

  static QString encodeObjId(const QString &id) {
    int len = id.length();

    QString id1;

    for (int i = 0; i < len; ++i) {
      auto c = id[i];

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
  std::string id_;
};

#endif
