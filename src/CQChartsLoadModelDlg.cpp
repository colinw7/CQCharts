#include <CQChartsLoadModelDlg.h>
#include <CQChartsAnalyzeFile.h>
#include <CQChartsLoader.h>
#include <CQChartsInputData.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnType.h>
#include <CQChartsModelUtil.h>
#include <CQChartsWidgetUtil.h>
#include <CQCharts.h>
#include <CQChartsFile.h>
#include <CQChartsUtil.h>

#include <CQTabSplit.h>
#include <CQFilename.h>
#include <CQStrParse.h>
#include <CQUtil.h>
#include <CQTclUtil.h>
#include <CQTableWidget.h>
#include <CCsv.h>
#include <CTsv.h>

#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QTextBrowser>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

CQChartsLoadModelDlg::
CQChartsLoadModelDlg(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("loadDlg");

  setWindowTitle("Load Model");
//setWindowIcon(QIcon()); TODO

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  auto *area = CQUtil::makeWidget<CQTabSplit>("area");

  area->setOrientation(Qt::Vertical);
  area->setGrouped(true);

  layout->addWidget(area);

  //----

  auto *fileFrame       = CQUtil::makeWidget<QFrame>("file");
  auto *fileFrameLayout = CQUtil::makeLayout<QGridLayout>(fileFrame, 2, 2);

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

  // Expression Row Number Edit
  numberEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("numberEdit");

  numberEdit_->setText(QString::number(expressionRows()));
  numberEdit_->setToolTip("Number of rows to generate for expression");

  fileFrameLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Num Rows", "numRowsLabel"), row, 0);
  fileFrameLayout->addWidget(numberEdit_                                                , row, 1);

  ++row;

  //--

  // Horizontal Header Checks
  fileFrameLayout->addWidget(
    CQUtil::makeLabelWidget<QLabel>("Horizontal Header", "hheaderLabel"), row, 0);

  auto *hheaderFrame  = CQUtil::makeWidget<QFrame>("hheaderFrame");
  auto *hheaderLayout = CQUtil::makeLayout<QHBoxLayout>(hheaderFrame, 0, 2);

  noHeaderCheck_ = CQUtil::makeLabelWidget<QRadioButton>("None", "noneHeaderCheck");

  noHeaderCheck_->setToolTip("No horizontal header");

  connect(noHeaderCheck_, SIGNAL(toggled(bool)), this, SLOT(updatePreviewSlot()));

  commentHeaderCheck_ = CQUtil::makeLabelWidget<QRadioButton>("Comment", "commentHeaderCheck");

  commentHeaderCheck_->setToolTip("Use first comment for horizontal header");

  connect(commentHeaderCheck_, SIGNAL(toggled(bool)), this, SLOT(updatePreviewSlot()));

  firstLineHeaderCheck_ =
    CQUtil::makeLabelWidget<QRadioButton>("First Line", "firstLineHeaderCheck");

  firstLineHeaderCheck_->setToolTip("Use first non-comment line for horizontal header");

  connect(firstLineHeaderCheck_, SIGNAL(toggled(bool)), this, SLOT(updatePreviewSlot()));

  hheaderLayout->addWidget (noHeaderCheck_);
  hheaderLayout->addWidget (commentHeaderCheck_);
  hheaderLayout->addWidget (firstLineHeaderCheck_);
  hheaderLayout->addStretch(1);

  fileFrameLayout->addWidget(hheaderFrame, row, 1);

  ++row;

  //---

  // Vertical Header Checks
  fileFrameLayout->addWidget(
    CQUtil::makeLabelWidget<QLabel>("Vertical Header", "vheaderLabel"), row, 0);

  firstColumnHeaderCheck_ =
    CQUtil::makeLabelWidget<QCheckBox>("First Column", "firstColumnHeaderCheck");

//connect(firstColumnHeaderCheck_, SIGNAL(stateChanged(int)), this, SLOT(updatePreviewSlot()));

  firstColumnHeaderCheck_->setToolTip("Use first column for vertical header");

  fileFrameLayout->addWidget(firstColumnHeaderCheck_, row, 1);

  ++row;

  //--

  // Filter Edit
  filterEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("filterEdit");

  filterEdit_->setToolTip("Filter expression for data");

  fileFrameLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Filter", "filterLabel"), row, 0);
  fileFrameLayout->addWidget(filterEdit_                                             , row, 1);

  ++row;

  //--

  fileFrameLayout->setRowStretch(row, 1);

  //----

  auto *dataArea = CQUtil::makeWidget<CQTabSplit>("dataArea");

  dataArea->setState(CQTabSplit::State::TAB);

  area->addWidget(dataArea, "Data");

  //----

  auto fixedFont = CQUtil::getMonospaceFont();

  //--

  auto *previewFrame       = CQUtil::makeWidget<QFrame>("preview");
  auto *previewFrameLayout = CQUtil::makeLayout<QVBoxLayout>(previewFrame, 2, 2);

  dataArea->addWidget(previewFrame, "Preview");

  //--

  previewTextEdit_ = CQUtil::makeWidget<QTextBrowser>("previewText");

  previewTextEdit_->setReadOnly(true);
  previewTextEdit_->setToolTip("File contents preview");

  previewFrameLayout->addWidget(previewTextEdit_);

  previewTextEdit_->setFont(CQChartsUtil::scaleFontSize(fixedFont, 0.9));

  //--

  auto *columnsFrame       = CQUtil::makeWidget<QFrame>("columnsFrame");
  auto *columnsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(columnsFrame, 2, 2);

  columnsTable_ = CQUtil::makeWidget<CQTableWidget>("columnsTable");

  columnsFrameLayout->addWidget(columnsTable_);

  dataArea->addWidget(columnsFrame, "Columns");

  //--

  auto *metaFrame       = CQUtil::makeWidget<QFrame>("metaFrame");
  auto *metaFrameLayout = CQUtil::makeLayout<QVBoxLayout>(metaFrame, 2, 2);

  dataArea->addWidget(metaFrame, "Meta");

  //--

  metaTextEdit_ = CQUtil::makeWidget<QTextBrowser>("metaText");

  metaTextEdit_->setReadOnly(true);
  metaTextEdit_->setToolTip("File contents meta");

  metaFrameLayout->addWidget(metaTextEdit_);

  metaTextEdit_->setFont(CQChartsUtil::scaleFontSize(fixedFont, 0.9));

  //----

  // Bottom Buttons
  auto *buttons = new CQChartsDialogButtons(this);

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

  int n = (int) CQChartsUtil::toInt(numberEdit_->text(), ok);

  if (! ok)
    n = expressionRows();

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
  auto filename = fileEdit_->name();

  int maxLines = 1024;

  QStringList lines;

  if (! CQChartsUtil::fileToLines(filename, lines, maxLines))
    return;

  //---

  int  lineNum        = 0;
  bool inMeta         = false;
  bool isFirstLine    = true;
  bool isFirstComment = true;

  lines_    .clear();
  metaLines_.clear();

  auto addLine = [&](const LineType &type, const QString &text) {
    lines_.emplace_back(type, text);

    if (type == LineType::DATA)
      ++lineNum;

    return (lineNum < previewLines());
  };

  bool allowMeta = (typeCombo_->currentIndex() == 0); // CSV

  for (int i = 0; i < lines.length(); ++i) {
    const auto &line = lines[i];

    CQStrParse parse(line);

    parse.skipSpace();

    // handle comments
    if (parse.isChar('#')) {
      if (! inMeta) {
        if (allowMeta && parse.isString("#META_DATA")) {
          inMeta = true;

          if (! addLine(LineType::META, line))
            break;
        }
        else {
          if (isFirstComment) {
            firstComment_  = line;
            isFirstComment = false;

            if (! addLine(LineType::COMMENT_HEADER, line))
              break;
          }
          else {
            if (! addLine(LineType::COMMENT, line))
              break;
          }
        }
      }
      else {
        assert(allowMeta);

        if (parse.isString("#END_META_DATA")) {
          inMeta = false;

          if (! addLine(LineType::META, line))
            break;
        }
        else {
          metaLines_.push_back(line);

          if (! addLine(LineType::META, line))
            break;
        }
      }
    }
    else {
      if (isFirstLine) {
        firstLine_  = line;
        isFirstLine = false;

        if (! addLine(LineType::DATA_HEADER, line))
          break;
      }
      else {
        if (! addLine(LineType::DATA, line))
          break;
      }
    }
  }

  updatePreviewSlot();

  //---

  auto dataType = CQBaseModelDataType::NONE;

  bool commentHeader     = false;
  bool firstLineHeader   = false;
  bool firstColumnHeader = false;

  CQChartsAnalyzeFile analyzeFile(filename);

  analyzeFile.setMaxLines(maxLines);

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
updatePreviewSlot()
{
  QString text;

  for (const auto &l : lines_) {
    if (text != "")
      text += "<br>\n";

    if      (l.type == LineType::DATA)
      text += l.text;
    else if (l.type == LineType::COMMENT_HEADER) {
      if (commentHeaderCheck_->isChecked())
        text += QString("<font color=\"green\">") + l.text + "</font>";
      else
        text += l.text;
    }
    else if (l.type == LineType::DATA_HEADER) {
      if (firstLineHeaderCheck_->isChecked())
        text += QString("<font color=\"green\">") + l.text + "</font>";
      else
        text += l.text;
    }
    else if (l.type == LineType::META) {
      //text += QString("<font color=\"magenta\">") + l.text + "</font>";
    }
    else if (l.type == LineType::COMMENT) {
      text += QString("<font color=\"blue\">") + l.text + "</font>";
    }
    else {
      assert(false);
    }
  }

  previewTextEdit_->setHtml(text);

  //---

  setMetaText();

  //---

  updateColumns();
}

void
CQChartsLoadModelDlg::
setMetaText()
{
  QString metaText;

  for (const auto &l : metaLines_) {
    if (metaText != "")
      metaText += "\n";

    metaText += l;
  }

  metaTextEdit_->setText(metaText);
}

void
CQChartsLoadModelDlg::
typeSlot()
{
  auto str = typeCombo_->currentText();

  numberEdit_->setEnabled(str == "Expr");

  //---

  updateColumns();

  if      (typeCombo_->currentIndex() == 0)
    fileEdit_->setPattern("CSV Files (*.csv)");
  else if (typeCombo_->currentIndex() == 1)
    fileEdit_->setPattern("TSV Files (*.tsv)");
  else if (typeCombo_->currentIndex() == 2)
    fileEdit_->setPattern("Gnuplot Files (*.data)");
}

void
CQChartsLoadModelDlg::
updateColumns()
{
  columns_    .clear();
  columnTypes_.clear();

  if      (typeCombo_->currentIndex() == 0) { // CSV
    parseCSVColumns();
  }
  else if (typeCombo_->currentIndex() == 1) { // TSV
    parseTSVColumns();
  }

  //---

  auto *columnTypeMgr = charts()->columnTypeMgr();

  QStringList typeNames;

  columnTypeMgr->typeNames(typeNames);

  //---

  auto createTypeEdit = [&](const QString &type, const QString &colName) {
    auto *typeCombo = CQUtil::makeWidget<QComboBox>("typeCombo");

    typeCombo->addItems(typeNames);
    typeCombo->setToolTip("Column Type");

    int i = typeNames.indexOf(type);

    if (i >= 0)
      typeCombo->setCurrentIndex(i);

    typeCombo->setProperty("colName", colName);

    connect(typeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChangedSlot(int)));

    return typeCombo;
  };

  //---

  columnsTable_->clear();

  columnsTable_->setColumnCount(3);
  columnsTable_->setRowCount(columns_.size());

  auto createHeaderItem = [&](int c, const QString &name) {
    columnsTable_->setHorizontalHeaderItem(c, new QTableWidgetItem(name));
  };

  createHeaderItem(0, "Name");
  createHeaderItem(1, "Type");
  createHeaderItem(2, "Data");

  auto createTableItem = [&](const QString &name) {
    auto *item = new QTableWidgetItem(name);

    item->setToolTip(name);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    return item;
  };

  int i = 0;

  for (const auto &c : columns_) {
    auto *nameItem = createTableItem(c);

    auto p = columnTypes_.find(c);

    auto type = (p != columnTypes_.end() ? (*p).second : QString("string"));

    auto *typeItem = createTableItem(type);

    columnsTable_->setItem(i, 0, nameItem);
    columnsTable_->setItem(i, 1, typeItem);

    columnsTable_->setCellWidget(i, 1, createTypeEdit(type, c));

    auto pd = columnData_.find(c);

    if (pd != columnData_.end()) {
      auto nameValues = (*pd).second;

      QStringList strs;

      for (auto &nv : nameValues) {
        QStringList strs1;

        strs1 << nv.first;
        strs1 << nv.second;

        strs << CQTcl::mergeList(strs1);
      }

      auto str = CQTcl::mergeList(strs);

      auto *dataItem = createTableItem(str);

      columnsTable_->setItem(i, 2, dataItem);
    }

    ++i;
  }
}

