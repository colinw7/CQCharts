#ifndef CQChartsModelExprControl_H
#define CQChartsModelExprControl_H

#include <QFrame>
#include <QSharedPointer>

class CQChartsModelData;
class CQChartsColumnCombo;
class CQChartsColumnTypeCombo;
class CQChartsLineEdit;

class QAbstractItemModel;
class QRadioButton;
class QLabel;

class CQChartsModelExprControl : public QFrame {
  Q_OBJECT

 public:
  enum class Mode {
    ADD,
    REMOVE,
    MODIFY
  };

  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelExprControl(QWidget *parent=nullptr);

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

  void init();

 private slots:
  void modeSlot();

  void applySlot();

 private:
  CQChartsModelData*       modelData_     { nullptr };
  Mode                     exprMode_      { Mode::ADD };
  QRadioButton*            addRadio_      { nullptr };
  QRadioButton*            removeRadio_   { nullptr };
  QRadioButton*            modifyRadio_   { nullptr };
  CQChartsLineEdit*        valueEdit_     { nullptr };
  QLabel*                  columnLabel_   { nullptr };
  CQChartsColumnCombo*     columnNumEdit_ { nullptr };
//CQChartsLineEdit*        columnEdit_    { nullptr };
  CQChartsLineEdit*        nameEdit_      { nullptr };
  QLabel*                  typeLabel_     { nullptr };
//CQChartsLineEdit*        typeEdit_      { nullptr };
  CQChartsColumnTypeCombo* typeCombo_     { nullptr };
};

#endif
