#ifndef CQChartsHierPlotType_H
#define CQChartsHierPlotType_H

#include <CQChartsPlotType.h>

class CQChartsHierPlotType : public CQChartsPlotType {
 public:
  CQChartsHierPlotType();

  void addParameters() override;

  bool isHierarchical() const override { return true; }
};

#endif
