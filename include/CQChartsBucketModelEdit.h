#ifndef CQChartsBucketModelEdit_H
#define CQChartsBucketModelEdit_H

#include <QFrame>

class CQChartsModelData;
class CQChartsColumnCombo;

class CQRealSpin;
class CQIntegerSpin;

class QCheckBox;
class QComboBox;

class CQChartsBucketModelEdit : public QFrame {
  Q_OBJECT

 public:
  using ModelData = CQChartsModelData;

 public:
  CQChartsBucketModelEdit(QWidget *parent);

  const ModelData *modelData() const { return modelData_; }
  void setModelData(ModelData *modelData);

  void connectSlots(bool b);

 private slots:
  void enabledSlot();
  void columnSlot();
  void typeSlot(int);

  void updateModelFromWidgets();
  void updateWidgetsFromModel();

 private:
  ModelData* modelData_ { nullptr };

  bool numeric_  { false };
  bool integral_ { false };

  QCheckBox*           enabledCheck_ { nullptr }; //!< enabled checkbox
  QComboBox*           typeCombo_    { nullptr }; //!< type combo
  CQChartsColumnCombo* columnEdit_   { nullptr }; //!< column edit
  CQRealSpin*          startEdit_    { nullptr }; //!< start edit
  CQRealSpin*          deltaEdit_    { nullptr }; //!< delta edit
  CQRealSpin*          minEdit_      { nullptr }; //!< min edit
  CQRealSpin*          maxEdit_      { nullptr }; //!< max edit
  CQIntegerSpin*       countEdit_    { nullptr }; //!< count edit
  QCheckBox*           multiCheck_   { nullptr }; //!< multi column checkbox
};

#endif
