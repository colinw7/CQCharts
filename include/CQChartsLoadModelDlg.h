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

/*!
 * \brief dialog to load new charts model
 * \ingroup Charts
 */
class CQChartsLoadModelDlg : public QDialog {
  Q_OBJECT

  Q_PROPERTY(int previewLines    READ previewLines    WRITE setPreviewLines   )
  Q_PROPERTY(int expressionLines READ expressionLines WRITE setExpressionLines)

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsLoadModelDlg(CQCharts *charts);
 ~CQChartsLoadModelDlg();

  CQCharts *charts() const { return charts_; }

  int previewLines() const { return previewLines_; }
  void setPreviewLines(int i) { previewLines_ = i; }

  int expressionLines() const { return expressionLines_; }
  void setExpressionLines(int i) { expressionLines_ = i; }

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
  int          previewLines_           { 100 };
  int          expressionLines_        { 100 };
};

#endif
