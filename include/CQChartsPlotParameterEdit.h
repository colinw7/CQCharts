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
  using PlotParameter = CQChartsPlotParameter;
  using ModelData     = CQChartsModelData;
  using Column        = CQChartsColumn;

 public:
  CQChartsColumnParameterEdit(const PlotParameter *parameter, bool isBasic);

  const PlotParameter *parameter() const { return parameter_; }

  void setModelData(ModelData *modelData);

  Column getColumn() const;
  void setColumn(const Column &column);

  bool isNumericOnly() const;
  void setNumericOnly(bool b);

 Q_SIGNALS:
  void columnChanged();

 private:
  const PlotParameter*    parameter_ { nullptr };
  CQChartsColumnCombo*    combo_     { nullptr };
  CQChartsColumnLineEdit* edit_      { nullptr };
};

//---

class CQChartsColumnsParameterEdit : public CQChartsColumnsLineEdit {
  Q_OBJECT

 public:
  using PlotParameter = CQChartsPlotParameter;

 public:
  CQChartsColumnsParameterEdit(const PlotParameter *parameter, bool isBasic);

  const PlotParameter *parameter() const { return parameter_; }

 private:
  const PlotParameter* parameter_ { nullptr };
};

//---

class CQChartsStringParameterEdit : public CQChartsLineEdit {
  Q_OBJECT

 public:
  using PlotParameter = CQChartsPlotParameter;

 public:
  CQChartsStringParameterEdit(const PlotParameter *parameter);

 private:
  const PlotParameter* parameter_ { nullptr };
};

//---

class CQChartsRealParameterEdit : public QFrame {
  Q_OBJECT

 public:
  using PlotParameter = CQChartsPlotParameter;

 public:
  CQChartsRealParameterEdit(const PlotParameter *parameter);

  double value() const;

 Q_SIGNALS:
  void valueChanged();

 private:
  const PlotParameter* parameter_ { nullptr };
  CQRealSpin*          spinEdit_  { nullptr };
  CQChartsLineEdit*    lineEdit_  { nullptr };
};

//---

class CQChartsIntParameterEdit : public QFrame {
  Q_OBJECT

 public:
  using PlotParameter = CQChartsPlotParameter;

 public:
  CQChartsIntParameterEdit(const PlotParameter *parameter);

  int value() const;

 Q_SIGNALS:
  void valueChanged();

 private:
  const PlotParameter* parameter_ { nullptr };
  CQIntegerSpin*       spinEdit_  { nullptr };
  CQChartsLineEdit*    lineEdit_  { nullptr };
};

//---

class CQChartsEnumParameterEdit : public QComboBox {
  Q_OBJECT

 public:
  using EnumParameter = CQChartsEnumParameter;

 public:
  CQChartsEnumParameterEdit(const EnumParameter *parameter);

  int currentValue() const;
  void setCurrentValue(int i);

 private:
  const EnumParameter* parameter_ { nullptr };
};

//---

class CQCheckBox;
class QCheckBox;

class CQChartsBoolParameterEdit : public QFrame {
  Q_OBJECT

 public:
  using PlotParameter = CQChartsPlotParameter;

 public:
  CQChartsBoolParameterEdit(const PlotParameter *parameter, bool choice=false);

  bool isChecked() const;
  void setChecked(bool b);

 private:
  const PlotParameter* parameter_ { nullptr };

 Q_SIGNALS:
  void stateChanged(int);

 private:
  QCheckBox*  check_  { nullptr };
  CQCheckBox* choice_ { nullptr };
};

//---

#endif
