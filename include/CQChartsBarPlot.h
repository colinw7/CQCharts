#ifndef CQChartsBarPlot_H
#define CQChartsBarPlot_H

#include <CQChartsGroupPlot.h>

//---

CQCHARTS_NAMED_SHAPE_DATA(Bar,bar)

// bar plot
class CQChartsBarPlot : public CQChartsGroupPlot,
 public CQChartsPlotBarShapeData<CQChartsBarPlot> {
  Q_OBJECT

  // data
  Q_PROPERTY(CQChartsColumn valueColumn  READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(QString        valueColumns READ valueColumnsStr WRITE setValueColumnsStr)
  Q_PROPERTY(CQChartsColumn colorColumn  READ colorColumn     WRITE setColorColumn    )

  // options
  Q_PROPERTY(bool           horizontal  READ isHorizontal WRITE setHorizontal )
  Q_PROPERTY(CQChartsLength margin      READ margin       WRITE setMargin     )
  Q_PROPERTY(CQChartsLength groupMargin READ groupMargin  WRITE setGroupMargin)

  // bar border
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Bar,bar)

  Q_PROPERTY(CQChartsLength cornerSize READ cornerSize WRITE setCornerSize)

  // color map
  CQCHARTS_COLOR_MAP_PROPERTIES

 public:
  CQChartsBarPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model);

  virtual ~CQChartsBarPlot();

  //---

  const CQChartsColumn &valueColumn() const { return valueColumns_.column(); }
  void setValueColumn(const CQChartsColumn &c);

  const Columns &valueColumns() const { return valueColumns_.columns(); }
  void setValueColumns(const Columns &valueColumns);

  QString valueColumnsStr() const;
  bool setValueColumnsStr(const QString &s);

  const CQChartsColumn &valueColumnAt(int i);

  int numValueColumns() const;

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

  const CQChartsLength &cornerSize() const;
  void setCornerSize(const CQChartsLength &r);

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
  CQChartsColumns valueColumns_ { 0 };     // value columns
  bool            horizontal_   { false }; // horizontal bars
  CQChartsLength  margin_       { "2px" }; // bar margin
  CQChartsLength  groupMargin_  { "4px" }; // bar group margin
  CQChartsLength  cornerSize_   { "0xp" }; // corner size
};

#endif
