#ifndef CQChartsModelDetailsTable_H
#define CQChartsModelDetailsTable_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>

class CQCharts;
class CQChartsModelData;
class CQTableWidget;

/*!
 * \brief Table showing Details/Summary of Model Data
 * \ingroup Charts
 */
class CQChartsModelDetailsTable : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(bool flip       READ isFlip       WRITE setFlip)
  Q_PROPERTY(bool autoUpdate READ isAutoUpdate WRITE setAutoUpdate)
  Q_PROPERTY(int  modelInd   READ modelInd     WRITE setModelInd)

 public:
  using ModelData = CQChartsModelData;

 public:
  CQChartsModelDetailsTable(ModelData *modelData=nullptr);

  void setCharts(CQCharts *charts) override;

  //! get/set model data
  ModelData *modelData() const { return modelData_; }
  void setModelData(ModelData *modelData) override;

  //! get/set flip
  bool isFlip() const { return flip_; }
  void setFlip(bool b);

  //! get/set auto update
  bool isAutoUpdate() const { return autoUpdate_; }
  void setAutoUpdate(bool b) { autoUpdate_ = b; }

  //! get/set model ind
  int modelInd() const;
  void setModelInd(int i);

  void checkedUpdate(bool force=false);

 private:
  void update();

  void contextMenuEvent(QContextMenuEvent *e) override;

 private Q_SLOTS:
  void modelChangedSlot();
  void modelTypeChangedSlot(int);

  void flipSlot(bool b);

 private:
  CQCharts*      charts_     { nullptr };
  ModelData*     modelData_  { nullptr };
  CQTableWidget* table_      { nullptr };
  bool           flip_       { false };
  bool           autoUpdate_ { true };
  bool           valid_      { false };
};

#endif
