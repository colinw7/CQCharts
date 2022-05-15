#ifndef CQChartsPivotModelEdit_H
#define CQChartsPivotModelEdit_H

#include <QFrame>

class CQChartsModelData;
class CQChartsColumnsEdit;

class CQIntegerSpin;

class QCheckBox;

class CQChartsPivotModelEdit : public QFrame {
  Q_OBJECT

 public:
  using ModelData = CQChartsModelData;

 public:
  CQChartsPivotModelEdit(QWidget *parent);

  const ModelData *modelData() const { return modelData_; }
  void setModelData(ModelData *modelData);

 private:
  void connectSlots(bool b);

 private slots:
  void enabledSlot();

  void updateModelFromWidgets();
  void updateWidgetsFromModel();

 private:
  ModelData* modelData_ { nullptr };

  QCheckBox* enabledCheck_ { nullptr }; //!< enabled checkbox

  CQChartsColumnsEdit* hColumnsEdit_ { nullptr };
  CQChartsColumnsEdit* vColumnsEdit_ { nullptr };
  CQChartsColumnsEdit* dColumnsEdit_ { nullptr };
};

#endif
