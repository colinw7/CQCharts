#ifndef CQChartsHierPlot_H
#define CQChartsHierPlot_H

#include <CQChartsHierPlotType.h>
#include <CQChartsPlot.h>

//----

/*!
 * \brief Hierarchical base plot
 * \ingroup Charts
 */
class CQChartsHierPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumns nameColumns READ nameColumns WRITE setNameColumns)
  Q_PROPERTY(CQChartsColumn  valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(QString         separator   READ separator   WRITE setSeparator  )

 public:
  CQChartsHierPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model);

 ~CQChartsHierPlot();

  //---

  const CQChartsColumns &nameColumns() const { return nameColumns_; }
  void setNameColumns(const CQChartsColumns &c);

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c);

  //---

  const QString &separator() const { return separator_; }
  void setSeparator(const QString &s) { separator_ = s; }

  //---

  void addProperties() override;

 protected:
  CQChartsColumns nameColumns_;         //!< multiple name columns
  CQChartsColumn  valueColumn_;         //!< value column
  QString         separator_   { "/" }; //!< hierarchical name separator
};

#endif
