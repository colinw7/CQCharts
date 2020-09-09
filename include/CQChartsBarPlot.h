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
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Bar, bar)

 public:
  using DataLabel = CQChartsDataLabel;

 public:
  CQChartsBarPlot(View *view, PlotType *plotType, const ModelP &model);

  virtual ~CQChartsBarPlot();

  //---

  // value columns
  const Columns &valueColumns() const { return valueColumns_; }
  void setValueColumns(const Columns &c);

  //---

  bool isHorizontal() const { return horizontal_; }

  //---

  // bar margin
  const Length &margin() const { return margin_; }
  void setMargin(const Length &l);

  // group margin
  const Length &groupMargin() const { return groupMargin_; }
  void setGroupMargin(const Length &l);

  //---

  bool allowZoomX() const override { return ! isHorizontal(); }
  bool allowZoomY() const override { return   isHorizontal(); }

  bool allowPanX() const override { return ! isHorizontal(); }
  bool allowPanY() const override { return   isHorizontal(); }

  //---

  // data label
  const DataLabel *dataLabel() const { return dataLabel_; }
  DataLabel *dataLabel() { return dataLabel_; }

  //---

  void addProperties() override;

  void addBoxProperties();

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

 public slots:
  // set horizontal
  virtual void setHorizontal(bool b);

 protected:
  Columns    valueColumns_;             //!< value columns
  bool       horizontal_   { false };   //!< horizontal bars
  Length     margin_       { "2px" };   //!< bar margin
  Length     groupMargin_  { "4px" };   //!< bar group margin
  DataLabel* dataLabel_    { nullptr }; //!< data label data
};

#endif
