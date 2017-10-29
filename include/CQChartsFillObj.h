#ifndef CQChartsFillObj_H
#define CQChartsFillObj_H

#include <CQChartsPaletteColor.h>
#include <QObject>
#include <QColor>
#include <QRectF>
#include <QPolygonF>

class CQPropertyViewTree;
class QPainter;

class CQChartsFillObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool    visible READ isVisible WRITE setVisible)
  Q_PROPERTY(QColor  color   READ color     WRITE setColor  )
  Q_PROPERTY(bool    palette READ isPalette WRITE setPalette)
  Q_PROPERTY(double  alpha   READ alpha     WRITE setAlpha  )
  Q_PROPERTY(Pattern pattern READ pattern   WRITE setPattern)

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
  CQChartsFillObj();

  virtual ~CQChartsFillObj() { }

  //---

  // visible
  bool isVisible() const { return visible_; }
  virtual void setVisible(bool b) { visible_ = b; redrawFillObj(); }

  // color (if not from palette)
  const QColor &color() const { return color_.color; }
  virtual void setColor(const QColor &c) { color_.color = c; redrawFillObj(); }

  // color from palette
  bool isPalette() const { return color_.palette; }
  virtual void setPalette(bool b) { color_.palette = b; redrawFillObj(); }

  // alpha
  double alpha() const { return alpha_; }
  virtual void setAlpha(double r) { alpha_ = r; redrawFillObj(); }

  // pattern
  const Pattern &pattern() const { return pattern_; }
  virtual void setPattern(const Pattern &v) { pattern_ = v; }

  //---

  QString colorStr() const { return color_.colorStr(); }
  void setColorStr(const QString &str) { color_.setColorStr(str); redrawFillObj(); }

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

  virtual void addProperties(CQPropertyViewTree *tree, const QString &path);

  void draw(QPainter *p, const QRectF &rect) const;

  void draw(QPainter *p, const QPolygonF &poly) const;

  virtual void redrawFillObj() { }

  //---

 protected:
  bool                 visible_ { false };
  CQChartsPaletteColor color_   { QColor(255, 255, 255), true };
  double               alpha_   { 1.0 };
  Pattern              pattern_ { Pattern::SOLID };
};

#endif
