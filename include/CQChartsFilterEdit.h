#ifndef CQChartsFilterEdit_H
#define CQChartsFilterEdit_H

#include <QFrame>

class CQChartsFilterEditEdit;
class CQIconCombo;
class CQSwitch;
class QCheckBox;

/*!
 * \brief filter edit
 * \ingroup Charts
 */
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

  void escapeSlot();

 signals:
  void filterAnd(bool);

  void replaceFilter(const QString &text);
  void addFilter(const QString &text);

  void replaceSearch(const QString &text);
  void addSearch(const QString &text);

  void escapePressed();

 private:
  CQChartsFilterEditEdit* edit_             { nullptr };
  CQIconCombo*            combo_            { nullptr };
  CQSwitch*               addReplaceSwitch_ { nullptr };
  CQSwitch*               andOrSwitch_      { nullptr };
  QString                 filterText_;
  QString                 searchText_;
  QString                 filterDetails_;
  QString                 searchDetails_;
};

//------

#include <CQLineEdit.h>

/*!
 * \brief filter line edit
 * \ingroup Charts
 */
class CQChartsFilterEditEdit : public CQLineEdit {
  Q_OBJECT

 public:
  CQChartsFilterEditEdit(QWidget *parent=nullptr);

  void keyPressEvent(QKeyEvent *e) override;

 signals:
   void escapePressed();
};

#endif
