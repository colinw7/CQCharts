#ifndef CQChartsLoadModelDlg_H
#define CQChartsLoadModelDlg_H

#include <CQChartsFileType.h>
#include <CQChartsLineEdit.h>

#include <QDialog>
#include <QSharedPointer>

class CQCharts;
class CQFilename;
class CQTableWidget;

struct CQChartsInputData;

class QAbstractItemModel;
class QComboBox;
class QRadioButton;
class QCheckBox;
class QTextBrowser;
class QPushButton;

/*!
 * \brief dialog to load new charts model
 * \ingroup Charts
 */
class CQChartsLoadModelDlg : public QDialog {
  Q_OBJECT

  Q_PROPERTY(int previewLines   READ previewLines   WRITE setPreviewLines  )
  Q_PROPERTY(int expressionRows READ expressionRows WRITE setExpressionRows)

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsLoadModelDlg(CQCharts *charts);
 ~CQChartsLoadModelDlg();

  CQCharts *charts() const { return charts_; }

  //---

  //! get/set preview number of lines
  int previewLines() const { return previewLines_; }
  void setPreviewLines(int i) { previewLines_ = i; }

  //! get/set number of rows to generate for expression type
  int expressionRows() const { return expressionRows_; }
  void setExpressionRows(int i) { expressionRows_ = i; }

  //---

  bool isCommentHeader() const;
  bool isFirstLineHeader() const;
  bool isFirstColumnHeader() const;

  //---

  int numRows() const;

  QString filterStr() const;

  int     modelInd() const { return modelInd_; }
  QString modelId() const { return modelId_; }

  QSize sizeHint() const override;

 signals:
  void modelLoaded(int);

 private slots:
  void previewFileSlot();
  void typeSlot();
  void updatePreviewSlot();
  void updateColumns();

  void typeChangedSlot(int ind);

  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  void parseCSVColumns();
  void parseTSVColumns();

  void setMetaText();

  bool loadFileModel(const QString &filename, CQChartsFileType type,
                     const CQChartsInputData &inputData);

  QAbstractItemModel *loadFile(const QString &filename, CQChartsFileType type,
                               const CQChartsInputData &inputData, bool &hierarchical);

 private:
  using ColumnTypes = std::map<QString, QString>;
  using NameValue   = std::map<QString, QString>;
  using ColumnData  = std::map<QString, NameValue>;

  enum class LineType {
    DATA,
    COMMENT_HEADER,
    DATA_HEADER,
    META,
    COMMENT
  };

  struct Line {
    LineType type { LineType::DATA };
    QString  text;

    Line() = default;

    Line(const LineType &type, const QString &text) :
     type(type), text(text) {
    }
  };

  using Lines = std::vector<Line>;

  CQCharts*         charts_                 { nullptr };
  bool              emitLoadSignal_         { false };
  int               modelInd_               { -1 };
  QString           modelId_;
  QComboBox*        typeCombo_              { nullptr };
  CQFilename*       fileEdit_               { nullptr };
  QRadioButton*     noHeaderCheck_          { nullptr };
  QRadioButton*     commentHeaderCheck_     { nullptr };
  QRadioButton*     firstLineHeaderCheck_   { nullptr };
  QCheckBox*        firstColumnHeaderCheck_ { nullptr };
  CQChartsLineEdit* numberEdit_             { nullptr };
  CQChartsLineEdit* filterEdit_             { nullptr };
  QTextBrowser*     previewTextEdit_        { nullptr };
  QTextBrowser*     metaTextEdit_           { nullptr };
  CQTableWidget*    columnsTable_           { nullptr };
  QPushButton*      okButton_               { nullptr };
  QPushButton*      applyButton_            { nullptr };
  int               previewLines_           { 100 };
  int               expressionRows_         { 100 };
  Lines             lines_;
  QStringList       metaLines_;
  QString           firstLine_;
  QString           firstComment_;
  QStringList       columns_;
  ColumnTypes       columnTypes_;
  ColumnData        columnData_;
};

#endif
