#ifndef CQChartsAxisEdit_H
#define CQChartsAxisEdit_H

#include <QDialog>

class CQChartsAxis;

class CQChartsAxisEdit;

class CQChartsEditAxisDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsEditAxisDlg(CQChartsAxis *axis);

  QSize sizeHint() const;

 private slots:
  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  CQChartsAxis*     axis_ { nullptr };
  CQChartsAxisEdit* edit_ { nullptr };
};

//---

#include <CQChartsData.h>
#include <QFrame>

class CQHRadioButtons;
class CQChartsAxisSideEdit;
class CQChartsLineDataEdit;
class CQChartsTextDataEdit;
class CQChartsFillDataEdit;

class CQCheckBox;
class CQGroupBox;
class CQIntegerSpin;
class CQRealSpin;
class QLineEdit;

class CQChartsAxisEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsAxisEdit(QWidget *parent, CQChartsAxis *axis);

  void applyData();

 signals:
  void axisChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsAxis*         axis_                  { nullptr };
  CQChartsAxisData      data_;
  CQGroupBox*           groupBox_              { nullptr };
  CQHRadioButtons*      directionEdit_         { nullptr };
  CQChartsAxisSideEdit* sideEdit_              { nullptr };
  CQCheckBox*           integralEdit_          { nullptr };
  CQCheckBox*           dateEdit_              { nullptr };
  CQCheckBox*           logEdit_               { nullptr };
  QLineEdit*            formatEdit_            { nullptr };
  CQIntegerSpin*        tickIncrementEdit_     { nullptr };
  CQRealSpin*           majorIncrementEdit_    { nullptr };
  CQRealSpin*           startEdit_             { nullptr };
  CQRealSpin*           endEdit_               { nullptr };
  CQCheckBox*           includeZeroEdit_       { nullptr };
  QLineEdit*            positionEdit_          { nullptr };
  CQChartsLineDataEdit* lineDataEdit_          { nullptr };
  CQChartsTextDataEdit* tickLabelTextDataEdit_ { nullptr };
  CQChartsTextDataEdit* labelTextDataEdit_     { nullptr };
  CQChartsLineDataEdit* majorGridLineDataEdit_ { nullptr };
  CQChartsLineDataEdit* minorGridLineDataEdit_ { nullptr };
  CQChartsFillDataEdit* gridFillDataEdit_      { nullptr };
};

#endif
