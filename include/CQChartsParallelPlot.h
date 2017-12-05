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
  CQChartsParallelLineObj(CQChartsParallelPlot *plot, const CQChartsGeom::BBox &rect,
                          const QPolygonF &poly, const QModelIndex &ind, int i, int n);

  QString calcId() const override;

  QString calcTipId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &ind) const override;

  void draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &) override;

 private:
  CQChartsParallelPlot *plot_ { nullptr };
  QPolygonF             poly_;
  QModelIndex           ind_;
  int                   i_    { -1 };
  int                   n_    { -1 };
};

//---

class CQChartsParallelPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsParallelPointObj(CQChartsParallelPlot *plot, const CQChartsGeom::BBox &rect,
                           double x, double y, const QModelIndex &ind, int iset, int nset,
                           int i, int n);

  QString calcId() const override;

  QString calcTipId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &ind) const override;

  void draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &) override;

 private:
  CQChartsParallelPlot* plot_  { nullptr };
  double                x_     { 0.0 };
  double                y_     { 0.0 };
  QModelIndex           ind_;
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

  void addParameters() override;

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

  Q_PROPERTY(int     xColumn      READ xColumn        WRITE setXColumn       )
  Q_PROPERTY(int     yColumn      READ yColumn        WRITE setYColumn       )
  Q_PROPERTY(QString yColumns     READ yColumnsStr    WRITE setYColumnsStr   )
  Q_PROPERTY(bool    points       READ isPoints       WRITE setPoints        )
  Q_PROPERTY(QString pointsColor  READ pointsColorStr WRITE setPointsColorStr)
  Q_PROPERTY(bool    lines        READ isLines        WRITE setLines         )
  Q_PROPERTY(QString linesColor   READ linesColorStr  WRITE setLinesColorStr )
  Q_PROPERTY(double  linesWidth   READ linesWidth     WRITE setLinesWidth    )
  Q_PROPERTY(QString symbolName   READ symbolName     WRITE setSymbolName    )
  Q_PROPERTY(double  symbolSize   READ symbolSize     WRITE setSymbolSize    )
  Q_PROPERTY(bool    symbolFilled READ isSymbolFilled WRITE setSymbolFilled  )

 public:
  CQChartsParallelPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsParallelPlot();

  //---

  // columns

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; updateRangeAndObjs(); }

  int yColumn() const { return yColumn_; }

  void setYColumn(int i) {
    yColumn_ = i;

    yColumns_.clear();

    if (yColumn_ >= 0)
      yColumns_.push_back(yColumn_);

    updateRangeAndObjs();
  }

  const Columns &yColumns() const { return yColumns_; }

  void setYColumns(const Columns &yColumns) {
    yColumns_ = yColumns;

    if (! yColumns_.empty())
      yColumn_ = yColumns_[0];
    else
      yColumn_ = -1;

    updateRangeAndObjs();
  }

  QString yColumnsStr() const;
  bool setYColumnsStr(const QString &s);

  //---

  // points
  bool isPoints() const { return pointObj_->isDisplayed(); }
  void setPoints(bool b) { pointObj_->setDisplayed(b); update(); }

  QString pointsColorStr() const;
  void setPointsColorStr(const QString &str);

  QColor interpPointsColor(int i, int n) const;

  //---

  // lines
  bool isLines() const { return lineObj_->isDisplayed(); }
  void setLines(bool b) { lineObj_->setDisplayed(b); update(); }

  QString linesColorStr() const;
  void setLinesColorStr(const QString &str);

  double linesWidth() const { return lineObj_->width(); }
  void setLinesWidth(double w) { lineObj_->setWidth(w); update(); }

  QColor interpLinesColor(int i, int n) const;

  //---

  // symbol
  double symbolSize() const { return pointObj_->size(); }
  void setSymbolSize(double r) { pointObj_->setSize(r); update(); }

  CQChartsPlotSymbol::Type symbolType() const { return pointObj_->symbolType(); }
  void setSymbolType(CQChartsPlotSymbol::Type t) { pointObj_->setSymbolType(t); update(); }

  QString symbolName() const { return pointObj_->symbolName(); }
  void setSymbolName(const QString &s) { pointObj_->setSymbolName(s); update(); }

  bool isSymbolFilled() const { return pointObj_->isFilled(); }
  void setSymbolFilled(bool b) { pointObj_->setFilled(b); update(); }

  //---

  const CQChartsGeom::Range &yRange(int i) { return yRanges_[i]; }

  CQChartsAxis *yAxis(int i) { return yAxes_[i]; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  int numSets() const;

  int getSetColumn(int i) const;

  int numValues() const;

  //---

  bool probe(ProbeData &probeData) const override;

  void draw(CQChartsRenderer *) override;

 private:
  using Ranges = std::vector<CQChartsGeom::Range>;
  using YAxes  = std::vector<CQChartsAxis*>;

  int               xColumn_   { 0 };
  int               yColumn_   { 1 };
  Columns           yColumns_;
  Ranges            yRanges_;
  YAxes             yAxes_;
  CQChartsPointObj* pointObj_;
  CQChartsLineObj*  lineObj_   { nullptr };
};

#endif
