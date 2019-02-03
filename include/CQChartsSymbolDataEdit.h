#ifndef CQChartsSymbolDataEdit_H
#define CQChartsSymbolDataEdit_H

#include <CQChartsData.h>
#include <QFrame>

class CQChartsSymbolEdit;
class CQChartsLengthEdit;
class CQChartsStrokeDataEdit;
class CQChartsFillDataEdit;
class CQChartsSymbolDataEditPreview;
class QGroupBox;

class CQChartsSymbolDataEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsSymbolDataEdit(QWidget *parent=nullptr);

  const CQChartsSymbolData &data() const { return data_; }
  void setData(const CQChartsSymbolData &v) { data_ = v; }

 signals:
  void symbolDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsSymbolData             data_;
  QGroupBox*                     groupBox_    { nullptr };
  CQChartsSymbolEdit*            symbolEdit_  { nullptr };
  CQChartsLengthEdit*            sizeEdit_    { nullptr };
  CQChartsStrokeDataEdit*        strokeEdit_  { nullptr };
  CQChartsFillDataEdit*          fillEdit_    { nullptr };
  CQChartsSymbolDataEditPreview* preview_     { nullptr };
};

//---

class CQChartsSymbolDataEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsSymbolDataEditPreview(CQChartsSymbolDataEdit *edit);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsSymbolDataEdit *edit_ { nullptr };
};

#endif
