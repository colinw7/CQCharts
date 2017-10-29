#ifndef CQChartsParallelPlot_H
#define CQChartsParallelPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPointObj.h>
#include <CQChartsLineObj.h>

class CQChartsParallelPlot;

class CQChartsParallelLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsParallelLineObj(CQChartsParallelPlot *plot, const CBBox2D &rect,
                          const QPolygonF &poly, int i, int n);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsParallelPlot *plot_ { nullptr };
  QPolygonF             poly_;
  int                   i_    { -1 };
  int                   n_    { -1 };
};

//---

class CQChartsParallelPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsParallelPointObj(CQChartsParallelPlot *plot, const CBBox2D &rect, double x, double y,
                           int iset, int nset, int i, int n);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsParallelPlot* plot_  { nullptr };
  double                x_     { 0.0 };
  double                y_     { 0.0 };
  int                   iset_  { -1 };
  int                   nset_  { -1 };
  int                   i_     { -1 };
  int                   n_     { -1 };
};

//---

class CQChartsParallelPlotType : public CQChartsPlotType {
 public:
  CQChartsParallelPlotType();

  QString name() const override { return "parallel"; }
  QString desc() const override { return "Parallel"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsParallelPlot : public CQChartsPlot {
  Q_OBJECT

  // properties
  //  line
  //   display, stroke
  //  fill under
  //   display, brush
  //  symbol
  //   display, color, shape. ...
  //  margin
  //  key

  Q_PROPERTY(int     xColumn      READ xColumn        WRITE setXColumn     )
  Q_PROPERTY(int     yColumn      READ yColumn        WRITE setYColumn     )
  Q_PROPERTY(QString yColumns     READ yColumnsStr    WRITE setYColumnsStr )
  Q_PROPERTY(bool    points       READ isPoints       WRITE setPoints      )
  Q_PROPERTY(QColor  pointsColor  READ pointsColor    WRITE setPointsColor )
  Q_PROPERTY(bool    lines        READ isLines        WRITE setLines       )
  Q_PROPERTY(QColor  linesColor   READ linesColor     WRITE setLinesColor  )
  Q_PROPERTY(double  linesWidth   READ linesWidth     WRITE setLinesWidth  )
  Q_PROPERTY(QString symbolName   READ symbolName     WRITE setSymbolName  )
  Q_PROPERTY(double  symbolSize   READ symbolSize     WRITE setSymbolSize  )
  Q_PROPERTY(bool    symbolFilled READ isSymbolFilled WRITE setSymbolFilled)

 public:
  CQChartsParallelPlot(CQChartsView *view, const ModelP &model);

  //---

  // columns

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }

  void setYColumn(int i) {
    yColumn_ = i;

    yColumns_.clear();

    if (yColumn_ >= 0)
      yColumns_.push_back(yColumn_);

    update();
  }

  const Columns &yColumns() const { return yColumns_; }

  void setYColumns(const Columns &yColumns) {
    yColumns_ = yColumns;

    if (! yColumns_.empty())
      yColumn_ = yColumns_[0];
    else
      yColumn_ = -1;

    update();
  }

  QString yColumnsStr() const;
  bool setYColumnsStr(const QString &s);

  //---

  // points
  bool isPoints() const { return pointObj_.isDisplayed(); }
  void setPoints(bool b) { pointObj_.setDisplayed(b); update(); }

  const QColor &pointsColor() const { return pointObj_.color(); }
  void setPointsColor(const QColor &c) { pointObj_.setColor(c); update(); }

  //---

  // lines
  bool isLines() const { return lineObj_.isDisplayed(); }
  void setLines(bool b) { lineObj_.setDisplayed(b); update(); }

  const QColor &linesColor() const { return lineObj_.color(); }
  void setLinesColor(const QColor &c) { lineObj_.setColor(c); update(); }

  double linesWidth() const { return lineObj_.width(); }
  void setLinesWidth(double w) { lineObj_.setWidth(w); update(); }

  //---

  // symbol
  double symbolSize() const { return pointObj_.size(); }
  void setSymbolSize(double r) { pointObj_.setSize(r); update(); }

  CSymbol2D::Type symbolType() const { return pointObj_.symbolType(); }
  void setSymbolType(CSymbol2D::Type t) { pointObj_.setSymbolType(t); update(); }

  QString symbolName() const { return pointObj_.symbolName(); }
  void setSymbolName(const QString &s) { pointObj_.setSymbolName(s); update(); }

  bool isSymbolFilled() const { return pointObj_.isFilled(); }
  void setSymbolFilled(bool b) { pointObj_.setFilled(b); update(); }

  //---

  const CRange2D &yRange(int i) { return yRanges_[i]; }

  CQChartsAxis *yAxis(int i) { return yAxes_[i]; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  //---

  int numSets() const;

  int getSetColumn(int i) const;

  int numValues() const;

  //---

  void draw(QPainter *) override;

 private:
  typedef std::vector<CRange2D>       Ranges;
  typedef std::vector<CQChartsAxis *> YAxes;

  int              xColumn_    { 0 };
  int              yColumn_    { 1 };
  Columns          yColumns_;
  Ranges           yRanges_;
  YAxes            yAxes_;
  CQChartsPointObj pointObj_;
  CQChartsLineObj  lineObj_;
};

#endif
