#ifndef CQChartsTextBoxDataEdit_H
#define CQChartsTextBoxDataEdit_H

#include <CQChartsData.h>
#include <QFrame>

class CQChartsTextDataEdit;
class CQChartsBoxDataEdit;
class CQChartsTextBoxDataEditPreview;
class QGroupBox;

class CQChartsTextBoxDataEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsTextBoxDataEdit(QWidget *parent=nullptr);

  const CQChartsTextBoxData &data() const { return data_; }
  void setData(const CQChartsTextBoxData &v) { data_ = v; }

 signals:
  void textBoxDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsTextBoxData             data_;
  CQChartsTextDataEdit*           textEdit_ { nullptr };
  CQChartsBoxDataEdit*            boxEdit_  { nullptr };
  CQChartsTextBoxDataEditPreview* preview_  { nullptr };
};

//---

class CQChartsTextBoxDataEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsTextBoxDataEditPreview(CQChartsTextBoxDataEdit *edit);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsTextBoxDataEdit *edit_ { nullptr };
};

#endif
