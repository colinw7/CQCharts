#ifndef CQChartsFillObj_H
#define CQChartsFillObj_H

#include <CQChartsPaletteColor.h>
#include <QObject>
#include <QRectF>
#include <QPolygonF>

class CQPropertyViewModel;
class QPainter;

class CQChartsFillObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool    visible READ isVisible WRITE setVisible )
  Q_PROPERTY(QString color   READ colorStr  WRITE setColorStr)
  Q_PROPERTY(double  alpha   READ alpha     WRITE setAlpha   )
  Q_PROPERTY(Pattern pattern READ pattern   WRITE setPattern )

  Q_ENUMS(Pattern);

 public:
  enum class Pattern {
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

 public:
  CQChartsFillObj(CQChartsPlot *plot);

  virtual ~CQChartsFillObj() { }

  //---

  // visible
  bool isVisible() const { return visible_; }
  virtual void setVisible(bool b) { visible_ = b; redrawFillObj(); }

  // color
  void setColor(const CQChartsPaletteColor &c) { color_ = c; redrawFillObj(); }

  QString colorStr() const { return color_.colorStr(); }
  void setColorStr(const QString &str) { color_.setColorStr(str); redrawFillObj(); }

  QColor interpColor(int i, int n) const;

  // alpha
  double alpha() const { return alpha_; }
  virtual void setAlpha(double r) { alpha_ = r; redrawFillObj(); }

  // pattern
  const Pattern &pattern() const { return pattern_; }
  virtual void setPattern(const Pattern &v) { pattern_ = v; }

  //---

  static Qt::BrushStyle patternToStyle(const Pattern &pattern) {
    switch (pattern) {
      case Pattern::SOLID: return Qt::SolidPattern;
      case Pattern::HATCH: return Qt::CrossPattern;
      case Pattern::DENSE: return Qt::Dense5Pattern;
      case Pattern::HORIZ: return Qt::HorPattern;
      case Pattern::VERT : return Qt::VerPattern;
      case Pattern::FDIAG: return Qt::FDiagPattern;
      case Pattern::BDIAG: return Qt::BDiagPattern;
      default            : return Qt::SolidPattern;
    }
  }

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  void draw(QPainter *p, const QRectF &rect) const;

  void draw(QPainter *p, const QPolygonF &poly) const;

  virtual void redrawFillObj() { }

  //---

 protected:
  CQChartsPlot*        plot_    { nullptr };
  bool                 visible_ { false };
  CQChartsPaletteColor color_;
  double               alpha_   { 1.0 };
  Pattern              pattern_ { Pattern::SOLID };
};

#endif
