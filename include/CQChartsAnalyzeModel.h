#ifndef CQChartsAnalyzeModel_H
#define CQChartsAnalyzeModel_H

#include <CQChartsColumn.h>
#include <map>

class CQCharts;
class CQChartsPlotType;
class CQChartsModelData;

/*!
 * \brief analyze a model to auto determine best plot type and columns
 * \ingroup Charts
 */
class CQChartsAnalyzeModel {
 public:
  using NameColumns     = std::map<QString,CQChartsColumn>;
  using TypeNameColumns = std::map<QString,NameColumns>;

 public:
  CQChartsAnalyzeModel(CQCharts *charts, CQChartsModelData *modelData);

  void analyze();

  bool analyzeType(CQChartsPlotType *type);

  const TypeNameColumns &typeNameColumns() { return typeNameColumns_; }

  void print() const;

 private:
  CQCharts*          charts_    { nullptr }; //!< charts
  CQChartsModelData* modelData_ { nullptr }; //!< model data
  TypeNameColumns    typeNameColumns_;       //!< typed columns
};

#endif
