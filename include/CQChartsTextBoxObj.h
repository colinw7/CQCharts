#ifndef CQChartsTextBoxObj_H
#define CQChartsTextBoxObj_H

#include <CQChartsBoxObj.h>
#include <QFont>
#include <QRectF>

class CQChartsTextBoxObj : public CQChartsBoxObj {
  Q_OBJECT

  Q_PROPERTY(bool    visible READ isVisible WRITE setVisible )
  Q_PROPERTY(QString text    READ text      WRITE setText    )
  Q_PROPERTY(QFont   font    READ font      WRITE setFont    )
  Q_PROPERTY(QString color   READ colorStr  WRITE setColorStr)
  Q_PROPERTY(double  angle   READ angle     WRITE setAngle   )

 public:
  CQChartsTextBoxObj(CQChartsPlot *plot);

  virtual ~CQChartsTextBoxObj() { }

  //---

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; redrawBoxObj(); }

  const QString &text() const { return text_; }
  virtual void setText(const QString &s) { text_ = s; redrawBoxObj(); }

  const QFont &font() const { return font_; }
  virtual void setFont(const QFont &f) { font_ = f; redrawBoxObj(); }

  void setColor(const CQChartsPaletteColor &c) { color_ = c; redrawBoxObj(); }

  QString colorStr() const;
  void setColorStr(const QString &s);

  QColor interpColor(int i, int n) const;

  double angle() const { return angle_; }
  void setAngle(double r) { angle_ = r; redrawBoxObj(); }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  virtual void draw(CQChartsRenderer *renderer, const QRectF &rect) const;
  virtual void draw(CQChartsRenderer *renderer, const QPolygonF &poly) const;

  //---

  void drawText(CQChartsRenderer *renderer, const QRectF &rect, const QString &text) const;

 protected:
  bool                 visible_ { true };
  QString              text_;
  QFont                font_;
  CQChartsPaletteColor color_;
  double               angle_   { 0.0 };
};

//------

class CQChartsRotatedTextBoxObj : public CQChartsTextBoxObj {
 public:
  CQChartsRotatedTextBoxObj(CQChartsPlot *plot);

  const QRectF &rect() const { return rect_; }

  void draw(CQChartsRenderer *renderer, const QPointF &c, const QString &text, double angle=0.0,
            Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter) const;

 private:
  mutable QRectF rect_;
};

#endif
