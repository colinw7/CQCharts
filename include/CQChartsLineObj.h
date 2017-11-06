#ifndef CQChartsLineObj_H
#define CQChartsLineObj_H

#include <CQChartsPaletteColor.h>
#include <QObject>
#include <QColor>
#include <QPointF>
#include <CLineDash.h>

class CQPropertyViewModel;
class QPainter;

class CQChartsLineObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool      displayed READ isDisplayed WRITE setDisplayed)
  Q_PROPERTY(QColor    color     READ color       WRITE setColor    )
  Q_PROPERTY(bool      palette   READ isPalette   WRITE setPalette  )
  Q_PROPERTY(double    width     READ width       WRITE setWidth    )
  Q_PROPERTY(CLineDash dash      READ dash        WRITE setDash     )

 public:
  CQChartsLineObj();

  virtual ~CQChartsLineObj() { }

  //---

  bool isDisplayed() const { return displayed_; }
  void setDisplayed(bool b) { displayed_ = b; redrawLineObj(); }

  const QColor &color() const { return color_.color; }
  void setColor(const QColor &c) { color_.color = c; redrawLineObj(); }

  bool isPalette() const { return color_.palette; }
  void setPalette(bool b) { color_.palette = b; redrawLineObj(); }

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; redrawLineObj(); }

  const CLineDash &dash() const { return dash_; }
  void setDash(const CLineDash &dash) { dash_ = dash; redrawLineObj(); }

  //---

  QString colorStr() const { return color_.colorStr(); }
  void setColorStr(const QString &str) { color_.setColorStr(str); redrawLineObj(); }

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  void draw(QPainter *p, const QPointF &p1, const QPointF &p2) const;

  static void draw(QPainter *p, const QPointF &p1, const QPointF &p2, const QColor &color,
                   double width, const CLineDash &dash);

  virtual void redrawLineObj() { }

 protected:
  bool                 displayed_ { true };
  CQChartsPaletteColor color_     { QColor(0, 0, 0), true };
  double               width_     { 0 };
  CLineDash            dash_      { };
};

#endif
