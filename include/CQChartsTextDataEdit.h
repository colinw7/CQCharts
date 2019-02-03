#ifndef CQChartsTextDataEdit_H
#define CQChartsTextDataEdit_H

#include <CQChartsData.h>
#include <QFrame>

class CQChartsTextDataEditPreview;
class CQFontChooser;
class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQAngleSpinBox;
class CQAlignEdit;
class QGroupBox;
class QCheckBox;

class CQChartsTextDataEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsTextDataEdit(QWidget *parent=nullptr);

  const CQChartsTextData &data() const { return data_; }
  void setData(const CQChartsTextData &v) { data_ = v; }

 signals:
  void textDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsTextData             data_;
  QGroupBox*                   groupBox_      { nullptr };
  CQFontChooser*               fontEdit_      { nullptr };
  CQChartsColorLineEdit*       colorEdit_     { nullptr };
  CQChartsAlphaEdit*           alphaEdit_     { nullptr };
  CQAngleSpinBox*              angleEdit_     { nullptr };
  QCheckBox*                   contrastEdit_  { nullptr };
  CQAlignEdit*                 alignEdit_     { nullptr };
  QCheckBox*                   formattedEdit_ { nullptr };
  QCheckBox*                   scaledEdit_    { nullptr };
  QCheckBox*                   htmlEdit_      { nullptr };
  CQChartsTextDataEditPreview* preview_       { nullptr };
};

//---

class CQChartsTextDataEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsTextDataEditPreview(CQChartsTextDataEdit *edit);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsTextDataEdit *edit_ { nullptr };
};

#endif
