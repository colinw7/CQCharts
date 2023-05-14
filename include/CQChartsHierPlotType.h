#ifndef CQChartsHierPlotType_H
#define CQChartsHierPlotType_H

#include <CQChartsPlotType.h>

/*!
 * \brief Hierarchical Plot base plot type
 * \ingroup Charts
 */
class CQChartsHierPlotType : public CQChartsPlotType {
 public:
  CQChartsHierPlotType();

  void addParameters() override;

  void addHierParameters(const QString &title="Hierarchy");

  virtual void addExtraHierParameters() { }

  bool isHierarchical() const override { return true; }

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;
};

#endif
