#ifndef CQChartsEditModelDlg_H
#define CQChartsEditModelDlg_H

#include <QDialog>

class CQCharts;
class CQChartsModelData;
class CQChartsModelDataWidget;
class CQChartsModelControl;

class QAbstractItemModel;

/*!
 * \brief edit model dialog
 * \ingroup Charts
 */
class CQChartsEditModelDlg : public QDialog {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsEditModelDlg(CQCharts *charts, CQChartsModelData *modelData=nullptr);
 ~CQChartsEditModelDlg();

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

  QAbstractItemModel *model() const { return model_.data(); }

  QSize sizeHint() const override;

 private slots:
  void currentModelChangedSlot();

  void writeSlot();
  void plotSlot();
  void cancelSlot();

 private:
  void init();

  void connectSlots(bool b);

//bool writeCSVModel(const QString &fileName);
//void writeModelCmds();

 private:
  CQCharts*                charts_       { nullptr }; //!< parent charts
  CQChartsModelData*       modelData_    { nullptr }; //!< model data
  ModelP                   model_;                    //!< model
  CQChartsModelDataWidget* modelWidget_  { nullptr }; //!< model data widget
  CQChartsModelControl*    modelControl_ { nullptr }; //!< model control widget
};

#endif
