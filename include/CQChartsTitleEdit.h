#ifndef CQChartsTitleEdit_H
#define CQChartsTitleEdit_H

#include <QDialog>

class CQChartsTitle;

class CQChartsTitleEdit;

class CQChartsEditTitleDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsEditTitleDlg(CQChartsTitle *title);

 private slots:
  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  CQChartsTitle*     title_ { nullptr };
  CQChartsTitleEdit* edit_  { nullptr };
};

//---

#include <CQChartsData.h>
#include <QFrame>

class CQChartsTitleLocationEdit;
class CQChartsPositionEdit;
class CQChartsRectEdit;
class CQChartsTextDataEdit;

class CQCheckBox;
class CQGroupBox;

class CQChartsTitleEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsTitleEdit(QWidget *parent, CQChartsTitle *title);

  void applyData();

 signals:
  void titleChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsTitle*             title_        { nullptr };
  CQChartsTitleData          data_;
  CQGroupBox*                groupBox_     { nullptr };
  CQChartsTitleLocationEdit* locationEdit_ { nullptr };
  CQChartsPositionEdit*      positionEdit_ { nullptr };
  CQChartsRectEdit*          rectEdit_     { nullptr };
  CQCheckBox*                insideEdit_   { nullptr };
  CQChartsTextDataEdit*      textEdit_     { nullptr };
};

#endif
