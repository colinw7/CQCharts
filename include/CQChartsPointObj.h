#ifndef CQChartsPointObj_H
#define CQChartsPointObj_H

#include <CQChartsPlotSymbol.h>
#include <CQChartsPaletteColor.h>
#include <QObject>
#include <QPointF>

class CQPropertyViewModel;

class CQChartsPointObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool    displayed     READ isDisplayed     WRITE setDisplayed     )
  Q_PROPERTY(QString symbolName    READ symbolName      WRITE setSymbolName    )
  Q_PROPERTY(QString strokeColor   READ strokeColorStr  WRITE setStrokeColorStr)
  Q_PROPERTY(QString fillColor     READ fillColorStr    WRITE setFillColorStr  )
  Q_PROPERTY(double  size          READ size            WRITE setSize          )
  Q_PROPERTY(bool    filled        READ isFilled        WRITE setFilled        )
  Q_PROPERTY(double  lineWidth     READ lineWidth       WRITE setLineWidth     )

 public:
  CQChartsPointObj(CQChartsPlot *plot);

  virtual ~CQChartsPointObj() { }

  //---

  bool isDisplayed() const { return displayed_; }
  void setDisplayed(bool b) { displayed_ = b; redrawPointObj(); }

  const CQChartsPlotSymbol::Type &symbolType() const { return symbol_; }
  void setSymbolType(const CQChartsPlotSymbol::Type &v) { symbol_ = v; redrawPointObj(); }

  double size() const { return size_; }
  void setSize(double r) { size_ = r; redrawPointObj(); }

  bool isStroked() const { return stroked_; }
  void setStroked(bool b) { stroked_ = b; redrawPointObj(); }

  double lineWidth() const { return lineWidth_; }
  void setLineWidth(double r) { lineWidth_ = r; redrawPointObj(); }

  bool isFilled() const { return filled_; }
  void setFilled(bool b) { filled_ = b; redrawPointObj(); }

  //---

  QString strokeColorStr() const { return strokeColor_.colorStr(); }
  void setStrokeColorStr(const QString &str) { strokeColor_.setColorStr(str); redrawPointObj(); }

  QString fillColorStr() const { return fillColor_.colorStr(); }
  void setFillColorStr(const QString &str) { fillColor_.setColorStr(str); redrawPointObj(); }

  // interp color from palette if needed
  QColor interpStrokeColor(int i, int n) const;
  QColor interpFillColor(int i, int n) const;

  //---

  QString symbolName() const;
  void setSymbolName(const QString &v);

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  void draw(QPainter *painter, const QPointF &p);

  static void draw(QPainter *painter, const QPointF &p, const CQChartsPlotSymbol::Type &symbol,
                   double size, bool stroked, const QColor &strokeColor, double lineWidth,
                   bool filled, const QColor &fillColor);

  virtual void redrawPointObj() { }

 protected:
  CQChartsPlot*            plot_        { nullptr };
  bool                     displayed_   { true };
  CQChartsPlotSymbol::Type symbol_      { CQChartsPlotSymbol::Type::CROSS };
  double                   size_        { 4 };
  bool                     stroked_     { true };
  CQChartsPaletteColor     strokeColor_;
  double                   lineWidth_   { 1 };
  bool                     filled_      { false };
  CQChartsPaletteColor     fillColor_;
};

#endif
