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
  Q_PROPERTY(CQChartsColumn  colorColumn  READ colorColumn  WRITE setColorColumn )

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

  const CQChartsColumn &colorColumn() const { return valueSetColumn("color"); }
  void setColorColumn(const CQChartsColumn &c) {
    (void) setValueSetColumn("color", c); updateRangeAndObjs(); }

  bool isColorMapped() const { return isValueSetMapped("color"); }
  void setColorMapped(bool b) { setValueSetMapped("color", b); updateObjs(); }

  double colorMapMin() const { return valueSetMapMin("color"); }
  void setColorMapMin(double r) { setValueSetMapMin("color", r); updateObjs(); }

  double colorMapMax() const { return valueSetMapMax("color"); }
  void setColorMapMax(double r) { setValueSetMapMax("color", r); updateObjs(); }

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
  CQChartsColumns valueColumns_ { "0" };   // value columns
  bool            horizontal_   { false }; // horizontal bars
  CQChartsLength  margin_       { "2px" }; // bar margin
  CQChartsLength  groupMargin_  { "4px" }; // bar group margin
};

#endif
