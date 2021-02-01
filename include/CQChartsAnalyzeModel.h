#ifndef CQChartsAnalyzeModel_H
#define CQChartsAnalyzeModel_H

#include <CQChartsAnalyzeModelData.h>
#include <CQChartsColumn.h>
#include <map>

class CQCharts;
class CQChartsPlotType;
class CQChartsPlot;
class CQChartsModelData;

/*!
 * \brief analyze a model to auto determine best plot type and columns
 * \ingroup Charts
 */
class CQChartsAnalyzeModel {
 public:
  using ModelData        = CQChartsModelData;
  using PlotType         = CQChartsPlotType;
  using Plot             = CQChartsPlot;
  using AnalyzeModelData = CQChartsAnalyzeModelData;

 public:
  using TypeAnalyzeModelData = std::map<QString, AnalyzeModelData>;

 public:
  CQChartsAnalyzeModel(CQCharts *charts, ModelData *modelData);

  void analyze();

  const TypeAnalyzeModelData &typeAnalyzeModelData() const { return typeAnalyzeModelData_; }

  bool analyzeType(PlotType *type);

  const AnalyzeModelData &analyzeModelData(const PlotType *type);

  void print() const;

  void initPlot(Plot *plot);

 private:
  bool analyzeType(PlotType *type, AnalyzeModelData &analyzeModelData);

 private:
  CQCharts*            charts_                { nullptr }; //!< charts
  ModelData*           modelData_             { nullptr }; //!< model data
  TypeAnalyzeModelData typeAnalyzeModelData_;              //!< type's parameter name column
};

#endif