void
CQChartsLoadModelDlg::
parseCSVColumns()
{
  QString headerText;

  bool hasHeader = false;

  if      (commentHeaderCheck_->isChecked()) {
    CQStrParse parse(firstComment_);

    parse.skipSpace();

    if (parse.isChar('#'))
      parse.skipChar();

    parse.skipSpace();

    headerText = parse.getAt();
    hasHeader  = true;
  }
  else if (firstLineHeaderCheck_->isChecked()) {
    headerText = firstLine_;
    hasHeader  = true;
  }
  else {
    headerText = firstLine_;
    hasHeader  = false;
  }

  CCsv csv;

  CCsv::Fields columns;

  csv.stringToColumns(headerText.toStdString(), columns);

  if (hasHeader) {
    for (auto &c : columns)
      columns_.push_back(QString::fromStdString(c));
  }
  else {
    int nc = columns.size();

    for (int ic = 1; ic <= nc; ++ic)
      columns_.push_back(QString::number(ic));
  }

  for (const auto &l : metaLines_) {
    CQStrParse parse(l);

    parse.skipSpace();

    if (parse.isChar('#'))
      parse.skipChar();

    parse.skipSpace();

    auto l1 = parse.getAt();

    CCsv::Fields metaColumns;

    csv.stringToColumns(l1.toStdString(), metaColumns);

    if (metaColumns.size() == 4) {
      if (metaColumns[0] == "column") {
        auto colName = QString::fromStdString(metaColumns[1]);

        if (metaColumns[2] == "type") {
          auto type = QString::fromStdString(metaColumns[3]);

          columnTypes_[colName] = type;
        }
        else {
          auto name  = QString::fromStdString(metaColumns[2]);
          auto value = QString::fromStdString(metaColumns[3]);

          columnData_[colName][name] = value;
        }
      }
    }
  }
}

