#ifndef CQCheckBox_H
#define CQCheckBox_H

#include <QCheckBox>

class CQCheckBox : public QCheckBox {
  Q_OBJECT

 public:
  CQCheckBox(QWidget *parent=nullptr);
  CQCheckBox(const QString &text, QWidget *parent=nullptr);

 private slots:
  void stateSlot(int b);
};

#endif
