#include <CQChartsPlotDlg.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsXYPlot.h>
#include <CQChartsPlotParameter.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <cassert>

CQChartsPlotDlg::
CQChartsPlotDlg(CQChartsView *view, QAbstractItemModel *model) :
 view_(view), model_(model)
{
  setObjectName("plotDlg");

  QVBoxLayout *layout = new QVBoxLayout(this);

  createTab();

  layout->addWidget(tab_);
}

int
CQChartsPlotDlg::
exec()
{
  plot_ = nullptr;

  return QDialog::exec();
}

void
CQChartsPlotDlg::
createTab()
{
  tab_ = new QTabWidget;

  tab_->setObjectName("tab");

  tab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QStringList names, descs;

  view_->charts()->getPlotTypes(names, descs);

  for (int i = 0; i < descs.size(); ++i) {
    addPlotTab(names[i]);
  }
}

void
CQChartsPlotDlg::
addPlotTab(const QString &typeName)
{
  CQChartsPlotType *type = view_->charts()->plotType(typeName);
  assert(type);

  //

  QFrame *frame = new QFrame;

  frame->setObjectName("frame");

  QVBoxLayout *frameLayout = new QVBoxLayout(frame);

  //---

  QGridLayout *editLayout = new QGridLayout;

  frameLayout->addLayout(editLayout);

  int row = 0;

  PlotData &plotData = typePlotData_[type->name()];

  addParameterEdits(type, plotData, editLayout, row);

  editLayout->setRowStretch(row, 1);

  //---

  QHBoxLayout *buttonLayout = new QHBoxLayout;

  plotData.okButton = new QPushButton("OK");

  plotData.okButton->setObjectName("ok");

  connect(plotData.okButton, SIGNAL(clicked()), this, SLOT(okSlot()));

  buttonLayout->addWidget(plotData.okButton);

  QPushButton *cancelButton = new QPushButton("Cancel");

  cancelButton->setObjectName("cancel");

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  buttonLayout->addWidget(cancelButton);

  buttonLayout->addStretch(1);

  frameLayout->addLayout(buttonLayout);

  //---

  plotData.tabInd = tab_->addTab(frame, type->desc());

  tabTypeName_[plotData.tabInd] = type->name();
}

void
CQChartsPlotDlg::
addParameterEdits(CQChartsPlotType *type, PlotData &plotData, QGridLayout *layout, int &row)
{
  int nbool = 0;

  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column")
      addParameterColumnEdit(plotData, layout, row, parameter);
    else if (parameter.type() == "columns")
      addParameterColumnsEdit(plotData, layout, row, parameter);
    else if (parameter.type() == "bool")
      ++nbool;
    else
      assert(false);
  }

  if (nbool > 0) {
    QHBoxLayout *boolLayout = new QHBoxLayout;

    for (const auto &parameter : type->parameters()) {
      if (parameter.type() == "bool")
        addParameterBoolEdit(plotData, boolLayout, parameter);
    }

    boolLayout->addStretch(1);

    layout->addLayout(boolLayout, row, 0, 1, 2);
  }
}

void
CQChartsPlotDlg::
addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                       const CQChartsPlotParameter &parameter)
{
  bool ok;

  int column = parameter.defValue().toInt(&ok);

  QLineEdit *le = addLineEdit(layout, row, parameter.desc(), parameter.name());

  if (ok)
    le->setText(QString("%1").arg(column));

  plotData.columnEdits[parameter.name()] = le;
}

void
CQChartsPlotDlg::
addParameterColumnsEdit(PlotData &plotData, QGridLayout *layout, int &row,
                        const CQChartsPlotParameter &parameter)
{
  QLineEdit *le = addLineEdit(layout, row, parameter.desc(), parameter.name());

  le->setText(parameter.defValue().toString());

  plotData.columnsEdits[parameter.name()] = le;
}

void
CQChartsPlotDlg::
addParameterBoolEdit(PlotData &plotData, QHBoxLayout *layout,
                     const CQChartsPlotParameter &parameter)
{
  bool b = parameter.defValue().toBool();

  QCheckBox *checkBox = new QCheckBox(parameter.desc());

  checkBox->setObjectName(parameter.name());

  checkBox->setChecked(b);

  layout->addWidget(checkBox);

  plotData.boolEdits[parameter.name()] = checkBox;
}

QLineEdit *
CQChartsPlotDlg::
addLineEdit(QGridLayout *grid, int &row, const QString &name, const QString &objName) const
{
  QLabel    *label = new QLabel(name);
  QLineEdit *edit  = new QLineEdit;

  label->setObjectName(objName + "Label");
  label->setObjectName(objName + "Edit" );

  grid->addWidget(label, row, 0);
  grid->addWidget(edit , row, 1);

  ++row;

  return edit;
}

