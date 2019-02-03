#ifndef CQChartsBoxDataEdit_H
#define CQChartsBoxDataEdit_H

#include <CQChartsData.h>
#include <QFrame>

class CQChartsShapeDataEdit;
class CQChartsSidesEdit;
class CQChartsBoxDataEditPreview;
class CQRealSpin;
class QGroupBox;

class CQChartsBoxDataEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsBoxDataEdit(QWidget *parent=nullptr);

  const CQChartsBoxData &data() const { return data_; }
  void setData(const CQChartsBoxData &v) { data_ = v; }

 signals:
  void boxDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsBoxData             data_;
  QGroupBox*                  groupBox_    { nullptr };
  CQRealSpin*                 marginEdit_  { nullptr };
  CQRealSpin*                 paddingEdit_ { nullptr };
  CQChartsShapeDataEdit*      shapeEdit_   { nullptr };
  CQChartsSidesEdit*          sidesEdit_   { nullptr };
  CQChartsBoxDataEditPreview* preview_     { nullptr };
};

//---

class CQChartsBoxDataEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsBoxDataEditPreview(CQChartsBoxDataEdit *edit);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsBoxDataEdit *edit_ { nullptr };
};

#endif
