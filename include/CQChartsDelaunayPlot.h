#ifndef CQChartsDelaunayPlot_H
#define CQChartsDelaunayPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsLineObj.h>
#include <CQChartsPointObj.h>

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

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

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

class CQChartsDelaunayPlotType : public CQChartsPlotType {
 public:
  CQChartsDelaunayPlotType();

  QString name() const override { return "delaunay"; }
  QString desc() const override { return "Delaunay"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsDelaunayPlot : public CQChartsPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(int xColumn    READ xColumn    WRITE setXColumn       )
  Q_PROPERTY(int yColumn    READ yColumn    WRITE setYColumn       )
  Q_PROPERTY(int nameColumn READ nameColumn WRITE setNameColumn    )

  // voronoi
  Q_PROPERTY(bool   voronoi          READ isVoronoi        WRITE setVoronoi         )
  Q_PROPERTY(double voronoiPointSize READ voronoiPointSize WRITE setVoronoiPointSize)

  // point:
  //  display, color, symbol, size
  Q_PROPERTY(bool    points             READ isPoints             WRITE setPoints              )
  Q_PROPERTY(QString symbolName         READ symbolName           WRITE setSymbolName          )
  Q_PROPERTY(double  symbolSize         READ symbolSize           WRITE setSymbolSize          )
  Q_PROPERTY(bool    symbolStroked      READ isSymbolStroked      WRITE setSymbolStroked       )
  Q_PROPERTY(QString pointsStrokeColor  READ pointsStrokeColorStr WRITE setPointsStrokeColorStr)
  Q_PROPERTY(double  pointsStrokeAlpha  READ pointsStrokeAlpha    WRITE setPointsStrokeAlpha   )
  Q_PROPERTY(double  symbolLineWidth    READ symbolLineWidth      WRITE setSymbolLineWidth     )
  Q_PROPERTY(bool    symbolFilled       READ isSymbolFilled       WRITE setSymbolFilled        )
  Q_PROPERTY(QString pointsFillColor    READ pointsFillColorStr   WRITE setPointsFillColorStr  )
  Q_PROPERTY(double  pointsFillAlpha    READ pointsFillAlpha      WRITE setPointsFillAlpha     )

  // line:
  //  display, stroke
  Q_PROPERTY(bool    lines      READ isLines       WRITE setLines        )
  Q_PROPERTY(QString linesColor READ linesColorStr WRITE setLinesColorStr)
  Q_PROPERTY(double  linesAlpha READ linesAlpha    WRITE setLinesAlpha   )
  Q_PROPERTY(double  linesWidth READ linesWidth    WRITE setLinesWidth   )

 public:
  CQChartsDelaunayPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsDelaunayPlot();

  //---

  // columns
  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; updateRangeAndObjs(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; updateRangeAndObjs(); }

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; updateRangeAndObjs(); }

  //---

  // voronoi
  bool isVoronoi() const { return voronoi_; }

  double voronoiPointSize() const { return voronoiPointSize_; }
  void setVoronoiPointSize(double r) { voronoiPointSize_ = r; update(); }

  //---

  // points
  bool isPoints() const { return pointObj_->isDisplayed(); }
  void setPoints(bool b) { pointObj_->setDisplayed(b); update(); }

  QString pointsStrokeColorStr() const;
  void setPointsStrokeColorStr(const QString &str);

  QColor interpPointStrokeColor(int i, int n) const;

  double pointsStrokeAlpha() const;
  void setPointsStrokeAlpha(double a);

  QString pointsFillColorStr() const;
  void setPointsFillColorStr(const QString &str);

  QColor interpPointFillColor(int i, int n) const;

  double pointsFillAlpha() const;
  void setPointsFillAlpha(double a);

  //---

  // lines
  bool isLines() const { return lineObj_->isDisplayed(); }
  void setLines(bool b) { lineObj_->setDisplayed(b); update(); }

  QString linesColorStr() const;
  void setLinesColorStr(const QString &str);

  QColor interpLinesColor(int i, int n) const;

  double linesAlpha() const { return lineObj_->alpha(); }
  void setLinesAlpha(double a) { lineObj_->setAlpha(a); update(); }

  double linesWidth() const { return lineObj_->width(); }
  void setLinesWidth(double w) { lineObj_->setWidth(w); update(); }

  //---

  // symbol
  QString symbolName() const { return pointObj_->symbolName(); }
  void setSymbolName(const QString &s) { pointObj_->setSymbolName(s); update(); }

  CQChartsPlotSymbol::Type symbolType() const { return pointObj_->symbolType(); }
  void setSymbolType(CQChartsPlotSymbol::Type t) { pointObj_->setSymbolType(t); update(); }

  double symbolSize() const { return pointObj_->size(); }
  void setSymbolSize(double r) { pointObj_->setSize(r); update(); }

  bool isSymbolStroked() const { return pointObj_->isStroked(); }
  void setSymbolStroked(bool b) { pointObj_->setStroked(b); update(); }

  double symbolLineWidth() const { return pointObj_->lineWidth(); }
  void setSymbolLineWidth(double r) { pointObj_->setLineWidth(r); update(); }

  bool isSymbolFilled() const { return pointObj_->isFilled(); }
  void setSymbolFilled(bool b) { pointObj_->setFilled(b); update(); }

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
  int               xColumn_          { 0 };
  int               yColumn_          { 1 };
  int               nameColumn_       { -1 };
  CQChartsPointObj* pointObj_         { nullptr };
  CQChartsLineObj*  lineObj_          { nullptr };
  bool              voronoi_          { true };
  double            voronoiPointSize_ { 2 };
  int               nr_               { 0 };
  CQChartsDelaunay* delaunay_         { nullptr };
  QString           yname_;
};

#endif
