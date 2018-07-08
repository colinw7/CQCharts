#ifndef CQChartsParallelPlot_H
#define CQChartsParallelPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsAxis.h>
#include <CQChartsData.h>

//---

class CQChartsParallelPlotType : public CQChartsPlotType {
 public:
  CQChartsParallelPlotType();

  QString name() const override { return "parallel"; }
  QString desc() const override { return "Parallel"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  QString description() const;

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
  void getPolyLine(QPolygonF &poly) const;

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
  //  lines
  //   display, stroke
  //  points
  //   display, color, shape. ...
  //  margin
  //  key

  // columns
  Q_PROPERTY(CQChartsColumn xColumn  READ xColumn     WRITE setXColumn    )
  Q_PROPERTY(CQChartsColumn yColumn  READ yColumn     WRITE setYColumn    )
  Q_PROPERTY(QString        yColumns READ yColumnsStr WRITE setYColumnsStr)

  // options
  Q_PROPERTY(bool           horizontal   READ isHorizontal   WRITE setHorizontal)

  // lines
  Q_PROPERTY(bool           lines           READ isLines           WRITE setLines          )
  Q_PROPERTY(bool           linesSelectable READ isLinesSelectable WRITE setLinesSelectable)
  Q_PROPERTY(CQChartsColor  linesColor      READ linesColor        WRITE setLinesColor     )
  Q_PROPERTY(double         linesAlpha      READ linesAlpha        WRITE setLinesAlpha     )
  Q_PROPERTY(CQChartsLength linesWidth      READ linesWidth        WRITE setLinesWidth     )

  // points
  Q_PROPERTY(bool           points            READ isPoints          WRITE setPoints           )
  Q_PROPERTY(CQChartsSymbol symbolType        READ symbolType        WRITE setSymbolType       )
  Q_PROPERTY(CQChartsLength symbolSize        READ symbolSize        WRITE setSymbolSize       )
  Q_PROPERTY(bool           symbolStroked     READ isSymbolStroked   WRITE setSymbolStroked    )
  Q_PROPERTY(CQChartsColor  symbolStrokeColor READ symbolStrokeColor WRITE setSymbolStrokeColor)
  Q_PROPERTY(double         symbolStrokeAlpha READ symbolStrokeAlpha WRITE setSymbolStrokeAlpha)
  Q_PROPERTY(CQChartsLength symbolStrokeWidth READ symbolStrokeWidth WRITE setSymbolStrokeWidth)
  Q_PROPERTY(bool           symbolFilled      READ isSymbolFilled    WRITE setSymbolFilled     )
  Q_PROPERTY(CQChartsColor  symbolFillColor   READ symbolFillColor   WRITE setSymbolFillColor  )
  Q_PROPERTY(double         symbolFillAlpha   READ symbolFillAlpha   WRITE setSymbolFillAlpha  )
  Q_PROPERTY(Pattern        symbolFillPattern READ symbolFillPattern WRITE setSymbolFillPattern)

  Q_ENUMS(Pattern)

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
  CQChartsParallelPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsParallelPlot();

  //---

  // columns

  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c) { xColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &yColumn() const { return yColumns_.column(); }
  void setYColumn(const CQChartsColumn &c) { yColumns_.setColumn(c); updateRangeAndObjs(); }

  const Columns &yColumns() const { return yColumns_.columns(); }
  void setYColumns(const Columns &cols) { yColumns_.setColumns(cols); updateRangeAndObjs(); }

  QString yColumnsStr() const { return yColumns_.columnsStr(); }
  bool setYColumnsStr(const QString &s) { return yColumns_.setColumnsStr(s); }

  //---

  bool isHorizontal() const { return horizontal_; }

  //---

  // points
  bool isPoints() const { return pointData_.visible; }
  void setPoints(bool b) { pointData_.visible = b; updateObjs(); }

  const CQChartsColor &symbolStrokeColor() const;
  void setSymbolStrokeColor(const CQChartsColor &c);

  QColor interpSymbolStrokeColor(int i, int n) const;

  double symbolStrokeAlpha() const;
  void setSymbolStrokeAlpha(double a);

  const CQChartsColor &symbolFillColor() const;
  void setSymbolFillColor(const CQChartsColor &c);

  QColor interpSymbolFillColor(int i, int n) const;

  double symbolFillAlpha() const;
  void setSymbolFillAlpha(double a);

  Pattern symbolFillPattern() const;
  void setSymbolFillPattern(const Pattern &p);

  //---

  // lines
  bool isLines() const { return lineData_.visible; }
  void setLines(bool b) { lineData_.visible = b; updateObjs(); }

  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b) { linesSelectable_ = b; update(); }

  const CQChartsColor &linesColor() const;
  void setLinesColor(const CQChartsColor &c);

  QColor interpLinesColor(int i, int n) const;

  double linesAlpha() const { return lineData_.alpha; }
  void setLinesAlpha(double a) { lineData_.alpha = a; update(); }

  const CQChartsLength &linesWidth() const { return lineData_.width; }
  void setLinesWidth(const CQChartsLength &l) { lineData_.width = l; update(); }

  //---

  // symbol
  const CQChartsSymbol &symbolType() const { return pointData_.type; }
  void setSymbolType(const CQChartsSymbol &t) { pointData_.type = t; update(); }

  const CQChartsLength &symbolSize() const { return pointData_.size; }
  void setSymbolSize(const CQChartsLength &s) { pointData_.size = s; update(); }

  bool isSymbolStroked() const { return pointData_.stroke.visible; }
  void setSymbolStroked(bool b) { pointData_.stroke.visible = b; update(); }

  const CQChartsLength &symbolStrokeWidth() const { return pointData_.stroke.width; }
  void setSymbolStrokeWidth(const CQChartsLength &l) { pointData_.stroke.width = l; update(); }

  bool isSymbolFilled() const { return pointData_.fill.visible; }
  void setSymbolFilled(bool b) { pointData_.fill.visible = b; update(); }

  //---

  const CQChartsGeom::Range &setRange(int i) { return setRanges_[i]; }

  CQChartsAxis *axis(int i) { return axes_[i]; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  bool rowColValue(int row, const CQChartsColumn &column, const QModelIndex &parent,
                   double &value, double defVal);

  int numSets() const;

  const CQChartsColumn &getSetColumn(int i) const;

  //---

  bool probe(ProbeData &probeData) const override;

  bool addMenuItems(QMenu *menu) override;

  void draw(QPainter *) override;

 public slots:
  // set horizontal
  void setHorizontal(bool b);

 private:
  using Ranges  = std::vector<CQChartsGeom::Range>;
  using YAxes   = std::vector<CQChartsAxis*>;
  using AxisDir = CQChartsAxis::Direction;

  CQChartsColumn     xColumn_         { 0 };                   // x value column
  CQChartsColumns    yColumns_        { 1 };                   // y value columns
  bool               horizontal_      { false };               // horizontal bars
  Ranges             setRanges_;                               // value set ranges
  AxisDir            adir_            { AxisDir::HORIZONTAL }; // axis direction
  YAxes              axes_;                                    // value axes
  CQChartsSymbolData pointData_;                               // point style data
  bool               linesSelectable_ { false };               // are lines selectable
  CQChartsLineData   lineData_;                                // line style data
};

#endif
