#ifndef CQChartsHierPlot_H
#define CQChartsHierPlot_H

#include <CQChartsHierPlotType.h>
#include <CQChartsPlot.h>

//----

class CQChartsHierPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumns nameColumns READ nameColumns WRITE setNameColumns)
  Q_PROPERTY(CQChartsColumn  valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(CQChartsColumn  colorColumn READ colorColumn WRITE setColorColumn)
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

  const QString &separator() const { return separator_; }
  void setSeparator(const QString &s) { separator_ = s; }

  //---

  void addProperties() override;

 protected:
  CQChartsColumns nameColumns_;         // multiple name columns
  CQChartsColumn  valueColumn_;         // value column
  QString         separator_   { "/" }; // hierarchical name separator
};

#endif
