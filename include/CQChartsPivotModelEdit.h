#ifndef CQChartsPivotModelEdit_H
#define CQChartsPivotModelEdit_H

#include <QFrame>

class CQChartsModelData;
class CQChartsColumnsEdit;

class CQIntegerSpin;

class QCheckBox;
class QLineEdit;

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

 private Q_SLOTS:
  void enabledSlot();

  void updateModelFromWidgets();
  void updateWidgetsFromModel();

 private:
  ModelData* modelData_ { nullptr };

  QCheckBox* enabledCheck_ { nullptr }; //!< enabled checkbox

  CQChartsColumnsEdit* hColumnsEdit_   { nullptr };
  CQChartsColumnsEdit* vColumnsEdit_   { nullptr };
  CQChartsColumnsEdit* dColumnsEdit_   { nullptr };
  QLineEdit*           valueTypesEdit_ { nullptr };
  QCheckBox*           includeTotals_  { nullptr };
  QLineEdit*           fillValue_      { nullptr };
  QLineEdit*           separatorEdit_  { nullptr };
};

#endif
