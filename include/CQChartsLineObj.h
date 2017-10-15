#ifndef CQChartsLineObj_H
#define CQChartsLineObj_H

#include <QObject>
#include <QColor>
#include <QPointF>
#include <CLineDash.h>

class CQPropertyViewTree;
class QPainter;

class CQChartsLineObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool      displayed READ isDisplayed WRITE setDisplayed)
  Q_PROPERTY(QColor    color     READ color       WRITE setColor    )
  Q_PROPERTY(double    width     READ width       WRITE setWidth    )
  Q_PROPERTY(CLineDash dash      READ dash        WRITE setDash     )
  Q_PROPERTY(bool      palette   READ isPalette   WRITE setPalette  )

 public:
  CQChartsLineObj();

  virtual ~CQChartsLineObj() { }

  //---

  bool isDisplayed() const { return displayed_; }
  void setDisplayed(bool b) { displayed_ = b; redrawLineObj(); }

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; redrawLineObj(); }

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; redrawLineObj(); }

  const CLineDash &dash() const { return dash_; }
  void setDash(const CLineDash &dash) { dash_ = dash; redrawLineObj(); }

  bool isPalette() const { return palette_; }
  void setPalette(bool b) { palette_ = b; }

  //---

  virtual void addProperties(CQPropertyViewTree *tree, const QString &path);

  void draw(QPainter *p, const QPointF &p1, const QPointF &p2) const;

  static void draw(QPainter *p, const QPointF &p1, const QPointF &p2, const QColor &color,
                   double width, const CLineDash &dash);

  virtual void redrawLineObj() { }

 protected:
  bool      displayed_ { true };
  QColor    color_     { 0, 0, 0 };
  double    width_     { 0 };
  CLineDash dash_      { };
  bool      palette_   { true };
};

#endif
