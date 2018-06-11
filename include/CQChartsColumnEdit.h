#ifndef CQChartsColumnEdit_H
#define CQChartsColumnEdit_H

#include <QFrame>
#include <QStyleOptionComboBox>

class CQWidgetMenu;
class QLineEdit;
class QPushButton;
class QGroupBox;
class QComboBox;
class QCheckBox;
class QAbstractItemModel;

class CQChartsColumnEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString text            READ text            WRITE setText           )
  Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)

 public:
  CQChartsColumnEdit(QWidget *parent=nullptr);

  void setModel(QAbstractItemModel *model);

  QString text() const;
  void setText(const QString &s);

  QString placeholderText() const;
  void setPlaceholderText(const QString &s);

  void paintEvent(QPaintEvent *);

  void resizeEvent(QResizeEvent *);

 signals:
  void textChanged(const QString &);

 private slots:
  void showMenu();

  void updateMenu();

  void menuColumnGroupClicked(bool b);
  void menuExprGroupClicked  (bool b);
  void vheaderCheckClicked   (bool b);

  void menuColumnChanged(int i);

  void expressionTextChanged(const QString &str);

  void updateState();

 private:
  void initStyle(QStyleOptionComboBox &opt);

 private:
  QLineEdit*          edit_            { nullptr };
  QPushButton*        button_          { nullptr };
  CQWidgetMenu*       menu_            { nullptr };
  QGroupBox*          menuColumnGroup_ { nullptr };
  QComboBox*          columnCombo_     { nullptr };
  QGroupBox*          menuExprGroup_   { nullptr };
  QLineEdit*          expressionEdit_  { nullptr };
  QCheckBox*          vheaderCheck_    { nullptr };
  QAbstractItemModel* model_           { nullptr };
};

//---

#include <QPushButton>

class CQChartsColumnEditMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQChartsColumnEditMenuButton(QWidget *parent=nullptr);

  void paintEvent(QPaintEvent *);
};

#endif
