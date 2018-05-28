#ifndef CQChartsLoadDlg_H
#define CQChartsLoadDlg_H

#include <QFrame>

class CQCharts;
class CQFilename;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QTextEdit;
class QPushButton;

class CQChartsLoadDlg : public QFrame {
  Q_OBJECT

 public:
  CQChartsLoadDlg(CQCharts *charts);
 ~CQChartsLoadDlg();

  bool isCommentHeader() const;
  bool isFirstLineHeader() const;
  bool isFirstColumnHeader() const;

  int numRows() const;

  QString filterStr() const;

 signals:
  void loadFile(const QString &type, const QString &filename);

 private slots:
  void previewFileSlot();
  void typeSlot();

  void okSlot();
  void applySlot();
  void cancelSlot();

 private:
  CQCharts*    charts_                 { nullptr };
  QComboBox*   typeCombo_              { nullptr };
  CQFilename*  fileEdit_               { nullptr };
  QCheckBox*   commentHeaderCheck_     { nullptr };
  QCheckBox*   firstLineHeaderCheck_   { nullptr };
  QCheckBox*   firstColumnHeaderCheck_ { nullptr };
  QLineEdit*   numberEdit_             { nullptr };
  QLineEdit*   filterEdit_             { nullptr };
  QTextEdit*   previewText_            { nullptr };
  QPushButton* okButton_               { nullptr };
  QPushButton* applyButton_            { nullptr };
};

#endif
