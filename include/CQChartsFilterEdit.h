#ifndef CQChartsFilterEdit_H
#define CQChartsFilterEdit_H

#include <QFrame>

class CQIconCombo;
class CQSwitch;
class CQLineEdit;
class QCheckBox;

class CQChartsFilterEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsFilterEdit(QWidget *parent=nullptr);

  void setFilterDetails(const QString &str);
  void setSearchDetails(const QString &str);

 private slots:
  void comboSlot(int);

  void andSlot();

  void acceptSlot();

 signals:
  void filterAnd(bool);

  void replaceFilter(const QString &text);
  void addFilter(const QString &text);

  void replaceSearch(const QString &text);
  void addSearch(const QString &text);

 private:
  CQLineEdit*  edit_             { nullptr };
  CQIconCombo* combo_            { nullptr };
  CQSwitch*    addReplaceSwitch_ { nullptr };
  CQSwitch*    andOrSwitch_      { nullptr };
  QString      filterText_;
  QString      searchText_;
  QString      filterDetails_;
  QString      searchDetails_;
};

#endif
