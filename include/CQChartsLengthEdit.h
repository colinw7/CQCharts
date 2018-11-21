#ifndef CQChartsLengthEdit_H
#define CQChartsLengthEdit_H

#include <CQChartsLength.h>
#include <QFrame>

class CQRealSpin;
class QComboBox;

class CQChartsLengthEdit : public QFrame {
 public:
  CQChartsLengthEdit(QWidget *parent=nullptr);

  void setLength(const CQChartsLength &pos);
  CQChartsLength length() const;

 private:
  CQRealSpin *realEdit_   { nullptr };
  QComboBox  *unitsCombo_ { nullptr };
};

#endif
