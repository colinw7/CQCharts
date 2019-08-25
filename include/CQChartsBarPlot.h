#ifndef CQChartsBarPlot_H
#define CQChartsBarPlot_H

#include <CQChartsGroupPlot.h>

class CQChartsDataLabel;

//---

/*!
 * \brief base class for Bar type plots (bar chart and distribution)
 * \ingroup Charts
 */
class CQChartsBarPlot : public CQChartsGroupPlot,
 public CQChartsObjBarShapeData<CQChartsBarPlot> {
  Q_OBJECT

  // data
  Q_PROPERTY(CQChartsColumns valueColumns READ valueColumns WRITE setValueColumns)

  // options
  Q_PROPERTY(bool           horizontal  READ isHorizontal WRITE setHorizontal )
  Q_PROPERTY(CQChartsLength margin      READ margin       WRITE setMargin     )
  Q_PROPERTY(CQChartsLength groupMargin READ groupMargin  WRITE setGroupMargin)

  // bar fill, stroke
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Bar,bar)

 public:
  CQChartsBarPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model);

  virtual ~CQChartsBarPlot();

  //---

  // value columns
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

  // data label
  const CQChartsDataLabel *dataLabel() const { return dataLabel_; }
  CQChartsDataLabel *dataLabel() { return dataLabel_; }

  //---

  void addProperties() override;

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  void write(std::ostream &os, const QString &varName="",
             const QString &modelName="") const override;

 public slots:
  // set horizontal
  virtual void setHorizontal(bool b);

 protected:
  CQChartsColumns    valueColumns_;             //!< value columns
  bool               horizontal_   { false };   //!< horizontal bars
  CQChartsLength     margin_       { "2px" };   //!< bar margin
  CQChartsLength     groupMargin_  { "4px" };   //!< bar group margin
  CQChartsDataLabel* dataLabel_    { nullptr }; //!< data label data
};

#endif
