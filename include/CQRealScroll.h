#ifndef CQRealScroll_H
#define CQRealScroll_H

#include <QScrollBar>

class CQRealScroll : public QScrollBar {
  Q_OBJECT

  Q_PROPERTY(double value READ value WRITE setValue)

 public:
  CQRealScroll(QWidget *parent=nullptr);
  CQRealScroll(Qt::Orientation orientation, QWidget *parent=nullptr);

  void setRange(double min, double max);

  double minimum() const { return min_; }
  double maximum() const { return max_; }

  void setPageStep(double d);
  double pageStep() const { return pageStep_; }

  void setSingleStep(double d);
  double singleStep() const { return singleStep_; }

  double value() const;
  void setValue(double r);

 Q_SIGNALS:
  void valueChanged(double);

 private Q_SLOTS:
  void scrollSlot(int);

 private:
  void init();
  void updateRange(double);
  void updateTip();

 private:
  double precision_ { 1000 };

  double min_        { 0.0 };
  double max_        { 1.0 };
  double pageStep_   { 0.1 };
  double singleStep_ { 0.01 };
};

#endif
