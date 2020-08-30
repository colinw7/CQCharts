#ifndef CQChartsModelColumnDataControl_H
#define CQChartsModelColumnDataControl_H

#include <QFrame>
#include <QSharedPointer>

class CQChartsModelData;
class CQChartsParamEdit;
class CQChartsLineEdit;

class QAbstractItemModel;
class QGridLayout;
class QComboBox;
class QLabel;

class CQChartsModelColumnDataControl : public QFrame {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelColumnDataControl(QWidget *parent=nullptr);

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

  void init();

 private slots:
  void setColumnData(int column);

 private slots:
  void typeChangedSlot();
  void headerTypeChangedSlot();

  void applySlot();

 private:
  struct ParamEdit {
    int                row   { 0 };
    QLabel*            label { nullptr };
    CQChartsParamEdit* edit  { nullptr };
  };

  using ParamEdits = std::vector<ParamEdit>;

  CQChartsModelData* modelData_       { nullptr };
  QFrame*            editFrame_       { nullptr };
  QGridLayout*       editLayout_      { nullptr };
  CQChartsLineEdit*  numEdit_         { nullptr };
  CQChartsLineEdit*  nameEdit_        { nullptr };
  QComboBox*         typeCombo_       { nullptr };
  QComboBox*         headerTypeCombo_ { nullptr };
  int                row_             { 0 };
  ParamEdits         paramEdits_;
};

#endif
