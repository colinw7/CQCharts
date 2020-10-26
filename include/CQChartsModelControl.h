#ifndef CQChartsModelControl_H
#define CQChartsModelControl_H

#include <QFrame>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQChartsModelExprControl;
class CQChartsModelFilterControl;
#ifdef CQCHARTS_FOLDED_MODEL
class CQChartsModelFoldControl;
#endif
class CQChartsModelFlattenControl;
class CQChartsModelColumnDataControl;

class CQCharts;
class CQChartsModelData;

class CQPropertyViewModel;
class CQPropertyViewTree;

/*!
 * \brief Model Control Widget
 * \ingroup Charts
 */
class CQChartsModelControl : public QFrame {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelControl(CQCharts *charts, CQChartsModelData *modelData=nullptr);
 ~CQChartsModelControl();

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

 public slots:
  void updateCurrentModel();
  void filterTextSlot(const QString &);

 private:
  CQChartsModelExprControl       *addExprFrame();
  CQChartsModelFilterControl     *addFilterFrame();
#ifdef CQCHARTS_FOLDED_MODEL
  CQChartsModelFoldControl       *addFoldFrame();
#endif
  CQChartsModelFlattenControl    *addFlattenFrame();
  CQChartsModelColumnDataControl *addColumnDataFrame();

  //---

  QFrame *addPropertiesFrame();

 private:
  CQCharts*                       charts_          { nullptr };
  CQChartsModelData*              modelData_       { nullptr };
  CQChartsModelExprControl*       exprFrame_       { nullptr };
  CQChartsModelFilterControl*     filterFrame_     { nullptr };
#ifdef CQCHARTS_FOLDED_MODEL
  CQChartsModelFoldControl*       foldFrame_       { nullptr };
#endif
  CQChartsModelFlattenControl*    flattenFrame_    { nullptr };
  CQChartsModelColumnDataControl* columnDataFrame_ { nullptr };
  CQPropertyViewModel*            propertyModel_   { nullptr };
  CQPropertyViewTree*             propertyTree_    { nullptr };
};

#endif