void
CQChartsPlotDlg::
okSlot()
{
  // create plot for typename of current tab
  QString typeName = tabTypeName_[tab_->currentIndex()];

  if (! view_->charts()->isPlotType(typeName))
    return;

  CQChartsPlotType *plotType = view_->charts()->plotType(typeName);

  plot_ = plotType->create(view_, model_);

  //---

  // set plot property for widgets for plot parameters
  CQChartsPlotType *type = view_->charts()->plotType(typeName);
  assert(type);

  PlotData &plotData = typePlotData_[typeName];

  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column") {
      bool ok;

      int column = parameter.defValue().toInt(&ok);

      if (! ok)
        column = -1;

      QString columnType;

      parseParameterColumnEdit(parameter, plotData, column, columnType);

      if (! CQUtil::setProperty(plot_, parameter.propName(), QVariant(column)))
        std::cerr << "Failed to set parameter " << parameter.propName().toStdString() << std::endl;

      if (columnType.length())
        model_->setHeaderData(column, Qt::Horizontal, columnType, CQCharts::Role::ColumnType);
    }
    else if (parameter.type() == "columns") {
      QString columnsStr = parameter.defValue().toString();

      std::vector<int> columns;

      (void) CQChartsUtil::fromString(columnsStr, columns);

      QString columnType;

      parseParameterColumnsEdit(parameter, plotData, columns, columnType);

      QString s = CQChartsUtil::toString(columns);

      if (! CQUtil::setProperty(plot_, parameter.propName(), QVariant(s)))
        std::cerr << "Failed to set parameter " << parameter.propName().toStdString() << std::endl;

      if (columnType.length() && ! columns.empty())
        model_->setHeaderData(columns[0], Qt::Horizontal, columnType, CQCharts::Role::ColumnType);
    }
    else if (parameter.type() == "bool") {
      bool b = parameter.defValue().toBool();

      parseParameterBoolEdit(parameter, plotData, b);

      if (! CQUtil::setProperty(plot_, parameter.propName(), QVariant(b)))
        std::cerr << "Failed to set parameter " << parameter.propName().toStdString() << std::endl;
    }
    else
      assert(false);
  }

  //---

  // init plot
  if (typeName == "xy") {
    CQChartsXYPlot *xyPlot = dynamic_cast<CQChartsXYPlot *>(plot_);
    assert(xyPlot);

    if      (xyPlot->isBivariate()) {
      xyPlot->setFillUnder(true);
      xyPlot->setPoints   (false);
    }
    else if (xyPlot->isStacked()) {
      xyPlot->setFillUnder(true);
      xyPlot->setPoints   (false);
    }
  }

  //---

  emit plotCreated(plot_);

  //---

  accept();
}

bool
CQChartsPlotDlg::
parseParameterColumnEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                         int &column, QString &columnType)
{
  bool ok;

  int defColumn = parameter.defValue().toInt(&ok);

  if (! ok)
    defColumn = -1;

  auto p = plotData.columnEdits.find(parameter.name());
  assert(p != plotData.columnEdits.end());

  if (! lineEditValue((*p).second, column, columnType, defColumn))
    return false;

  return true;
}

bool
CQChartsPlotDlg::
parseParameterColumnsEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                          std::vector<int> &columns, QString &columnType)
{
  auto p = plotData.columnsEdits.find(parameter.name());
  assert(p != plotData.columnsEdits.end());

  int column;

  bool ok = lineEditValue((*p).second, column, columnType, -1);

  if (ok) {
    columns.clear();

    columns.push_back(column);

    return true;
  }

  columns.clear();

  return lineEditValues((*p).second, columns, columnType);
}

bool
CQChartsPlotDlg::
parseParameterBoolEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData, bool &b)
{
  auto p = plotData.boolEdits.find(parameter.name());
  assert(p != plotData.boolEdits.end());

  b = (*p).second->isChecked();

  return true;
}

bool
CQChartsPlotDlg::
lineEditValue(QLineEdit *le, int &i, QString &columnType, int defi) const
{
  QString str = le->text();

  //--

  int pos = str.indexOf(":");

  if (pos >= 0) {
    str = str.mid(0, pos).simplified();

    columnType = str.mid(pos + 1).simplified();
  }
  else
    str = str.simplified();

  if (! stringToColumn(str, i)) {
    i = defi;

    return false;
  }

  return true;
}

bool
CQChartsPlotDlg::
stringToColumn(const QString &str, int &column) const
{
  bool ok = false;

  int column1 = str.toInt(&ok);

  if (ok) {
    column = column1;

    return true;
  }

  //---

  if (! str.length())
    return false;

  for (int column1 = 0; column1 < model_->columnCount(); ++column1) {
    QVariant var = model_->headerData(column1, Qt::Horizontal, Qt::DisplayRole);

    if (! var.isValid())
      continue;

    if (var.toString() == str) {
      column = column1;
      return true;
    }
  }

  return false;
}

bool
CQChartsPlotDlg::
lineEditValues(QLineEdit *le, std::vector<int> &ivals, QString &columnType) const
{
  bool ok = true;

  QStringList strs = le->text().split(" ", QString::SkipEmptyParts);

  for (int i = 0; i < strs.size(); ++i) {
    int pos = strs[i].indexOf(":");

    QString lhs, rhs;

    if (pos > 0) {
      lhs = strs[i].mid(0, pos).simplified();
      rhs = strs[i].mid(pos + 1).simplified();
    }
    else
      lhs = strs[i].simplified();

    //---

    bool ok1;

    int col = lhs.toInt(&ok1);

    if (ok1) {
      ivals.push_back(col);

      if (rhs.length())
        columnType = rhs;
    }
    else
      ok = false;
  }

  return ok;
}
