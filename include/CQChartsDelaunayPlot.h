#ifndef CQChartsDelaunayPlot_H
#define CQChartsDelaunayPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsLineObj.h>
#include <CQChartsPointObj.h>

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

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
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

  Q_PROPERTY(int     xColumn      READ xColumn        WRITE setXColumn     )
  Q_PROPERTY(int     yColumn      READ yColumn        WRITE setYColumn     )
  Q_PROPERTY(int     nameColumn   READ nameColumn     WRITE setNameColumn  )
  Q_PROPERTY(bool    points       READ isPoints       WRITE setPoints      )
  Q_PROPERTY(QColor  pointsColor  READ pointsColor    WRITE setPointsColor )
  Q_PROPERTY(bool    lines        READ isLines        WRITE setLines       )
  Q_PROPERTY(QColor  linesColor   READ linesColor     WRITE setLinesColor  )
  Q_PROPERTY(double  linesWidth   READ linesWidth     WRITE setLinesWidth  )
  Q_PROPERTY(QString symbolName   READ symbolName     WRITE setSymbolName  )
  Q_PROPERTY(double  symbolSize   READ symbolSize     WRITE setSymbolSize  )
  Q_PROPERTY(bool    symbolFilled READ isSymbolFilled WRITE setSymbolFilled)
  Q_PROPERTY(bool    voronoi      READ isVoronoi      WRITE setVoronoi     )

 public:
  CQChartsDelaunayPlot(CQChartsView *view, const ModelP &model);

  //---

  // columns

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; update(); }

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

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
  void setSymbolName(const QString &s) { pointObj_.setSymbolName(s); }

  bool isSymbolFilled() const { return pointObj_.isFilled(); }
  void setSymbolFilled(bool b) { pointObj_.setFilled(b); update(); }

  //---

  bool isVoronoi() const { return voronoi_; }
  void setVoronoi(bool b) { voronoi_ = b; update(); }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  //---

  void draw(QPainter *) override;

  void drawForeground(QPainter *p) override;

 private:
  void drawDelaunay(QPainter *p);
  void drawVoronoi (QPainter *p);

 private:
  int              xColumn_    { 0 };
  int              yColumn_    { 1 };
  int              nameColumn_ { -1 };
  CQChartsPointObj pointObj_;
  CQChartsLineObj  lineObj_;
  bool             voronoi_    { true };
  CDelaunay*       delaunay_   { nullptr };
};

#endif
