#ifndef CQChartsTextBoxObj_H
#define CQChartsTextBoxObj_H

#include <CQChartsBoxObj.h>
#include <QFont>

class CQChartsTextBoxObj : public CQChartsBoxObj {
  Q_OBJECT

  Q_PROPERTY(QString text  READ text  WRITE setText )
  Q_PROPERTY(QFont   font  READ font  WRITE setFont )
  Q_PROPERTY(QColor  color READ color WRITE setColor)

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

  //---

  void addProperties(CQPropertyViewTree *tree, const QString &path);

  //---

  virtual void draw(QPainter *p, const QRectF &rect);
  virtual void draw(QPainter *p, const QPolygonF &poly);

 protected:
  void drawText(QPainter *p, const QRectF &rect);

 protected:
  QString text_;
  QFont   font_;
  QColor  color_ { 0, 0, 0 };
};

#endif
