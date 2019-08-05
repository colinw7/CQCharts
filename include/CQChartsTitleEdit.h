#ifndef CQChartsTitleEdit_H
#define CQChartsTitleEdit_H

#include <QDialog>

class CQChartsTitle;

class CQChartsTitleEdit;

/*!
 * \brief edit title dialog
 * \ingroup Charts
 */
class CQChartsEditTitleDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsEditTitleDlg(QWidget *parent, CQChartsTitle *title);

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

/*!
 * \brief title edit
 * \ingroup Charts
 */
class CQChartsTitleEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsTitleEdit(QWidget *parent, CQChartsTitle *title);

  void applyData();

 signals:
  void titleChanged();

 private:
  void dataToWidgets();

  void connectSlots(bool b);

 private slots:
  void widgetsToData();

 private:
  CQChartsTitle*             title_        { nullptr }; //!< title
  CQChartsTitleData          data_;                     //!< title data
  CQGroupBox*                groupBox_     { nullptr }; //!< group box
  CQChartsTitleLocationEdit* locationEdit_ { nullptr }; //!< location edit
  CQChartsPositionEdit*      positionEdit_ { nullptr }; //!< position edit
  CQChartsRectEdit*          rectEdit_     { nullptr }; //!< rect edit
  CQCheckBox*                insideEdit_   { nullptr }; //!< inside edit
  CQChartsTextDataEdit*      textEdit_     { nullptr }; //!< text edit
  bool                       connected_    { false };   //!< is connected
};

#endif
