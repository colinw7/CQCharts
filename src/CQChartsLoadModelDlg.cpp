#include <CQChartsLoadModelDlg.h>
#include <CQChartsAnalyzeFile.h>
#include <CQChartsLoader.h>
#include <CQChartsInputData.h>
#include <CQChartsModelData.h>
#include <CQChartsWidgetUtil.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQTabSplit.h>
#include <CQFilename.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

namespace {

static bool isFixedPitch(const QFont & font) {
  const QFontInfo fi(font);
  //qDebug() << fi.family() << fi.fixedPitch();
  return fi.fixedPitch();
}

static QFont getMonospaceFont(){
  QFont font("monospace");
  if (isFixedPitch(font)) return font;
  font.setStyleHint(QFont::Monospace);
  if (isFixedPitch(font)) return font;
  font.setStyleHint(QFont::TypeWriter);
  if (isFixedPitch(font)) return font;
  font.setFamily("courier");
  if (isFixedPitch(font)) return font;
  return font;
}

}

CQChartsLoadModelDlg::
CQChartsLoadModelDlg(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("loadDlg");

  setWindowTitle("Load Model");
  //setWindowIcon(QIcon()); TODO

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  CQTabSplit *area = CQUtil::makeWidget<CQTabSplit>("area");

  area->setOrientation(Qt::Vertical);

  layout->addWidget(area);

  //----

  QFrame *fileFrame = CQUtil::makeWidget<QFrame>("file");

  QGridLayout *fileFrameLayout = CQUtil::makeLayout<QGridLayout>(fileFrame, 2, 2);

  area->addWidget(fileFrame, "File");

  //--

  int row = 0;

  // File Prompt
  fileEdit_ = CQUtil::makeWidget<CQFilename>("fileEdit");

  fileEdit_->setToolTip("File name to load");

  fileFrameLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("File", "fileLabel"), row, 0);
  fileFrameLayout->addWidget(fileEdit_                                           , row, 1);

  connect(fileEdit_, SIGNAL(filenameChanged(const QString &)), this, SLOT(previewFileSlot()));

  ++row;

  //--

  // Type Combo
  QStringList modelTypeNames;

  charts_->getModelTypeNames(modelTypeNames);

  typeCombo_ = CQUtil::makeWidget<QComboBox>("typeCombo");

  typeCombo_->addItems(modelTypeNames);
  typeCombo_->setToolTip("File type");

  fileFrameLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Type", "typeLabel"), row, 0);
  fileFrameLayout->addWidget(typeCombo_                                          , row, 1);

  connect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(typeSlot()));

  ++row;

  //--

  // Number Edit
  numberEdit_ = CQUtil::makeWidget<CQLineEdit>("numerEdit");

  numberEdit_->setText(QString("%1").arg(expressionLines()));
  numberEdit_->setToolTip("Number of rows to generate for expression");

  fileFrameLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Num Rows", "numRowsLabel"), row, 0);
  fileFrameLayout->addWidget(numberEdit_                                                , row, 1);

  ++row;

  //--

  // Option Checks
  QHBoxLayout *optionLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  commentHeaderCheck_ =
    CQUtil::makeLabelWidget<QCheckBox>("Comment Header", "commentHeaderCheck");

  commentHeaderCheck_->setToolTip("Use first comment for horizontal header");

  optionLayout->addWidget(commentHeaderCheck_);

  firstLineHeaderCheck_ =
    CQUtil::makeLabelWidget<QCheckBox>("First Line Header", "firstLineHeaderCheck");

  firstLineHeaderCheck_->setToolTip("Use first non-comment line for horizontal header");

  optionLayout->addWidget(firstLineHeaderCheck_);

  firstColumnHeaderCheck_ =
    CQUtil::makeLabelWidget<QCheckBox>("First Column Header", "firstColumnHeaderCheck");

  firstColumnHeaderCheck_->setToolTip("Use first column for vertical header");

  optionLayout->addWidget(firstColumnHeaderCheck_);

  optionLayout->addStretch(1);

  fileFrameLayout->addLayout(optionLayout, row, 0, 1, 2);

  ++row;

  //--

  // Filter Edit
  filterEdit_ = CQUtil::makeWidget<CQLineEdit>("filterEdit");

  filterEdit_->setToolTip("Filter expression for data");

  fileFrameLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Filter", "filterLabel"), row, 0);
  fileFrameLayout->addWidget(filterEdit_                                             , row, 1);

  ++row;

  //--

  fileFrameLayout->setRowStretch(row, 1);

  //----

  QFrame *previewFrame = CQUtil::makeWidget<QFrame>("preview");

  QVBoxLayout *previewFrameLayout = CQUtil::makeLayout<QVBoxLayout>(previewFrame, 2, 2);

  area->addWidget(previewFrame, "Preview");

  //--

  previewText_ = CQUtil::makeWidget<QTextEdit>("previewText");

  previewText_->setToolTip("File contents preview");

  previewFrameLayout->addWidget(previewText_);

  QFont fixedFont = getMonospaceFont();

  fixedFont.setPointSizeF(font().pointSizeF()*0.8);

  previewText_->setFont(fixedFont);

  //----

  // Bottom Buttons
  CQChartsDialogButtons *buttons = new CQChartsDialogButtons(this);

  buttons->connect(this, SLOT(okSlot()), SLOT(applySlot()), SLOT(cancelSlot()));

  buttons->setToolTips("Load model and close dialog",
                       "Load model and keep dialog open",
                       "Close dialog without loading model");

  okButton_    = buttons->okButton();
  applyButton_ = buttons->applyButton();

  layout->addWidget(buttons);

  //----

  typeSlot();
}

