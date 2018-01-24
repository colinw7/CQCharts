#ifndef CQChartsTextBoxObj_H
#define CQChartsTextBoxObj_H

#include <CQChartsBoxObj.h>
#include <CQChartsGeom.h>
#include <QFont>
#include <QRectF>

class CQChartsTextBoxObj : public CQChartsBoxObj {
  Q_OBJECT

  Q_PROPERTY(bool    textVisible  READ isTextVisible  WRITE setTextVisible )
  Q_PROPERTY(QString textStr      READ textStr        WRITE setTextStr     )
  Q_PROPERTY(QFont   textFont     READ textFont       WRITE setTextFont    )
  Q_PROPERTY(QString textColor    READ textColorStr   WRITE setTextColorStr)
  Q_PROPERTY(double  textAlpha    READ textAlpha      WRITE setTextAlpha   )
  Q_PROPERTY(double  textAngle    READ textAngle      WRITE setTextAngle   )
  Q_PROPERTY(bool    textContrast READ isTextContrast WRITE setTextContrast)

 public:
  CQChartsTextBoxObj(CQChartsPlot *plot);

  virtual ~CQChartsTextBoxObj() { }

  //---

  bool isTextVisible() const { return textVisible_; }
  void setTextVisible(bool b) { textVisible_ = b; redrawBoxObj(); }

  const QString &textStr() const { return textStr_; }
  virtual void setTextStr(const QString &s) { textStr_ = s; redrawBoxObj(); }

  const QFont &textFont() const { return textFont_; }
  virtual void setTextFont(const QFont &f) { textFont_ = f; redrawBoxObj(); }

  void setTextFontSize(double s) { textFont_.setPointSizeF(s); redrawBoxObj(); }

  void setTextColor(const CQChartsPaletteColor &c) { textColor_ = c; redrawBoxObj(); }

  QString textColorStr() const;
  void setTextColorStr(const QString &s);

  QColor interpTextColor(int i, int n) const;

  double textAlpha() const { return textAlpha_; }
  void setTextAlpha(double r) { textAlpha_ = r; redrawBoxObj(); }

  double textAngle() const { return textAngle_; }
  void setTextAngle(double r) { textAngle_ = r; redrawBoxObj(); }

  bool isTextContrast() const { return textContrast_; }
  void setTextContrast(bool b) { textContrast_ = b; redrawBoxObj(); }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  virtual void draw(QPainter *painter, const QRectF &rect) const;
  virtual void draw(QPainter *painter, const QPolygonF &poly) const;

  //---

  void drawText(QPainter *painter, const QRectF &rect, const QString &text) const;

 protected:
  bool                 textVisible_  { true };
  QString              textStr_;
  QFont                textFont_;
  CQChartsPaletteColor textColor_;
  double               textAlpha_    { 1.0 };
  double               textAngle_    { 0.0 };
  bool                 textContrast_ { false };
};

//------

class CQChartsRotatedTextBoxObj : public CQChartsTextBoxObj {
 public:
  CQChartsRotatedTextBoxObj(CQChartsPlot *plot);

  const QRectF &rect() const { return rect_; }

  void draw(QPainter *painter, const QPointF &c, const QString &text, double angle=0.0,
            Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter) const;

  CQChartsGeom::BBox bbox(const QPointF &center, const QString &text, double angle=0.0,
                          Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter) const;

  void drawConnectedRadialText(QPainter *painter, const QPointF &center, double ro, double lr,
                               double ta, const QString &text, const QPen &lpen, bool isRotated);

  void calcConnectedRadialTextBBox(const QPointF &center, double ro, double lr, double ta,
                                   const QString &text, bool isRotated, CQChartsGeom::BBox &tbbox);

 private:
  void drawCalcConnectedRadialText(QPainter *painter, const QPointF &center, double ro, double lr,
                                   double ta, const QString &text, const QPen &lpen,
                                   bool isRotated, CQChartsGeom::BBox &tbbox);
 private:
  mutable QRectF rect_;
};

#endif
