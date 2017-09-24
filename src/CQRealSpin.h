#ifndef CQRealSpin_H
#define CQRealSpin_H

#include <QDoubleSpinBox>

class CQRealSpin : public QDoubleSpinBox {
  Q_OBJECT

  Q_PROPERTY(bool   autoStep READ isAutoStep WRITE setAutoStep)
  Q_PROPERTY(double step     READ step)

 public:
  CQRealSpin(QWidget *parent, double value=0);
  CQRealSpin(double value=0);

  virtual ~CQRealSpin() { }

  bool isAutoStep() const { return autoStep_; }
  void setAutoStep(bool b) { autoStep_ = b; }

  double step() const { return step_; }

  void stepBy(int n);

 signals:
  void stepChanged(double);

 private slots:
  void updateStep();

 private:
  void init(double value);

  double posToStep(int pos) const;

  bool isNegative() const;

  int dotPos() const;

 private:
  bool   autoStep_ { true };
  double step_     { 1 };
};

#endif
