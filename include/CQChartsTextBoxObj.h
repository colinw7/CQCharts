#ifndef CQChartsTextBoxObj_H
#define CQChartsTextBoxObj_H

#include <CQChartsBoxObj.h>
#include <QFont>
#include <QRectF>

class CQChartsTextBoxObj : public CQChartsBoxObj {
  Q_OBJECT

  Q_PROPERTY(QString text  READ text  WRITE setText )
  Q_PROPERTY(QFont   font  READ font  WRITE setFont )
  Q_PROPERTY(QColor  color READ color WRITE setColor)
  Q_PROPERTY(double  angle READ angle WRITE setAngle)

 public:
  CQChartsTextBoxObj();

  virtual ~CQChartsTextBoxObj() { }

  //---

  const QString &text() const { return text_; }
  virtual void setText(const QString &s) { text_ = s; redrawBoxObj(); }

  const QFont &font() const { return font_; }
  virtual void setFont(const QFont &f) { font_ = f; redrawBoxObj(); }

  const QColor &color() const { return color_; }
  virtual void setColor(const QColor &c) { color_ = c; redrawBoxObj(); }

  double angle() const { return angle_; }
  void setAngle(double r) { angle_ = r; redrawBoxObj(); }

  //---

  void addProperties(CQPropertyViewTree *tree, const QString &path) override;

  //---

  virtual void draw(QPainter *p, const QRectF &rect) const;
  virtual void draw(QPainter *p, const QPolygonF &poly) const;

  //---

  void drawText(QPainter *p, const QRectF &rect, const QString &text) const;

 protected:
  QString text_;
  QFont   font_;
  QColor  color_ { 0, 0, 0 };
  double  angle_ { 0.0 };
};

//------

class CQChartsRotatedTextBoxObj : public CQChartsTextBoxObj {
 public:
  CQChartsRotatedTextBoxObj();

  const QRectF &rect() const { return rect_; }

  void draw(QPainter *p, const QPointF &c, const QString &text, double angle=0.0,
            Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter) const;

 private:
  mutable QRectF rect_;
};

#endif
