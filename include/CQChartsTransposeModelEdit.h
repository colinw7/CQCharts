#ifndef CQChartsTransposeModelEdit_H
#define CQChartsTransposeModelEdit_H

#include <QFrame>

class CQChartsModelData;
class CQChartsColumnsEdit;

class CQIntegerSpin;

class QCheckBox;

class CQChartsTransposeModelEdit : public QFrame {
  Q_OBJECT

 public:
  using ModelData = CQChartsModelData;

 public:
  CQChartsTransposeModelEdit(QWidget *parent);

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
};

#endif
