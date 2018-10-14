#ifndef CQChartsBarPlot_H
#define CQChartsBarPlot_H

#include <CQChartsGroupPlot.h>

//---

CQCHARTS_NAMED_SHAPE_DATA(Bar,bar)

// bar plot
class CQChartsBarPlot : public CQChartsGroupPlot,
 public CQChartsObjBarShapeData<CQChartsBarPlot> {
  Q_OBJECT

  // data
  Q_PROPERTY(CQChartsColumns valueColumns READ valueColumns WRITE setValueColumns)

  // options
  Q_PROPERTY(bool           horizontal  READ isHorizontal WRITE setHorizontal )
  Q_PROPERTY(CQChartsLength margin      READ margin       WRITE setMargin     )
  Q_PROPERTY(CQChartsLength groupMargin READ groupMargin  WRITE setGroupMargin)

  // bar border
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Bar,bar)

  // color map
  CQCHARTS_COLOR_MAP_PROPERTIES

 public:
  CQChartsBarPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model);

  virtual ~CQChartsBarPlot();

  //---

  const CQChartsColumns &valueColumns() const { return valueColumns_; }
  void setValueColumns(const CQChartsColumns &c);

  //---

  bool isHorizontal() const { return horizontal_; }

  //---

  // bar margin
  const CQChartsLength &margin() const { return margin_; }
  void setMargin(const CQChartsLength &l);

  // group margin
  const CQChartsLength &groupMargin() const { return groupMargin_; }
  void setGroupMargin(const CQChartsLength &l);

  //---

  bool allowZoomX() const override { return ! isHorizontal(); }
  bool allowZoomY() const override { return   isHorizontal(); }

  bool allowPanX() const override { return ! isHorizontal(); }
  bool allowPanY() const override { return   isHorizontal(); }

  //---

  void addProperties() override;

  //---

  bool probe(ProbeData &probeData) const override;

 public slots:
  // set horizontal
  virtual void setHorizontal(bool b);

 protected:
  CQChartsColumns valueColumns_;           // value columns
  bool            horizontal_   { false }; // horizontal bars
  CQChartsLength  margin_       { "2px" }; // bar margin
  CQChartsLength  groupMargin_  { "4px" }; // bar group margin
};

#endif
