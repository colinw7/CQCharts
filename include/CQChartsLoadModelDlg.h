#ifndef CQChartsLoadModelDlg_H
#define CQChartsLoadModelDlg_H

#include <CQChartsFileType.h>
#include <CQLineEdit.h>
#include <QDialog>
#include <QSharedPointer>

class CQCharts;
class CQFilename;

struct CQChartsInputData;

class QAbstractItemModel;
class QComboBox;
class QCheckBox;
class QTextEdit;
class QPushButton;

//! \brief dialog to load new charts model
class CQChartsLoadModelDlg : public QDialog {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsLoadModelDlg(CQCharts *charts);
 ~CQChartsLoadModelDlg();

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
  CQLineEdit*  numberEdit_             { nullptr };
  CQLineEdit*  filterEdit_             { nullptr };
  QTextEdit*   previewText_            { nullptr };
  QPushButton* okButton_               { nullptr };
  QPushButton* applyButton_            { nullptr };
};

#endif
