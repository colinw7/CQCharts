#ifndef CQPOINT2D_EDIT_H
#define CQPOINT2D_EDIT_H

#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <CPoint2D.h>

class CQPoint2DEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool spin     READ isSpin   WRITE setSpin    )
  Q_PROPERTY(int  decimals READ decimals WRITE setDecimals)

 public:
  CQPoint2DEdit(QWidget *parent, const CPoint2D &value=CPoint2D(0,0), bool spin=false);
  CQPoint2DEdit(const CPoint2D &value=CPoint2D(0,0), bool spin=false);

  CQPoint2DEdit(QWidget *parent, const QPointF &value, bool spin=false);
  CQPoint2DEdit(const QPointF &value, bool spin=false);

  virtual ~CQPoint2DEdit() { }

  bool isSpin() const { return spin_; }
  void setSpin(bool b);

  int decimals() const { return decimals_; }
  void setDecimals(int i);

  void setValue(const CPoint2D &point);
  void setValue(const QPointF &point);

  void setMinimum(const CPoint2D &point);
  void setMaximum(const CPoint2D &point);
  void setStep(const CPoint2D &point);

  const CPoint2D &getValue() const;
  QPointF getQValue() const;

 private:
  void init(const CPoint2D &value);

 private slots:
  void editingFinishedI();

 signals:
  void valueChanged();

 private:
  void updateRange();

  void pointToWidget();
  void widgetToPoint();

 private:
  CPoint2D          point_ { CPoint2D(0, 0) };
  bool              spin_ { false };
  CPoint2D          min_ { CPoint2D(-1E50, -1E50) };
  CPoint2D          max_ { CPoint2D(1E50, 1E50) };
  CPoint2D          step_ { CPoint2D(1, 1) };
  int               decimals_ { 3 };
  QDoubleValidator *x_validator_ { 0 };
  QDoubleValidator *y_validator_ { 0 };
  QLineEdit        *x_edit_ { 0 };
  QLineEdit        *y_edit_ { 0 };
  QDoubleSpinBox   *x_spin_ { 0 };
  QDoubleSpinBox   *y_spin_ { 0 };
  QHBoxLayout      *layout_ { 0 };
  mutable bool      disableSignals_ { false };
};

#endif
