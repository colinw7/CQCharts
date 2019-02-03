#ifndef CQChartsLineEditBase_H
#define CQChartsLineEditBase_H

#include <QFrame>
#include <QStyleOptionComboBox>

class QHBoxLayout;
class QLineEdit;
class QPushButton;
class CQWidgetMenu;

class CQChartsLineEditBase : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString text            READ text            WRITE setText           )
  Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)

 public:
  CQChartsLineEditBase(QWidget *parent = nullptr);

  QString text() const;
  void setText(const QString &s);

  QString placeholderText() const;
  void setPlaceholderText(const QString &s);

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

 protected slots:
  void showMenuSlot();

  void updateMenuSlot();

  void textChangedSlot();

 protected:
  virtual void textChanged() = 0;

  virtual void updateMenu();

  virtual void connectSlots(bool b);

  void initStyle(QStyleOptionComboBox &opt);

 protected:
  QHBoxLayout*  layout_ { nullptr };
  QLineEdit*    edit_   { nullptr };
  QPushButton*  button_ { nullptr };
  CQWidgetMenu* menu_   { nullptr };
};

//---

#include <QPushButton>

class CQChartsLineEditMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQChartsLineEditMenuButton(QWidget *parent=nullptr);

  void paintEvent(QPaintEvent *) override;
};

#endif
