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
  void setPageStep(double d);
  void setSingleStep(double d);

  double value() const;
  void setValue(double r);

 Q_SIGNALS:
  void valueChanged(double);

 private Q_SLOTS:
  void scrollSlot(int);

 private:
  void init();
  void updateRange(double);

 private:
  double precision_ { 1000 };

  double min_        { 0.0 };
  double max_        { 1.0 };
  double pageStep_   { 0.1 };
  double singleStep_ { 0.01 };
};

#endif
