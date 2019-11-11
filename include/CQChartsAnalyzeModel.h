#ifndef CQChartsAnalyzeModel_H
#define CQChartsAnalyzeModel_H

#include <CQChartsAnalyzeModelData.h>
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
  using TypeAnalyzeModelData = std::map<QString,CQChartsAnalyzeModelData>;

 public:
  CQChartsAnalyzeModel(CQCharts *charts, CQChartsModelData *modelData);

  void analyze();

  const TypeAnalyzeModelData &typeAnalyzeModelData() const { return typeAnalyzeModelData_; }

  bool analyzeType(CQChartsPlotType *type);

  const CQChartsAnalyzeModelData &analyzeModelData(const CQChartsPlotType *type);

  void print() const;

 private:
  bool analyzeType(CQChartsPlotType *type, CQChartsAnalyzeModelData &analyzeModelData);

 private:
  CQCharts*            charts_                { nullptr }; //!< charts
  CQChartsModelData*   modelData_             { nullptr }; //!< model data
  TypeAnalyzeModelData typeAnalyzeModelData_;              //!< type's parameter name column
};

#endif
