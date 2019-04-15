#ifndef CQChartsAxisEdit_H
#define CQChartsAxisEdit_H

#include <QDialog>

class CQChartsAxis;

class CQChartsAxisEdit;

//! \brief edit axis dialog
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

//! \brief axis edit
class CQChartsAxisEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsAxisEdit(QWidget *parent, CQChartsAxis *axis);

  void applyData();

 signals:
  void axisChanged();

 private:
  void dataToWidgets();

  void connectSlots(bool b);

 private slots:
  void widgetsToData();

 private:
  CQChartsAxis*         axis_                  { nullptr }; //!< parent axis
  CQChartsAxisData      data_;                              //!< axis data
  CQGroupBox*           groupBox_              { nullptr }; //<! group box
  CQHRadioButtons*      directionEdit_         { nullptr }; //<! direction edit
  CQChartsAxisSideEdit* sideEdit_              { nullptr }; //<! side edit
  CQCheckBox*           integralEdit_          { nullptr }; //<! is integral edit
  CQCheckBox*           dateEdit_              { nullptr }; //<! is date edit
  CQCheckBox*           logEdit_               { nullptr }; //<! is log edit
  QLineEdit*            formatEdit_            { nullptr }; //<! format edit
  CQIntegerSpin*        tickIncrementEdit_     { nullptr }; //<! tick increment edit
  CQRealSpin*           majorIncrementEdit_    { nullptr }; //<! major increment edit
  CQRealSpin*           startEdit_             { nullptr }; //<! start edit
  CQRealSpin*           endEdit_               { nullptr }; //<! end edit
  CQCheckBox*           includeZeroEdit_       { nullptr }; //<! include zero edit
  QLineEdit*            positionEdit_          { nullptr }; //<! position edit
  CQChartsLineDataEdit* lineDataEdit_          { nullptr }; //<! line data edit
  CQChartsTextDataEdit* tickLabelTextDataEdit_ { nullptr }; //<! tick label text edit
  CQChartsTextDataEdit* labelTextDataEdit_     { nullptr }; //<! label text edit
  CQChartsLineDataEdit* majorGridLineDataEdit_ { nullptr }; //<! major grid line data edit
  CQChartsLineDataEdit* minorGridLineDataEdit_ { nullptr }; //<! minor grid line data edit
  CQChartsFillDataEdit* gridFillDataEdit_      { nullptr }; //<! grid fill data edit
  bool                  connected_             { false };   //!< is connected
};

#endif
