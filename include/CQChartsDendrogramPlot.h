#ifndef CQChartsDendrogramPlot_H
#define CQChartsDendrogramPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDendrogram.h>

class CQChartsDendrogramPlot;

//---

class CQChartsDendrogramPlotType : public CQChartsPlotType {
 public:
  CQChartsDendrogramPlotType();

  QString name() const override { return "dendrogram"; }
  QString desc() const override { return "Dendrogram"; }

  Dimension dimension() const override { return Dimension::NONE; }

  void addParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsDendrogramNodeObj : public CQChartsPlotObj {
 public:
  CQChartsDendrogramNodeObj(CQChartsDendrogramPlot *plot, CQChartsDendrogram::Node *node,
                            const CQChartsGeom::BBox &rect);

  QString calcId() const override;

  CQChartsGeom::BBox textRect() const;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsDendrogramPlot*   plot_ { nullptr };
  CQChartsDendrogram::Node* node_ { nullptr };
};

//---

class CQChartsDendrogramPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn nameColumn      READ nameColumn      WRITE setNameColumn     )
  Q_PROPERTY(CQChartsColumn valueColumn     READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(double         circleSize      READ circleSize      WRITE setCircleSize     )
  Q_PROPERTY(double         textMargin      READ textMargin      WRITE setTextMargin     )
  Q_PROPERTY(CQChartsColor  nodeBorderColor READ nodeBorderColor WRITE setNodeBorderColor)
  Q_PROPERTY(double         nodeBorderAlpha READ nodeBorderAlpha WRITE setNodeBorderAlpha)
  Q_PROPERTY(CQChartsLength nodeBorderWidth READ nodeBorderWidth WRITE setNodeBorderWidth)
  Q_PROPERTY(CQChartsColor  nodeFillColor   READ nodeFillColor   WRITE setNodeFillColor  )
  Q_PROPERTY(double         nodeFillAlpha   READ nodeFillAlpha   WRITE setNodeFillAlpha  )
  Q_PROPERTY(CQChartsColor  edgeLineColor   READ edgeLineColor   WRITE setEdgeLineColor  )
  Q_PROPERTY(double         edgeLineAlpha   READ edgeLineAlpha   WRITE setEdgeLineAlpha  )
  Q_PROPERTY(CQChartsLength edgeLineWidth   READ edgeLineWidth   WRITE setEdgeLineWidth  )
  Q_PROPERTY(QFont          textFont        READ textFont        WRITE setTextFont       )
  Q_PROPERTY(CQChartsColor  textColor       READ textColor       WRITE setTextColor      )
  Q_PROPERTY(double         textAlpha       READ textAlpha       WRITE setTextAlpha      )

 public:
  CQChartsDendrogramPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsDendrogramPlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c) { nameColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c) { valueColumn_ = c; updateRangeAndObjs(); }

  //---

  double circleSize() const { return circleSize_; }
  void setCircleSize(double r) { circleSize_ = r; update(); }

  double textMargin() const { return textMargin_; }
  void setTextMargin(double r) { textMargin_ = r; update(); }

  //---

  const CQChartsColor &nodeBorderColor() const;
  void setNodeBorderColor(const CQChartsColor &c);

  QColor interpNodeBorderColor(int i, int n) const;

  double nodeBorderAlpha() const;
  void setNodeBorderAlpha(double a);

  const CQChartsLength &nodeBorderWidth() const;
  void setNodeBorderWidth(const CQChartsLength &l);

  //---

  const CQChartsColor &nodeFillColor() const;
  void setNodeFillColor(const CQChartsColor &c);

  QColor interpNodeFillColor(int i, int n) const;

  double nodeFillAlpha() const;
  void setNodeFillAlpha(double a);

  //---

  const CQChartsColor &edgeLineColor() const;
  void setEdgeLineColor(const CQChartsColor &c);

  QColor interpEdgeLineColor(int i, int n) const;

  double edgeLineAlpha() const;
  void setEdgeLineAlpha(double a);

  const CQChartsLength &edgeLineWidth() const;
  void setEdgeLineWidth(const CQChartsLength &l);

  //---

  const QFont &textFont() const;
  void setTextFont(const QFont &f);

  const CQChartsColor &textColor() const;
  void setTextColor(const CQChartsColor &c);

  QColor interpTextColor(int i, int n) const;

  double textAlpha() const;
  void setTextAlpha(double a);

  //---

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void addNameValue(const QString &name, double value);

  bool initObjs() override;

  //---

  void addNodeObjs(CQChartsDendrogram::HierNode *hier, int depth);
  void addNodeObj (CQChartsDendrogram::Node *node);

  //---

  bool selectPress(const CQChartsGeom::Point &p, ModSelect modSelect) override;

  //---

  void draw(QPainter *) override;

  void drawNodes(QPainter *painter, CQChartsDendrogram::HierNode *hier, int depth);

  void drawNode(QPainter *painter, CQChartsDendrogram::HierNode *hier,
                CQChartsDendrogram::Node *node);

 private:
  CQChartsColumn      nameColumn_  { 0 };       // name column
  CQChartsColumn      valueColumn_ { 1 };       // value column
  CQChartsDendrogram* dendrogram_  { nullptr }; // dendogram class
  double              circleSize_  { 8 };       // circle size
  double              textMargin_  { 4 };       // text margin
  CQChartsShapeData   nodeData_;                // node fill/border data
  CQChartsStrokeData  edgeData_;                // edge stroke data
  CQChartsTextData    textData_;                // text data
};

#endif