void
CQChartsLoadModelDlg::
parseTSVColumns()
{
  QString headerText;

  bool hasHeader = false;

  if      (commentHeaderCheck_->isChecked()) {
    CQStrParse parse(firstComment_);

    parse.skipSpace();

    if (parse.isChar('#'))
      parse.skipChar();

    parse.skipSpace();

    headerText = parse.getAt();
    hasHeader  = true;
  }
  else if (firstLineHeaderCheck_->isChecked()) {
    headerText = firstLine_;
    hasHeader  = true;
  }
  else {
    headerText = firstLine_;
    hasHeader  = false;
  }

  CTsv tsv;

  CCsv::Fields columns;

  tsv.stringToColumns(headerText.toStdString(), columns);

  if (hasHeader) {
    for (auto &c : columns)
      columns_.push_back(QString::fromStdString(c));
  }
  else {
    int nc = columns.size();

    for (int ic = 1; ic <= nc; ++ic)
      columns_.push_back(QString::number(ic));
  }
}

void
CQChartsLoadModelDlg::
typeChangedSlot(int)
{
  auto *combo = qobject_cast<QComboBox *>(sender());
  if (! combo) return;

  auto type = combo->currentText();

  auto colName = combo->property("colName").toString();
  if (colName == "") return;

  columnTypes_[colName] = type;
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

  modelId_.clear();

  //----

  auto filename = fileEdit_->name();
  auto type     = typeCombo_->currentText();

  //----

  auto fileType = CQChartsFileTypeUtil::stringToFileType(type);

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

  auto *model = loadFile(filename, type, inputData, hierarchical);
  if (! model) return false;

  ModelP modelP(model);

  auto *modelData = charts()->initModelData(modelP);

  charts()->setModelName(modelData, filename);

  //---

  int ic = 0;

  for (const auto &c : columns_) {
    auto p = columnTypes_.find(c);

    if (p != columnTypes_.end()) {
      const auto &type = (*p).second;

      CQChartsModelUtil::setColumnTypeStr(charts(), modelP.data(), CQChartsColumn(ic), type);
    }

    ++ic;
  }

  //---

  modelInd_ = modelData->ind();
  modelId_  = modelData->id();

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

  CQChartsFile file(charts_, filename);

  return loader.loadFile(file, type, inputData, hierarchical);
}

void
CQChartsLoadModelDlg::
cancelSlot()
{
  modelInd_ = -1;

  modelId_.clear();

  hide();
}

QSize
CQChartsLoadModelDlg::
sizeHint() const
{
  QFontMetrics fm(font());

  int w = fm.width("X")*65;
  int h = fm.height()*45;

  return QSize(w, h);
}
