#ifndef CQChartsColumnControlGroup_H
#define CQChartsColumnControlGroup_H

#include <QFrame>

class CQIconButton;
class CQGroupBox;
//class QCheckBox;
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

  void addFixedWidget(QWidget *w);

  void addColumnWidget (QWidget *w);
  void addColumnStretch();

  bool hasKey() const { return hasKey_; }
  void setHasKey(bool b) { hasKey_ = b; }

  bool isFixed() const;
  void setFixed();

  bool isColumn() const;
  void setColumn();

  bool isKeyVisible() const;

 private Q_SLOTS:
  void keyCheckSlot();

//void controlButtonClicked(QAbstractButton *button);
  void columnCheckSlot();

 private:
  void setColumnStack(bool b);

  void setCurrentIndex(int ind);

 Q_SIGNALS:
  void groupChanged();

  void showKey(bool b);

 private:
  CQGroupBox*     groupBox_     { nullptr };
  bool            hasKey_       { true };
  CQIconButton*   keyCheck_     { nullptr };
  CQIconButton*   columnCheck_  { nullptr };
  QStackedWidget* stack_        { nullptr };
//QButtonGroup*   radioGroup_   { nullptr };
  QFrame*         globalWidget_ { nullptr };
  QFrame*         columnWidget_ { nullptr };
};

#endif
