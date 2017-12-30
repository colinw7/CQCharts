#ifndef CQChartsFilterEdit_H
#define CQChartsFilterEdit_H

#include <QFrame>

class CQIconCombo;
class CQHistoryLineEdit;
class QRadioButton;

class CQChartsFilterEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsFilterEdit(QWidget *parent=nullptr);

 private slots:
  void comboSlot(int);

  void acceptSlot(const QString &text);

 signals:
  void replaceFilter(const QString &text);
  void addFilter(const QString &text);

  void replaceSearch(const QString &text);
  void addSearch(const QString &text);

 private:
  CQHistoryLineEdit* edit_          { nullptr };
  CQIconCombo*       combo_         { nullptr };
  QRadioButton*      replaceButton_ { nullptr };
  QRadioButton*      addButton_     { nullptr };
};

#endif
