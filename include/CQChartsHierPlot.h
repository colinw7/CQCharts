#ifndef CQChartsHierPlot_H
#define CQChartsHierPlot_H

#include <CQChartsPlot.h>

class CQChartsHierPlotType : public CQChartsPlotType {
 public:
  CQChartsHierPlotType();

  void addParameters() override;
};

//----

class CQChartsHierPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(QString        nameColumns READ nameColumnsStr WRITE setNameColumnsStr)
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn    WRITE setValueColumn   )
  Q_PROPERTY(CQChartsColumn colorColumn READ colorColumn    WRITE setColorColumn   )
  Q_PROPERTY(QString        separator   READ separator      WRITE setSeparator     )

 public:
  CQChartsHierPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model);

 ~CQChartsHierPlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const Columns &nameColumns() const { return nameColumns_; }
  void setNameColumns(const Columns &nameColumns);

  QString nameColumnsStr() const;
  bool setNameColumnsStr(const QString &s);

  int numNameColumns() const { return nameColumns_.size(); }

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c);

  //---

  const CQChartsColumn &colorColumn() const { return valueSetColumn("color"); }
  void setColorColumn(const CQChartsColumn &c) {
    setValueSetColumn("color", c); updateRangeAndObjs(); }

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
  CQChartsColumn nameColumn_  { 0 };   // name column
  Columns        nameColumns_;         // multiple name columns
  CQChartsColumn valueColumn_;         // value column
  QString        separator_   { "/" }; // hierarchical name separator
};

#endif
