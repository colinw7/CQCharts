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

  bool isHierarchical() const override { return true; }
};

#endif
