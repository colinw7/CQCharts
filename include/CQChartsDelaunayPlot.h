#ifndef CQChartsDelaunayPlot_H
#define CQChartsDelaunayPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

class CQChartsDelaunayPlotType : public CQChartsPlotType {
 public:
  CQChartsDelaunayPlotType();

  QString name() const override { return "delaunay"; }
  QString desc() const override { return "Delaunay"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  QString description() const;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsDelaunayPlot;
class CQChartsDelaunay;

//---

class CQChartsDelaunayPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsDelaunayPointObj(CQChartsDelaunayPlot *plot, const CQChartsGeom::BBox &rect,
                           double x, double y, const QModelIndex &ind, int i, int n);

  QString calcId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsDelaunayPlot *plot_ { nullptr };
  double                x_    { 0.0 };
  double                y_    { 0.0 };
  QModelIndex           ind_;
  int                   i_    { -1 };
  int                   n_    { -1 };
};

//---

class CQChartsDelaunayPlot : public CQChartsPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn xColumn    READ xColumn    WRITE setXColumn   )
  Q_PROPERTY(CQChartsColumn yColumn    READ yColumn    WRITE setYColumn   )
  Q_PROPERTY(CQChartsColumn nameColumn READ nameColumn WRITE setNameColumn)

  // voronoi
  Q_PROPERTY(bool   voronoi          READ isVoronoi        WRITE setVoronoi         )
  Q_PROPERTY(double voronoiPointSize READ voronoiPointSize WRITE setVoronoiPointSize)

  // point:
  //  display
  Q_PROPERTY(bool           points             READ isPoints          WRITE setPoints           )
  //  symbol (type, color, size, ,..)
  Q_PROPERTY(CQChartsSymbol symbolType         READ symbolType        WRITE setSymbolType       )
  Q_PROPERTY(CQChartsLength symbolSize         READ symbolSize        WRITE setSymbolSize       )
  Q_PROPERTY(bool           symbolStroked      READ isSymbolStroked   WRITE setSymbolStroked    )
  Q_PROPERTY(CQChartsColor  symbolStrokeColor  READ symbolStrokeColor WRITE setSymbolStrokeColor)
  Q_PROPERTY(double         symbolStrokeAlpha  READ symbolStrokeAlpha WRITE setSymbolStrokeAlpha)
  Q_PROPERTY(CQChartsLength symbolStrokeWidth  READ symbolStrokeWidth WRITE setSymbolStrokeWidth)
  Q_PROPERTY(bool           symbolFilled       READ isSymbolFilled    WRITE setSymbolFilled     )
  Q_PROPERTY(CQChartsColor  symbolFillColor    READ symbolFillColor   WRITE setSymbolFillColor  )
  Q_PROPERTY(double         symbolFillAlpha    READ symbolFillAlpha   WRITE setSymbolFillAlpha  )
  Q_PROPERTY(Pattern        symbolFillPattern  READ symbolFillPattern WRITE setSymbolFillPattern)

  // line:
  //  display, stroke
  Q_PROPERTY(bool           lines      READ isLines    WRITE setLines     )
  Q_PROPERTY(CQChartsColor  linesColor READ linesColor WRITE setLinesColor)
  Q_PROPERTY(double         linesAlpha READ linesAlpha WRITE setLinesAlpha)
  Q_PROPERTY(CQChartsLength linesWidth READ linesWidth WRITE setLinesWidth)

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
  CQChartsDelaunayPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsDelaunayPlot();

  //---

  // columns
  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c) { xColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &yColumn() const { return yColumn_; }
  void setYColumn(const CQChartsColumn &c) { yColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c) { nameColumn_ = c; updateRangeAndObjs(); }

  //---

  // voronoi
  bool isVoronoi() const { return voronoi_; }

  double voronoiPointSize() const { return voronoiPointSize_; }
  void setVoronoiPointSize(double r) { voronoiPointSize_ = r; update(); }

  //---

  // points
  bool isPoints() const { return pointData_.visible; }
  void setPoints(bool b) { pointData_.visible = b; update(); }

  const CQChartsColor &symbolStrokeColor() const;
  void setSymbolStrokeColor(const CQChartsColor &c);

  QColor interpPointStrokeColor(int i, int n) const;

  double symbolStrokeAlpha() const;
  void setSymbolStrokeAlpha(double a);

  const CQChartsColor &symbolFillColor() const;
  void setSymbolFillColor(const CQChartsColor &c);

  QColor interpPointFillColor(int i, int n) const;

  double symbolFillAlpha() const;
  void setSymbolFillAlpha(double a);

  Pattern symbolFillPattern() const;
  void setSymbolFillPattern(const Pattern &p);

  //---

  // lines
  bool isLines() const { return lineData_.visible; }
  void setLines(bool b) { lineData_.visible = b; update(); }

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

  const QString &yname() const { return yname_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  void addPointObj(double x, double y, const QModelIndex &xind, int r);

  //---

  bool addMenuItems(QMenu *menu);

  //---

  void draw(QPainter *) override;

  void drawForeground(QPainter *painter) override;

 public slots:
  void setVoronoi(bool b);

 private:
  void drawDelaunay(QPainter *p);
  void drawVoronoi (QPainter *p);

 private:
  CQChartsColumn     xColumn_          { 0 };       // x column
  CQChartsColumn     yColumn_          { 1 };       // y column
  CQChartsColumn     nameColumn_;                   // name column
  CQChartsSymbolData pointData_;                    // point style data
  CQChartsLineData   lineData_;                     // line style data
  bool               voronoi_          { true };    // is voronoi
  double             voronoiPointSize_ { 2 };       // voronoi point size
  int                nr_               { 0 };       // number of rows
  CQChartsDelaunay*  delaunay_         { nullptr }; // delaunay data
  QString            yname_;                        // y name
};

#endif
