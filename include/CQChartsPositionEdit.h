#ifndef CQChartsPositionEdit_H
#define CQChartsPositionEdit_H

#include <CQChartsPosition.h>
#include <QFrame>

class CQPoint2DEdit;
class QComboBox;

class CQChartsPositionEdit : public QFrame {
 public:
  CQChartsPositionEdit(QWidget *parent=nullptr);

  void setPosition(const CQChartsPosition &pos);
  CQChartsPosition position() const;

 private:
  CQPoint2DEdit *pointEdit_  { nullptr };
  QComboBox     *unitsCombo_ { nullptr };
};

#endif