CQChartsLoadModelDlg::
~CQChartsLoadModelDlg()
{
}

bool
CQChartsLoadModelDlg::
isCommentHeader() const
{
  return commentHeaderCheck_->isChecked();
}

bool
CQChartsLoadModelDlg::
isFirstLineHeader() const
{
  return firstLineHeaderCheck_->isChecked();
}

bool
CQChartsLoadModelDlg::
isFirstColumnHeader() const
{
  return firstColumnHeaderCheck_->isChecked();
}

int
CQChartsLoadModelDlg::
numRows() const
{
  bool ok;

  int n = CQChartsUtil::toInt(numberEdit_->text(), ok);

  if (! ok)
    n = expressionLines();

  return n;
}

QString
CQChartsLoadModelDlg::
filterStr() const
{
  return filterEdit_->text();
}

void
CQChartsLoadModelDlg::
previewFileSlot()
{
  QString fileName = fileEdit_->name();

  QStringList lines;

  if (! CQChartsUtil::fileToLines(fileName, lines, previewLines()))
    return;

  QString text;

  for (int i = 0; i < lines.length(); ++i) {
    const QString &line = lines[i];

    if (text.length())
      text += "\n";

     text += line;
  }

  previewText_->setText(text);

  //---

  CQBaseModelDataType dataType = CQBaseModelDataType::NONE;

  bool commentHeader     = false;
  bool firstLineHeader   = false;
  bool firstColumnHeader = false;

  CQChartsAnalyzeFile analyzeFile(fileName);

  if (! analyzeFile.getDetails(dataType, commentHeader, firstLineHeader, firstColumnHeader))
    return;

  commentHeaderCheck_    ->setChecked(commentHeader);
  firstLineHeaderCheck_  ->setChecked(firstLineHeader);
  firstColumnHeaderCheck_->setChecked(firstColumnHeader);

  if      (dataType == CQBaseModelDataType::CSV)
    typeCombo_->setCurrentIndex(0);
  else if (dataType == CQBaseModelDataType::TSV)
    typeCombo_->setCurrentIndex(1);
  else if (dataType == CQBaseModelDataType::GNUPLOT)
    typeCombo_->setCurrentIndex(3);

  typeSlot();
}

void
CQChartsLoadModelDlg::
typeSlot()
{
  QString str = typeCombo_->currentText();

  numberEdit_->setEnabled(str == "Expr");
}

void
CQChartsLoadModelDlg::
okSlot()
{
  if (applySlot())
    hide();
}

bool
CQChartsLoadModelDlg::
applySlot()
{
  modelInd_ = -1;

  //----

  QString filename = fileEdit_->name();
  QString type     = typeCombo_->currentText();

  //----

  CQChartsFileType fileType = CQChartsFileTypeUtil::stringToFileType(type);

  if (fileType == CQChartsFileType::NONE)
    return false;

  //---

  CQChartsInputData inputData;

  inputData.commentHeader     = this->isCommentHeader();
  inputData.firstLineHeader   = this->isFirstLineHeader();
  inputData.firstColumnHeader = this->isFirstColumnHeader();
  inputData.numRows           = this->numRows();
  inputData.filter            = this->filterStr();

  if (! loadFileModel(filename, fileType, inputData))
    return false;

  return true;
}

bool
CQChartsLoadModelDlg::
loadFileModel(const QString &filename, CQChartsFileType type, const CQChartsInputData &inputData)
{
  bool hierarchical;

  QAbstractItemModel *model = loadFile(filename, type, inputData, hierarchical);

  if (! model)
    return false;

  ModelP modelp(model);

  CQChartsModelData *modelData = charts()->initModelData(modelp);

  charts()->setModelName(modelData, filename);

  modelInd_ = modelData->ind();

  emit modelLoaded(modelInd_);

  return true;
}

QAbstractItemModel *
CQChartsLoadModelDlg::
loadFile(const QString &filename, CQChartsFileType type, const CQChartsInputData &inputData,
         bool &hierarchical)
{
  CQChartsLoader loader(charts());

  //loader.setQtcl(qtcl());

  return loader.loadFile(filename, type, inputData, hierarchical);
}

void
CQChartsLoadModelDlg::
cancelSlot()
{
  modelInd_ = -1;

  hide();
}
