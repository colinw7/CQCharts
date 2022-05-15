#ifndef CQChartsModelProxyControl_H
#define CQChartsModelProxyControl_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>

class CQChartsModelData;

class CQChartsBucketModelEdit;
class CQChartsHierSepModelEdit;
class CQChartsPivotModelEdit;
class CQChartsSummaryModelEdit;
class CQChartsTransposeModelEdit;

/*!
 * \brief Widget to allow user to specify model data's proxy model
 * \ingroup Charts
 */
class CQChartsModelProxyControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsModelProxyControl(QWidget *parent=nullptr);

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData) override;

 private:
  using BucketModelEdit    = CQChartsBucketModelEdit;
  using HierSepModelEdit   = CQChartsHierSepModelEdit;
  using PivotModelEdit     = CQChartsPivotModelEdit;
  using SummaryModelEdit   = CQChartsSummaryModelEdit;
  using TransposeModelEdit = CQChartsTransposeModelEdit;

  CQChartsModelData* modelData_ { nullptr };

  BucketModelEdit*    bucketEdit_    { nullptr }; //!< bucket model proxy widget
  HierSepModelEdit*   hierSepEdit_   { nullptr }; //!< hier sep model proxy widget
  PivotModelEdit*     pivotEdit_     { nullptr }; //!< pivot model proxy widget
  SummaryModelEdit*   summaryEdit_   { nullptr }; //!< summary model proxy widget
  TransposeModelEdit* transposeEdit_ { nullptr }; //!< transpose model proxy widget
};

#endif
