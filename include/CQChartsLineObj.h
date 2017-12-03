#ifndef CQChartsLineObj_H
#define CQChartsLineObj_H

#include <CQChartsPaletteColor.h>
#include <CQChartsLineDash.h>
#include <QObject>
#include <QPointF>

class CQPropertyViewModel;
class QPainter;
class QPen;

class CQChartsLineObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool             displayed READ isDisplayed WRITE setDisplayed)
  Q_PROPERTY(QString          color     READ colorStr    WRITE setColorStr )
  Q_PROPERTY(double           width     READ width       WRITE setWidth    )
  Q_PROPERTY(CQChartsLineDash dash      READ dash        WRITE setDash     )

 public:
  CQChartsLineObj(CQChartsPlot *plot);

  virtual ~CQChartsLineObj() { }

  //---

  bool isDisplayed() const { return displayed_; }
  void setDisplayed(bool b) { displayed_ = b; redrawLineObj(); }

  void setColor(const CQChartsPaletteColor &c) { color_ = c; redrawLineObj(); }

  QString colorStr() const { return color_.colorStr(); }
  void setColorStr(const QString &str) { color_.setColorStr(str); redrawLineObj(); }

  QColor interpColor(int i, int n) const;

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; redrawLineObj(); }

  const CQChartsLineDash &dash() const { return dash_; }
  void setDash(const CQChartsLineDash &dash) { dash_ = dash; redrawLineObj(); }

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  void draw(QPainter *p, const QPointF &p1, const QPointF &p2) const;

  static void draw(QPainter *p, const QPointF &p1, const QPointF &p2, const QColor &color,
                   double width, const CQChartsLineDash &dash);

  static void draw(QPainter *p, const QPointF &p1, const QPointF &p2, const QPen &pen);

  virtual void redrawLineObj() { }

 protected:
  CQChartsPlot*        plot_      { nullptr };
  bool                 displayed_ { true };
  CQChartsPaletteColor color_;
  double               width_     { 0 };
  CQChartsLineDash     dash_      { };
};

#endif
