#ifndef CQChartsDendrogramPlot_H
#define CQChartsDendrogramPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDendrogram.h>

class CQChartsDendrogramPlot;
class CQChartsTextBoxObj;

//---

class CQChartsDendrogramNodeObj : public CQChartsPlotObj {
 public:
  CQChartsDendrogramNodeObj(CQChartsDendrogramPlot *plot, CQChartsDendrogram::Node *node,
                            const CQChartsGeom::BBox &rect);

  QString calcId() const override;

  CQChartsGeom::BBox textRect() const;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsDendrogramPlot*   plot_ { nullptr };
  CQChartsDendrogram::Node* node_ { nullptr };
};

//---

class CQChartsDendrogramPlotType : public CQChartsPlotType {
 public:
  CQChartsDendrogramPlotType();

  QString name() const override { return "dendrogram"; }
  QString desc() const override { return "Dendrogram"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsDendrogramPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int            nameColumn      READ nameColumn      WRITE setNameColumn     )
  Q_PROPERTY(int            valueColumn     READ valueColumn     WRITE setValueColumn    )
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

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; updateRangeAndObjs(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; updateRangeAndObjs(); }

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
  int                 nameColumn_  { 0 };
  int                 valueColumn_ { 1 };
  CQChartsDendrogram* dendrogram_  { nullptr };
  double              circleSize_  { 8 };
  double              textMargin_  { 4 };
  CQChartsTextBoxObj* nodeBoxObj_  { nullptr }; // node fill/border/text object
  CQChartsTextBoxObj* edgeBoxObj_  { nullptr }; // edge fill/border/text object
};

#endif
