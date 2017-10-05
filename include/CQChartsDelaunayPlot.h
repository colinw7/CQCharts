#ifndef CQChartsDelaunayPlot_H
#define CQChartsDelaunayPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>

class CQChartsDelaunayPlot;
class CDelaunay;

//---

class CQChartsDelaunayPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsDelaunayPointObj(CQChartsDelaunayPlot *plot, const CBBox2D &rect, double x, double y,
                           int i, int n);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsDelaunayPlot *plot_ { nullptr };
  double                x_    { 0.0 };
  double                y_    { 0.0 };
  int                   i_    { -1 };
  int                   n_    { -1 };
};

//---

class CQChartsDelaunayPlotType : public CQChartsPlotType {
 public:
  CQChartsDelaunayPlotType();

  QString name() const override { return "delaunay"; }
  QString desc() const override { return "Delaunay"; }

  CQChartsPlot *create(CQChartsView *view, QAbstractItemModel *model) const override;
};

//---

class CQChartsDelaunayPlot : public CQChartsPlot {
  Q_OBJECT

  // properties
  //  point
  //   display, color, symbol, size
  //  line
  //   display, stroke
  //  margin
  //  key

  Q_PROPERTY(int     xColumn     READ xColumn     WRITE setXColumn    )
  Q_PROPERTY(int     yColumn     READ yColumn     WRITE setYColumn    )
  Q_PROPERTY(int     nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(bool    points      READ isPoints    WRITE setPoints     )
  Q_PROPERTY(QColor  pointsColor READ pointsColor WRITE setPointsColor)
  Q_PROPERTY(bool    lines       READ isLines     WRITE setLines      )
  Q_PROPERTY(QColor  linesColor  READ linesColor  WRITE setLinesColor )
  Q_PROPERTY(QString symbolName  READ symbolName  WRITE setSymbolName )
  Q_PROPERTY(double  symbolSize  READ symbolSize  WRITE setSymbolSize )
  Q_PROPERTY(bool    voronoi     READ isVoronoi   WRITE setVoronoi    )

 private:
  struct PointData {
    bool            shown  { true };
    CSymbol2D::Type symbol { CSymbol2D::Type::CROSS };
    QColor          color  { 200, 40, 40 };
    double          size   { 4 };

    PointData() { }
  };

  struct LineData {
    bool   shown { true };
    QColor color { 40, 40, 200 };

    LineData() { }
  };

 public:
  CQChartsDelaunayPlot(CQChartsView *view, QAbstractItemModel *model);

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; update(); }

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  //---

  bool isPoints() const { return pointData_.shown; }
  void setPoints(bool b) { pointData_.shown = b; update(); }

  const QColor &pointsColor() const { return pointData_.color; }
  void setPointsColor(const QColor &c) { pointData_.color = c; update(); }

  //---

  bool isLines() const { return lineData_.shown; }
  void setLines(bool b) { lineData_.shown = b; update(); }

  const QColor &linesColor() const { return lineData_.color; }
  void setLinesColor(const QColor &c) { lineData_.color = c; update(); }

  //---

  double symbolSize() const { return pointData_.size; }
  void setSymbolSize(double r) { pointData_.size = r; update(); }

  CSymbol2D::Type symbolType() const { return pointData_.symbol; }
  void setSymbolType(CSymbol2D::Type t) { pointData_.symbol = t; update(); }

  QString symbolName() const;
  void setSymbolName(const QString &s);

  //---

  bool isVoronoi() const { return voronoi_; }
  void setVoronoi(bool b) { voronoi_ = b; update(); }

  //---

  void addProperties() override;

  void updateRange() override;

  void initObjs(bool force=false) override;

  //---

  void draw(QPainter *) override;

  void drawForeground(QPainter *p);

 private:
  void drawDelaunay(QPainter *p);
  void drawVoronoi (QPainter *p);

 private:
  typedef std::map<int,bool> IdHidden;

  int        xColumn_    { 0 };
  int        yColumn_    { 1 };
  int        nameColumn_ { -1 };
  PointData  pointData_;
  LineData   lineData_;
  bool       voronoi_    { true };
  CDelaunay* delaunay_   { nullptr };
};

#endif
