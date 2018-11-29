#include <CQChartsLoadDlg.h>
#include <CQChartsAnalyzeFile.h>
#include <CQChartsLoader.h>
#include <CQChartsInputData.h>
#include <CQChartsModelData.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQDividedArea.h>
#include <CQFilename.h>

#include <QComboBox>
#include <QLineEdit>
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

CQChartsLoadDlg::
CQChartsLoadDlg(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("loadDlg");

  setWindowTitle("Load Model");
  //setWindowIcon(QIcon()); TODO

  QVBoxLayout *layout = new QVBoxLayout(this);

  //---

  CQDividedArea *area = new CQDividedArea;
  area->setObjectName("area");

  layout->addWidget(area);

  //----

  QFrame *fileFrame = new QFrame;
  fileFrame->setObjectName("file");

  QGridLayout *fileFrameLayout = new QGridLayout(fileFrame);

  area->addWidget(fileFrame, "File");

  //--

  int row = 0;

  // File Prompt
  fileEdit_ = new CQFilename;
  fileEdit_->setObjectName("fileEdit");

  fileFrameLayout->addWidget(new QLabel("File"), row, 0);
  fileFrameLayout->addWidget(fileEdit_         , row, 1);

  connect(fileEdit_, SIGNAL(filenameChanged(const QString &)), this, SLOT(previewFileSlot()));

  ++row;

  //--

  // Type Combo
  typeCombo_ = new QComboBox;
  typeCombo_->setObjectName("typeCombo");

  typeCombo_->addItem("CSV");
  typeCombo_->addItem("TSV");
  typeCombo_->addItem("Json");
  typeCombo_->addItem("Data");
  typeCombo_->addItem("Expr");

  fileFrameLayout->addWidget(new QLabel("Type"), row, 0);
  fileFrameLayout->addWidget(typeCombo_        , row, 1);

  connect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(typeSlot()));

  ++row;

  //--

  // Number Edit
  numberEdit_ = new QLineEdit;
  numberEdit_->setObjectName("numberEdit");

  numberEdit_->setText("100");

  fileFrameLayout->addWidget(new QLabel("Num Rows"), row, 0);
  fileFrameLayout->addWidget(numberEdit_           , row, 1);

  ++row;

  //--

  // Option Checks
  QHBoxLayout *optionLayout = new QHBoxLayout;

  commentHeaderCheck_ = new QCheckBox("Comment Header");
  commentHeaderCheck_->setObjectName("commentHeaderCheck");

  optionLayout->addWidget(commentHeaderCheck_);

  firstLineHeaderCheck_ = new QCheckBox("First Line Header");
  firstLineHeaderCheck_->setObjectName("firstLineHeaderCheck");

  optionLayout->addWidget(firstLineHeaderCheck_);

  firstColumnHeaderCheck_ = new QCheckBox("First Column Header");
  firstColumnHeaderCheck_->setObjectName("firstColumnHeaderCheck");

  optionLayout->addWidget(firstColumnHeaderCheck_);

  optionLayout->addStretch(1);

  fileFrameLayout->addLayout(optionLayout, row, 0, 1, 2);

  ++row;

  //--

  // Filter Edit
  filterEdit_ = new QLineEdit;
  filterEdit_->setObjectName("filterEdit");

  fileFrameLayout->addWidget(new QLabel("Filter"), row, 0);
  fileFrameLayout->addWidget(filterEdit_         , row, 1);

  ++row;

  //--

  fileFrameLayout->setRowStretch(row, 1);

  //----

  QFrame *previewFrame = new QFrame;
  previewFrame->setObjectName("preview");

  QVBoxLayout *previewFrameLayout = new QVBoxLayout(previewFrame);

  area->addWidget(previewFrame, "Preview");

  //--

  previewText_ = new QTextEdit;
  previewText_->setObjectName("previewText");

  previewFrameLayout->addWidget(previewText_);

  QFont fixedFont = getMonospaceFont();

  fixedFont.setPointSizeF(font().pointSizeF()*0.8);

  previewText_->setFont(fixedFont);

  //----

  // Bottom Buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout;

  okButton_ = new QPushButton("OK");
  okButton_->setObjectName("ok");

  applyButton_ = new QPushButton("Apply");
  applyButton_->setObjectName("apply");

  QPushButton *cancelButton = new QPushButton("Cancel");
  cancelButton->setObjectName("cancel");

  connect(okButton_   , SIGNAL(clicked()), this, SLOT(okSlot()));
  connect(applyButton_, SIGNAL(clicked()), this, SLOT(applySlot()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));

  buttonLayout->addStretch(1);

  buttonLayout->addWidget(okButton_);
  buttonLayout->addWidget(applyButton_);
  buttonLayout->addWidget(cancelButton);

  layout->addLayout(buttonLayout);
}

CQChartsLoadDlg::
~CQChartsLoadDlg()
{
}

bool
CQChartsLoadDlg::
isCommentHeader() const
{
  return commentHeaderCheck_->isChecked();
}

bool
CQChartsLoadDlg::
isFirstLineHeader() const
{
  return firstLineHeaderCheck_->isChecked();
}

bool
CQChartsLoadDlg::
isFirstColumnHeader() const
{
  return firstColumnHeaderCheck_->isChecked();
}

int
CQChartsLoadDlg::
numRows() const
{
  bool ok;

  int n = numberEdit_->text().toInt(&ok);

  if (! ok)
    n = 100;

  return n;
}

QString
CQChartsLoadDlg::
filterStr() const
{
  return filterEdit_->text();
}

void
CQChartsLoadDlg::
previewFileSlot()
{
  QString fileName = fileEdit_->name();

  std::size_t maxLines = 10;

  QStringList lines;

  if (! CQChartsUtil::fileToLines(fileName, lines, maxLines))
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
CQChartsLoadDlg::
typeSlot()
{
  QString str = typeCombo_->currentText();

  numberEdit_->setEnabled(str == "Expr");
}

void
CQChartsLoadDlg::
okSlot()
{
  if (applySlot())
    hide();
}

bool
CQChartsLoadDlg::
applySlot()
{
  modelInd_ = -1;

  //----

  QString filename = fileEdit_->name();
  QString type     = typeCombo_->currentText();

  //----

  CQChartsFileType fileType = stringToFileType(type);

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
CQChartsLoadDlg::
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
CQChartsLoadDlg::
loadFile(const QString &filename, CQChartsFileType type, const CQChartsInputData &inputData,
         bool &hierarchical)
{
  CQChartsLoader loader(charts());

  //loader.setQtcl(qtcl());

  return loader.loadFile(filename, type, inputData, hierarchical);
}

void
CQChartsLoadDlg::
cancelSlot()
{
  modelInd_ = -1;

  hide();
}
