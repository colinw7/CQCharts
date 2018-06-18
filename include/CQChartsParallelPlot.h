#ifndef CQChartsParallelPlot_H
#define CQChartsParallelPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

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

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

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

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

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

  Q_PROPERTY(CQChartsColumn xColumn      READ xColumn        WRITE setXColumn     )
  Q_PROPERTY(CQChartsColumn yColumn      READ yColumn        WRITE setYColumn     )
  Q_PROPERTY(QString        yColumns     READ yColumnsStr    WRITE setYColumnsStr )
  Q_PROPERTY(bool           points       READ isPoints       WRITE setPoints      )
  Q_PROPERTY(CQChartsColor  pointsColor  READ pointsColor    WRITE setPointsColor )
  Q_PROPERTY(bool           lines        READ isLines        WRITE setLines       )
  Q_PROPERTY(CQChartsColor  linesColor   READ linesColor     WRITE setLinesColor  )
  Q_PROPERTY(CQChartsLength linesWidth   READ linesWidth     WRITE setLinesWidth  )
  Q_PROPERTY(QString        symbolName   READ symbolName     WRITE setSymbolName  )
  Q_PROPERTY(double         symbolSize   READ symbolSize     WRITE setSymbolSize  )
  Q_PROPERTY(bool           symbolFilled READ isSymbolFilled WRITE setSymbolFilled)

 public:
  CQChartsParallelPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsParallelPlot();

  //---

  // columns

  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c) { xColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &yColumn() const { return yColumn_; }

  void setYColumn(const CQChartsColumn &c) {
    yColumn_ = c;

    yColumns_.clear();

    if (yColumn_.isValid())
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
  bool isPoints() const { return pointData_.visible; }
  void setPoints(bool b) { pointData_.visible = b; update(); }

  const CQChartsColor &pointsColor() const;
  void setPointsColor(const CQChartsColor &c);

  QColor interpPointsColor(int i, int n) const;

  //---

  // lines
  bool isLines() const { return lineData_.visible; }
  void setLines(bool b) { lineData_.visible = b; update(); }

  const CQChartsColor &linesColor() const;
  void setLinesColor(const CQChartsColor &c);

  const CQChartsLength &linesWidth() const { return lineData_.width; }
  void setLinesWidth(const CQChartsLength &l) { lineData_.width = l; update(); }

  QColor interpLinesColor(int i, int n) const;

  //---

  // symbol
  double symbolSize() const { return pointData_.size; }
  void setSymbolSize(double s) { pointData_.size = s; update(); }

  CQChartsPlotSymbol::Type symbolType() const { return pointData_.type; }
  void setSymbolType(CQChartsPlotSymbol::Type t) { pointData_.type = t; update(); }

  QString symbolName() const;
  void setSymbolName(const QString &s);

  bool isSymbolFilled() const { return pointData_.fill.visible; }
  void setSymbolFilled(bool b) { pointData_.fill.visible = b; update(); }

  //---

  const CQChartsGeom::Range &yRange(int i) { return yRanges_[i]; }

  CQChartsAxis *yAxis(int i) { return yAxes_[i]; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  int numSets() const;

  const CQChartsColumn &getSetColumn(int i) const;

  //---

  bool probe(ProbeData &probeData) const override;

  void draw(QPainter *) override;

 private:
  using Ranges = std::vector<CQChartsGeom::Range>;
  using YAxes  = std::vector<CQChartsAxis*>;

  CQChartsColumn     xColumn_   { 0 }; // x value column
  CQChartsColumn     yColumn_   { 1 }; // y value columns
  Columns            yColumns_;        // y value columns
  Ranges             yRanges_;         // y ranges
  YAxes              yAxes_;           // y axes
  CQChartsSymbolData pointData_;       // point style data
  CQChartsLineData   lineData_;        // line style data
};

#endif
