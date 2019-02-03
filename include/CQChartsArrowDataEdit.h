#ifndef CQChartsArrowDataEdit_H
#define CQChartsArrowDataEdit_H

#include <CQChartsData.h>
#include <QFrame>

class CQChartsLengthEdit;
class CQChartsArrowDataEditPreview;
class CQAngleSpinBox;
class QCheckBox;

class CQChartsArrowDataEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsArrowDataEdit(QWidget *parent=nullptr);

  const CQChartsArrowData &data() const { return data_; }
  void setData(const CQChartsArrowData &v) { data_ = v; }

 signals:
  void arrowDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsArrowData             data_;
  QCheckBox*                    relativeEdit_  { nullptr };
  CQChartsLengthEdit*           lengthEdit_    { nullptr };
  CQAngleSpinBox*               angleEdit_     { nullptr };
  CQAngleSpinBox*               backAngleEdit_ { nullptr };
  QCheckBox*                    fheadEdit_     { nullptr };
  QCheckBox*                    theadEdit_     { nullptr };
  QCheckBox*                    lineEndsEdit_  { nullptr };
  CQChartsLengthEdit*           lineWidthEdit_ { nullptr };
  CQChartsArrowDataEditPreview* preview_       { nullptr };
};

//---

class CQChartsArrowDataEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsArrowDataEditPreview(CQChartsArrowDataEdit *edit);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsArrowDataEdit *edit_ { nullptr };
};

#endif
