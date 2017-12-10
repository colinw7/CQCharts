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

  void draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &) override;

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

  // properties
  //  point
  //   display, color, symbol, size
  //  line
  //   display, stroke
  //  margin
  //  key

  Q_PROPERTY(int     xColumn      READ xColumn        WRITE setXColumn       )
  Q_PROPERTY(int     yColumn      READ yColumn        WRITE setYColumn       )
  Q_PROPERTY(int     nameColumn   READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(bool    points       READ isPoints       WRITE setPoints        )
  Q_PROPERTY(QString pointsColor  READ pointsColorStr WRITE setPointsColorStr)
  Q_PROPERTY(bool    lines        READ isLines        WRITE setLines         )
  Q_PROPERTY(QString linesColor   READ linesColorStr  WRITE setLinesColorStr )
  Q_PROPERTY(double  linesWidth   READ linesWidth     WRITE setLinesWidth    )
  Q_PROPERTY(QString symbolName   READ symbolName     WRITE setSymbolName    )
  Q_PROPERTY(double  symbolSize   READ symbolSize     WRITE setSymbolSize    )
  Q_PROPERTY(bool    symbolFilled READ isSymbolFilled WRITE setSymbolFilled  )
  Q_PROPERTY(bool    voronoi      READ isVoronoi      WRITE setVoronoi       )

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

  // points
  bool isPoints() const { return pointObj_->isDisplayed(); }
  void setPoints(bool b) { pointObj_->setDisplayed(b); update(); }

  QString pointsColorStr() const;
  void setPointsColorStr(const QString &s);

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
  void setSymbolName(const QString &s) { pointObj_->setSymbolName(s); }

  bool isSymbolFilled() const { return pointObj_->isFilled(); }
  void setSymbolFilled(bool b) { pointObj_->setFilled(b); update(); }

  //---

  bool isVoronoi() const { return voronoi_; }
  void setVoronoi(bool b) { voronoi_ = b; update(); }

  const QString &yname() const { return yname_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  void draw(CQChartsRenderer *) override;

  void drawForeground(CQChartsRenderer *renderer) override;

 private:
  void drawDelaunay(CQChartsRenderer *p);
  void drawVoronoi (CQChartsRenderer *p);

 private:
  int               xColumn_    { 0 };
  int               yColumn_    { 1 };
  int               nameColumn_ { -1 };
  CQChartsPointObj* pointObj_   { nullptr };
  CQChartsLineObj*  lineObj_    { nullptr };
  bool              voronoi_    { true };
  CQChartsDelaunay* delaunay_   { nullptr };
  QString           yname_;
};

#endif
