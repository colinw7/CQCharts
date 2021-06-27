#include <CQExcelFrame.h>
#include <CQExcelView.h>
#include <CQExcelModel.h>
#if 1
#include <CCsv.h>
#endif
#include <CQCsvModel.h>
#include <CQModelUtil.h>
#include <CQUtil.h>
#include <CQIconButton.h>

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QAbstractProxyModel>
#include <QFileDialog>

#include <svg/load_svg.h>
#include <svg/save_svg.h>

#include <fstream>
#include <iostream>

namespace CQExcel {

Frame::
Frame(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("excelFrame");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //--

  auto *toolbarFrame  = CQUtil::makeWidget<QFrame>("toolbarFrame");
  auto *toolbarLayout = CQUtil::makeLayout<QHBoxLayout>(toolbarFrame, 0, 2);

  nameLabel_    = CQUtil::makeWidget<QLabel>("nameLabel");
  functionEdit_ = CQUtil::makeWidget<QLineEdit>("functionEdit");

  connect(functionEdit_, SIGNAL(returnPressed()), this, SLOT(functionSlot()));

  toolbarLayout->addWidget(nameLabel_);
  toolbarLayout->addWidget(functionEdit_);

  auto *loadButton = CQUtil::makeLabelWidget<CQIconButton>("LOAD", "load");
  auto *saveButton = CQUtil::makeLabelWidget<CQIconButton>("SAVE", "save");

  loadButton->setToolTip("Load CSV");
  saveButton->setToolTip("Save CSV");

  connect(loadButton, SIGNAL(clicked()), this, SLOT(loadSlot()));
  connect(saveButton, SIGNAL(clicked()), this, SLOT(saveSlot()));

  toolbarLayout->addWidget(loadButton);
  toolbarLayout->addWidget(saveButton);

  layout->addWidget(toolbarFrame);

  //--

  view_ = new View(this);

  connect(view_, SIGNAL(currentIndexChanged()), this, SLOT(currentIndexSlot()));
  connect(view_, SIGNAL(currentSelectionChanged()), this, SLOT(selectionSlot()));

  layout->addWidget(view_);
}

bool
Frame::
loadCsv(const QString &filename, const LoadOptions &options)
{
  auto *model = view()->excelModel();

#if 1
  // parse file into array of fields
  CCsv csv(filename.toStdString());

  csv.setCommentHeader  (options.comment_header);
  csv.setFirstLineHeader(options.first_line_header);
//csv.setSeparator      (",");

  if (! csv.load())
    return false;

  //---

  // get header data and rows from csv
  const CCsv::Fields &header = csv.header();
  const CCsv::Data   &data   = csv.data();

  //---

  using Cells = std::vector<QVariant>;
  using Data  = std::vector<Cells>;

  Cells hheader;
  Cells vheader;
  Data  cdata;

  //---

  int numColumns = 0;

  // add header to model
  if (! header.empty()) {
    int i = 0;

    for (const auto &f : header) {
      if (i == 0 && options.first_column_header) {
        //vheader.push_back(f.c_str());
      }
      else
        hheader.push_back(f.c_str());

      ++i;
    }

    numColumns = std::max(numColumns, int(hheader.size()));
  }

  //---

  // expand horizontal header to max number of columns
  for (const auto &fields : data) {
    int numFields = fields.size();

    if (options.first_column_header)
      --numFields;

    numColumns = std::max(numColumns, numFields);
  }

  while (int(hheader.size()) < numColumns)
    hheader.push_back("");

  //---

  // add fields to model
  int nr = 0;

  for (const auto &fields : data) {
    // get row vertical header and cells
    Cells   cells;
    QString vheader;

    int i = 0;

    for (const auto &f : fields) {
      if (i == 0 && options.first_column_header)
        vheader = f.c_str();
      else
        cells.push_back(f.c_str());

      ++i;
    }

    //---

    // add row vertical header and cells to model
    if (options.first_column_header)
      vheader.push_back(vheader);

    cdata.push_back(cells);

    //---

    // stop if hit maximum rows
    ++nr;

#if 0
    if (maxRows() > 0 && nr >= maxRows())
      break;
#endif
  }

  //---

  // expand vertical header to number of rows
  int numRows = cdata.size();

  while (int(vheader.size()) < numRows)
    vheader.push_back("");

  //---

  // load model
  model->beginResetModel();

  model->resizeModel(numColumns, numRows);

  for (int ir = 0; ir < numRows; ++ir) {
    for (int ic = 0; ic < numColumns; ++ic) {
      auto ind = model->index(ir, ic, QModelIndex());

      const auto &row = cdata[ir];
      int nc = row.size();
      if (ic >= nc) continue;

      model->setData(ind, row[ic], Qt::DisplayRole);
    }
  }

  for (int ic = 0; ic < numColumns; ++ic)
    model->setHeaderData(ic, Qt::Horizontal, hheader[ic], Qt::DisplayRole);

  if (! vheader.empty()) {
    for (int ir = 0; ir < numRows; ++ir)
      model->setHeaderData(ir, Qt::Vertical, vheader[ir], Qt::DisplayRole);
  }

  model->endResetModel();

  //---

  // process meta data
  auto meta = csv.meta();

  if (! meta.empty()) {
    for (const auto &fields : meta) {
      int numFields = fields.size();

      // handle column data:
      //   column <column_name> <value_type> <value>
      if      (fields[0] == "column") {
      }
      // handle cell data:
      //   cell <cell_index> <role> <value>
      else if (fields[0] == "cell") {
        if (numFields != 4) {
          std::cerr << "Invalid number of values for cell meta data\n";
          continue;
        }

        auto indStr  = QString::fromStdString(fields[1]);
        auto roleStr = QString::fromStdString(fields[2]);
        auto value   = QString::fromStdString(fields[3]);

        // get role
        int role = CQModelUtil::nameToRole(roleStr);

        if (role < 0) {
          std::cerr << "Invalid role '" << fields[2] << "'\n";
          continue;
        }

        // get cell index
        int row = -1, col = -1;

        if (! CQModelUtil::stringToRowCol(indStr, row, col)) {
          std::cerr << "Invalid index '" << fields[1] << "'\n";
          continue;
        }

        auto ind = model->index(row, col, QModelIndex());

        model->setData(ind, value, role);
      }
      // handle global data
      //   global <> <name> <value>
      else if (fields[0] == "global") {
      }
      else {
        std::cerr << "Unknown meta data '" << fields[0] << "\n";
        continue;
      }
    }
  }
#else
  CQCsvModel csvModel;

  csvModel.setCommentHeader    (options.comment_header);
  csvModel.setFirstLineHeader  (options.first_line_header);
  csvModel.setFirstColumnHeader(options.first_column_header);

  if (! csvModel.load(filename))
    return false;

  model->copyModel(&csvModel);
#endif

  //---

  return true;
}

void
Frame::
setModel(QAbstractItemModel *model)
{
  auto *proxyModel = qobject_cast<QAbstractProxyModel *>(model);

  Model *excelModel;

  if (proxyModel)
    excelModel = qobject_cast<Model *>(proxyModel->sourceModel());
  else
    excelModel = qobject_cast<Model *>(model);

  assert(excelModel);

  view_->setExcelModel(model);
}

void
Frame::
functionSlot()
{
  const auto &text = functionEdit_->text().trimmed();

  view()->applyFunction(text);
}

void
Frame::
loadSlot()
{
  auto fileName = QFileDialog::getOpenFileName(this, "Load CSV", "excel.csv",
                                               "CSV Files (*.csv)");
  if (! fileName.length()) return;

  LoadOptions options;

  options.first_line_header = true;

  loadCsv(fileName, options);
}

void
Frame::
saveSlot()
{
  auto fileName = QFileDialog::getSaveFileName(this, "Save CSV", "excel.csv",
                                               "CSV Files (*.csv)");
  if (! fileName.length()) return;

  CQCsvModel::MetaData meta;

  //---

  Model::CellExpr cellExpr;

  view_->excelModel()->getCellExpressions(cellExpr);

  for (const auto &pe : cellExpr) {
    const auto &ind  = pe.first;
    const auto &expr = pe.second;

    auto indStr = QString("%1:%2").arg(ind.row()).arg(ind.column());

    std::vector<std::string> strs;

    strs.push_back("cell");
    strs.push_back(indStr.toStdString());
    strs.push_back("edit");
    strs.push_back(expr.toStdString());

    meta.push_back(strs);
  }

  //---

  Model::CellStyle cellStyle;

  view_->excelModel()->getCellStyles(cellStyle);

  for (const auto &pc : cellStyle) {
    const auto &ind   = pc.first;
    const auto &style = pc.second;

    auto indStr = QString("%1:%2").arg(ind.row()).arg(ind.column());

    std::vector<std::string> strs;

    strs.push_back("cell");
    strs.push_back(indStr.toStdString());
    strs.push_back("style");
    strs.push_back(style.toString().toStdString());

    meta.push_back(strs);
  }

  //---

  std::ofstream os(fileName.toStdString());

  CQCsvModel::ConfigData configData;

  configData.dataRole = int(CQBaseModelRole::Export);

  configData.firstLineHeader = true;

  CQCsvModel::save(view_->excelModel(), os, configData, meta);
  //  saveAs(fileName);
}

void
Frame::
currentIndexSlot()
{
  QString cellName, functionText;

  view()->getIndexDetails(view()->currentIndex(), cellName, functionText);

  nameLabel   ()->setText(cellName);
  functionEdit()->setText(functionText);
}

void
Frame::
selectionSlot()
{
  QString cellName;

  view()->getSelectionDetails(cellName);

  nameLabel()->setText(cellName);
}

QSize
Frame::
sizeHint() const
{
  return QSize(1024, 1024);
}

}
