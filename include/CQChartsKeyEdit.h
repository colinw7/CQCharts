#ifndef CQChartsKeyEdit_H
#define CQChartsKeyEdit_H

#include <QDialog>

class CQChartsKey;

class CQChartsKeyEdit;

/*!
 * \brief edit key dialog
 * \ingroup Charts
 */
class CQChartsEditKeyDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsEditKeyDlg(QWidget *parent, CQChartsKey *key);

 private slots:
  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  CQChartsKey*     key_  { nullptr };
  CQChartsKeyEdit* edit_ { nullptr };
};

//---

#include <CQChartsData.h>
#include <QFrame>

class CQChartsKeyLocationEdit;
class CQChartsKeyPressBehaviorEdit;
class CQChartsAlphaEdit;
class CQChartsTextDataEdit;
class CQChartsTextBoxDataEdit;

class CQCheckBox;
class CQGroupBox;
class CQIntegerSpin;
class CQPoint2DEdit;
class CQLineEdit;

/*!
 * \brief key edit
 * \ingroup Charts
 */
class CQChartsKeyEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsKeyEdit(QWidget *parent, CQChartsKey *key);

  void applyData();

 signals:
  void keyChanged();

 private:
  void dataToWidgets();

  void connectSlots(bool b);

 private slots:
  void widgetsToData();

 private:
  CQChartsKey*                  key_                  { nullptr }; //!< parent key
  CQChartsKeyData               data_;                             //!< key data
  CQGroupBox*                   groupBox_             { nullptr }; //!< group box
  CQCheckBox*                   horizontalEdit_       { nullptr }; //!< horizontal edit
  CQCheckBox*                   autoHideEdit_         { nullptr }; //!< auto hide edit
  CQCheckBox*                   clippedEdit_          { nullptr }; //!< clipped edit
  CQCheckBox*                   aboveEdit_            { nullptr }; //!< above edit
  CQCheckBox*                   interactiveEdit_      { nullptr }; //!< interactive edit
  CQChartsKeyLocationEdit*      locationEdit_         { nullptr }; //!< location edit
  CQChartsAlphaEdit*            hiddenAlphaEdit_      { nullptr }; //!< hidden alpha edit
  CQIntegerSpin*                maxRowsEdit_          { nullptr }; //!< max rows edit
  CQChartsKeyPressBehaviorEdit* pressBehaviorEdit_    { nullptr }; //!< press behavior edit
  CQPoint2DEdit*                absolutePositionEdit_ { nullptr }; //!< absolute position edit
  CQCheckBox*                   insideXEdit_          { nullptr }; //!< inside x edit
  CQCheckBox*                   insideYEdit_          { nullptr }; //!< inside y edit
  CQIntegerSpin*                spacingEdit_          { nullptr }; //!< spacing edit
  CQCheckBox*                   flippedEdit_          { nullptr }; //!< flipped edit
  CQLineEdit*                   headerEdit_           { nullptr }; //!< header edit
  CQChartsTextDataEdit*         headerTextDataEdit_   { nullptr }; //!< header text edit
  CQLineEdit*                   scrollWidthEdit_      { nullptr }; //!< scroll width edit
  CQLineEdit*                   scrollHeightEdit_     { nullptr }; //!< scroll height edit
  CQChartsTextBoxDataEdit*      textBoxEdit_          { nullptr }; //!< text box edit
  bool                          connected_            { false };   //!< is connected
};

#endif
