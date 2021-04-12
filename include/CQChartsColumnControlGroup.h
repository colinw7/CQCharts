#ifndef CQChartsColumnControlGroup_H
#define CQChartsColumnControlGroup_H

#include <QFrame>

class CQGroupBox;
class QCheckBox;
class QAbstractButton;
class QStackedWidget;
class QButtonGroup;

class CQChartsColumnControlGroup : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString title READ title WRITE setTitle)

 public:
  CQChartsColumnControlGroup(QWidget *parent=nullptr);

  const QString &title() const;
  void setTitle(const QString &s);

  void addFixedWidget (QWidget *w);
  void addColumnWidget(QWidget *w);

  bool isFixed () const;
  void setFixed();

  bool isColumn () const;
  void setColumn();

  bool isKeyVisible() const;

 private slots:
  void keyCheckSlot(int);

  void controlButtonClicked(QAbstractButton *button);

 private:
  void setCurrentIndex(int ind);

 signals:
  void groupChanged();

  void showKey(bool b);

 private:
  CQGroupBox*     groupBox_     { nullptr };
  QCheckBox*      keyCheck_     { nullptr };
  QStackedWidget* stack_        { nullptr };
  QButtonGroup*   radioGroup_   { nullptr };
  QFrame*         globalWidget_ { nullptr };
  QFrame*         columnWidget_ { nullptr };
};

#endif
