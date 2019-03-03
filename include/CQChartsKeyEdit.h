#ifndef CQChartsKeyEdit_H
#define CQChartsKeyEdit_H

#include <QDialog>

class CQChartsKey;

class CQChartsKeyEdit;

class CQChartsEditKeyDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsEditKeyDlg(CQChartsKey *key);

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
class CQChartsTextBoxDataEdit;

class CQCheckBox;
class CQGroupBox;
class CQIntegerSpin;
class CQPoint2DEdit;
class QLineEdit;

class CQChartsKeyEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsKeyEdit(QWidget *parent, CQChartsKey *key);

  void applyData();

 signals:
  void keyChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsKey*                  key_               { nullptr };
  CQChartsKeyData               data_;
  CQGroupBox*                   groupBox_          { nullptr };
  CQCheckBox*                   horizontalEdit_    { nullptr };
  CQCheckBox*                   autoHideEdit_      { nullptr };
  CQCheckBox*                   clippedEdit_       { nullptr };
  CQCheckBox*                   aboveEdit_         { nullptr };
  CQCheckBox*                   interactiveEdit_   { nullptr };
  CQChartsKeyLocationEdit*      locationEdit_      { nullptr };
  QLineEdit*                    headerEdit_        { nullptr };
  CQChartsAlphaEdit*            hiddenAlphaEdit_   { nullptr };
  CQIntegerSpin*                maxRowsEdit_       { nullptr };
  CQChartsKeyPressBehaviorEdit* pressBehaviorEdit_ { nullptr };
  CQPoint2DEdit*                absPositionEdit_   { nullptr };
  CQCheckBox*                   insideXEdit_       { nullptr };
  CQCheckBox*                   insideYEdit_       { nullptr };
  CQIntegerSpin*                spacingEdit_       { nullptr };
  CQCheckBox*                   flippedEdit_       { nullptr };
  QLineEdit*                    scrollWidthEdit_   { nullptr };
  QLineEdit*                    scrollHeightEdit_  { nullptr };
  CQChartsTextBoxDataEdit*      textBoxEdit_       { nullptr };
};

#endif
