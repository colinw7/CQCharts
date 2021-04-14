#ifndef CQChartsIconButton_H
#define CQChartsIconButton_H

#include <QToolButton>

class CQChartsIconButton : public QToolButton {
  Q_OBJECT

 public:
  CQChartsIconButton(QWidget *parent=nullptr);

  void setIcon(const QString &iconName);
};

#endif
