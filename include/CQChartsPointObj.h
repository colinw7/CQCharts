#ifndef CQChartsPointObj_H
#define CQChartsPointObj_H

#include <CQChartsPlotSymbol.h>
#include <CQChartsPaletteColor.h>
#include <QObject>
#include <QColor>
#include <QPointF>

class CQPropertyViewTree;

class CQChartsPointObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool    displayed  READ isDisplayed WRITE setDisplayed )
  Q_PROPERTY(QString symbolName READ symbolName  WRITE setSymbolName)
  Q_PROPERTY(QColor  color      READ color       WRITE setColor     )
  Q_PROPERTY(bool    palette    READ isPalette   WRITE setPalette   )
  Q_PROPERTY(double  size       READ size        WRITE setSize      )
  Q_PROPERTY(bool    filled     READ isFilled    WRITE setFilled    )

 public:
  CQChartsPointObj();

  virtual ~CQChartsPointObj() { }

  //---

  bool isDisplayed() const { return displayed_; }
  void setDisplayed(bool b) { displayed_ = b; redrawPointObj(); }

  const CSymbol2D::Type &symbolType() const { return symbol_; }
  void setSymbolType(const CSymbol2D::Type &v) { symbol_ = v; redrawPointObj(); }

  const QColor &color() const { return color_.color; }
  void setColor(const QColor &c) { color_.color = c; redrawPointObj(); }

  bool isPalette() const { return color_.palette; }
  void setPalette(bool b) { color_.palette = b; redrawPointObj(); }

  double size() const { return size_; }
  void setSize(double r) { size_ = r; redrawPointObj(); }

  bool isFilled() const { return filled_; }
  void setFilled(bool b) { filled_ = b; redrawPointObj(); }

  //---

  QString colorStr() const { return color_.colorStr(); }
  void setColorStr(const QString &str) { color_.setColorStr(str); redrawPointObj(); }

  //---

  QString symbolName() const;
  void setSymbolName(const QString &v);

  //---

  virtual void addProperties(CQPropertyViewTree *tree, const QString &path);

  void draw(QPainter *painter, const QPointF &p);

  static void draw(QPainter *painter, const QPointF &p, const CSymbol2D::Type &symbol,
                   double size, const QColor &color, bool filled);

  virtual void redrawPointObj() { }

 protected:
  bool                 displayed_ { true };
  CSymbol2D::Type      symbol_    { CSymbol2D::Type::CROSS };
  CQChartsPaletteColor color_     { QColor(0, 0, 0), true };
  double               size_      { 4 };
  bool                 filled_    { false };
};

#endif
