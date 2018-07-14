#ifndef CQChartsLoadDlg_H
#define CQChartsLoadDlg_H

#include <CQChartsFileType.h>
#include <QDialog>
#include <QSharedPointer>

class CQCharts;
class CQChartsInputData;
class CQFilename;

class QAbstractItemModel;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QTextEdit;
class QPushButton;

class CQChartsLoadDlg : public QDialog {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsLoadDlg(CQCharts *charts);
 ~CQChartsLoadDlg();

  CQCharts *charts() const { return charts_; }

  bool isCommentHeader() const;
  bool isFirstLineHeader() const;
  bool isFirstColumnHeader() const;

  int numRows() const;

  QString filterStr() const;

  int modelInd() const { return modelInd_; }

 signals:
  void modelLoaded(int);

 private slots:
  void previewFileSlot();
  void typeSlot();

  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  bool loadFileModel(const QString &filename, CQChartsFileType type,
                     const CQChartsInputData &inputData);

  QAbstractItemModel *loadFile(const QString &filename, CQChartsFileType type,
                               const CQChartsInputData &inputData, bool &hierarchical);

 private:
  CQCharts*    charts_                 { nullptr };
  bool         emitLoadSignal_         { false };
  int          modelInd_               { -1 };
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
