#ifndef CQChartsHierSepModelEdit_H
#define CQChartsHierSepModelEdit_H

#include <QFrame>

class CQChartsModelData;
class CQChartsColumnCombo;

class QCheckBox;
class QLineEdit;

class CQChartsHierSepModelEdit : public QFrame {
  Q_OBJECT

 public:
  using ModelData = CQChartsModelData;

 public:
  CQChartsHierSepModelEdit(QWidget *parent);

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

  QCheckBox*           enabledCheck_  { nullptr }; //!< enabled checkbox
  QLineEdit*           separatorEdit_ { nullptr }; //!< separator edit
  CQChartsColumnCombo* columnEdit_    { nullptr }; //!< column edit
};

#endif
