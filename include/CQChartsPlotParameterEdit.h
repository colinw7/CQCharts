#ifndef CQChartsPlotParameterEdit_H
#define CQChartsPlotParameterEdit_H

#include <CQChartsPlotParameter.h>
#include <CQChartsColumnsEdit.h>
#include <CQChartsLineEdit.h>

#include <QComboBox>

class CQChartsColumnCombo;
class CQChartsColumnLineEdit;
class CQChartsModelData;
class CQChartsColumn;
class CQRealSpin;
class CQIntegerSpin;

class CQChartsColumnParameterEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsColumnParameterEdit(const CQChartsPlotParameter *parameter, bool isBasic);

  const CQChartsPlotParameter *parameter() const { return parameter_; }

  void setModelData(const CQChartsModelData *modelData);

  CQChartsColumn getColumn() const;
  void setColumn(const CQChartsColumn &column);

  bool isNumericOnly() const;
  void setNumericOnly(bool b);

 signals:
  void columnChanged();

 private:
  const CQChartsPlotParameter* parameter_ { nullptr };
  CQChartsColumnCombo*         combo_     { nullptr };
  CQChartsColumnLineEdit*      edit_      { nullptr };
};

//---

class CQChartsColumnsParameterEdit : public CQChartsColumnsLineEdit {
  Q_OBJECT

 public:
  CQChartsColumnsParameterEdit(const CQChartsPlotParameter *parameter, bool isBasic);

  const CQChartsPlotParameter *parameter() const { return parameter_; }

 private:
  const CQChartsPlotParameter* parameter_ { nullptr };
};

//---

class CQChartsStringParameterEdit : public CQChartsLineEdit {
  Q_OBJECT

 public:
  CQChartsStringParameterEdit(const CQChartsPlotParameter *parameter);

 private:
  const CQChartsPlotParameter* parameter_ { nullptr };
};

//---

class CQChartsRealParameterEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsRealParameterEdit(const CQChartsPlotParameter *parameter);

  double value() const;

 signals:
  void valueChanged();

 private:
  const CQChartsPlotParameter* parameter_ { nullptr };
  CQRealSpin*                  spinEdit_  { nullptr };
  CQChartsLineEdit*            lineEdit_  { nullptr };
};

//---

class CQChartsIntParameterEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsIntParameterEdit(const CQChartsPlotParameter *parameter);

  int value() const;

 signals:
  void valueChanged();

 private:
  const CQChartsPlotParameter* parameter_ { nullptr };
  CQIntegerSpin*               spinEdit_  { nullptr };
  CQChartsLineEdit*            lineEdit_  { nullptr };
};

//---

class CQChartsEnumParameterEdit : public QComboBox {
  Q_OBJECT

 public:
  CQChartsEnumParameterEdit(const CQChartsEnumParameter *parameter);

  int currentValue() const;
  void setCurrentValue(int i);

 private:
  const CQChartsEnumParameter* parameter_ { nullptr };
};

//---

class CQCheckBox;
class QCheckBox;

class CQChartsBoolParameterEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsBoolParameterEdit(const CQChartsPlotParameter *parameter, bool choice=false);

  bool isChecked() const;
  void setChecked(bool b);

 private:
  const CQChartsPlotParameter* parameter_ { nullptr };

 signals:
  void stateChanged(int);

 private:
  QCheckBox*  check_  { nullptr };
  CQCheckBox* choice_ { nullptr };
};

//---

#endif
