#include <CQChartsPlotDlg.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsXYPlot.h>
#include <CQChartsPlotParameter.h>
#include <CQChartsAxis.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QStackedWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <cassert>

CQChartsPlotDlg::
CQChartsPlotDlg(CQCharts *charts, const ModelP &model) :
 charts_(charts), model_(model)
{
  setObjectName("plotDlg");

  setWindowTitle("Create Plot");

  //----

  QVBoxLayout *layout = new QVBoxLayout(this);

  //----

  QGroupBox *typeGroup = new QGroupBox("Type Data");

  typeGroup->setObjectName("typeGroup");

  QVBoxLayout *typeLayout = new QVBoxLayout(typeGroup);

  layout->addWidget(typeGroup);

  //--

  QHBoxLayout *typeComboLayout = new QHBoxLayout;

  typeLayout->addLayout(typeComboLayout);

  QLabel *typeLabel = new QLabel("Type");

  typeLabel->setObjectName("typeLabel");

  typeComboLayout->addWidget(typeLabel);

  //--

  QComboBox *typeCombo = new QComboBox;

  typeCombo->setObjectName("typeCombo");

  typeComboLayout->addWidget(typeCombo);

  QStringList names, descs;

  this->charts()->getPlotTypeNames(names, descs);

  typeCombo->addItems(descs);

  connect(typeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboSlot(int)));

  //--

  typeComboLayout->addStretch(1);

  //----

  stack_ = new QStackedWidget;

  stack_->setObjectName("stack");

  typeLayout->addWidget(stack_);

  for (int i = 0; i < names.size(); ++i)
    addPlotWidgets(names[i], i);

  //----

  QFrame *sep1 = new QFrame;

  sep1->setObjectName("sep1");
  sep1->setFixedHeight(4);
  sep1->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  layout->addWidget(sep1);

  //----

  QGroupBox *genGroup = new QGroupBox("General Data");

  genGroup->setObjectName("genGroup");

  QGridLayout *genLayout = new QGridLayout(genGroup);

  layout->addWidget(genGroup);

  //--

  int row = 0, column = 0;

  viewEdit_ = addLineEdit(genLayout, row, column, "View Name", "viewEdit", "View Name");

  //--

  ++row; column = 0;

  posEdit_ = addLineEdit(genLayout, row, column, "Position", "position", "Position");

  posEdit_->setText("0 0 1 1");

  //--

  ++row; column = 0;

  titleEdit_ = addLineEdit(genLayout, row, column, "Title", "title", "Title");

  //--

  ++row; column = 0;

  xminEdit_ = addLineEdit(genLayout, row, column, "XMin", "xmin", "X Axis Minimum Value");
  yminEdit_ = addLineEdit(genLayout, row, column, "YMin", "ymin", "Y Axis Minimum Value");

  ++row; column = 0;

  xmaxEdit_ = addLineEdit(genLayout, row, column, "XMax", "xmax", "X Axis Maximum Value");
  ymaxEdit_ = addLineEdit(genLayout, row, column, "YMax", "ymax", "Y Axis Maximum Value");

  //--

  ++row; column = 0;

  xintegralCheck_ = new QCheckBox("X Integral");
  yintegralCheck_ = new QCheckBox("Y Integral");

  xintegralCheck_->setObjectName("xintegralCheck");
  yintegralCheck_->setObjectName("yintegralCheck");

  genLayout->addWidget(xintegralCheck_, row, column); ++column;
  genLayout->addWidget(yintegralCheck_, row, column);

  //----

  QFrame *sep2 = new QFrame;

  sep2->setObjectName("sep2");
  sep2->setFixedHeight(4);
  sep2->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  layout->addWidget(sep2);

  //----

  QHBoxLayout *buttonLayout = new QHBoxLayout;

  layout->addLayout(buttonLayout);

  //--

  QPushButton *okButton = new QPushButton("OK");

  okButton->setObjectName("ok");

  connect(okButton, SIGNAL(clicked()), this, SLOT(okSlot()));

  buttonLayout->addWidget(okButton);

  //--

  QPushButton *applyButton = new QPushButton("Apply");

  applyButton->setObjectName("apply");

  connect(applyButton, SIGNAL(clicked()), this, SLOT(applySlot()));

  buttonLayout->addWidget(applyButton);

  //--

  QPushButton *cancelButton = new QPushButton("Cancel");

  cancelButton->setObjectName("cancel");

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  buttonLayout->addWidget(cancelButton);

  //--

  buttonLayout->addStretch(1);
}

void
CQChartsPlotDlg::
addPlotWidgets(const QString &typeName, int ind)
{
  CQChartsPlotType *type = charts()->plotType(typeName);
  assert(type);

  //

  QFrame *frame = new QFrame;

  frame->setObjectName("frame");

  stack_->addWidget(frame);

  //---

  QGridLayout *frameLayout = new QGridLayout(frame);

  int row = 0;

  PlotData &plotData = typePlotData_[type->name()];

  addParameterEdits(type, plotData, frameLayout, row);

  frameLayout->setRowStretch(row, 1);

  //---

  plotData.ind = ind;

  tabTypeName_[plotData.ind] = type->name();
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

    ++row;
  }
}

void
CQChartsPlotDlg::
addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                       const CQChartsPlotParameter &parameter)
{
  int column = 0;

  bool ok;

  int pColumn = parameter.defValue().toInt(&ok);

  QLineEdit *columnEdit =
    addLineEdit(layout, row, column, parameter.desc(), parameter.name() + "Column",
                "Column Name or Number");

  if (ok)
    columnEdit->setText(QString("%1").arg(pColumn));

  plotData.columnEdits[parameter.name()] = columnEdit;

  //---

  QLineEdit *formatEdit =
    addLineEdit(layout, row, column, "", parameter.name() + "Format", "Column Format");

  plotData.formatEdits[parameter.name()] = formatEdit;

  //---

  ++row;
}

void
CQChartsPlotDlg::
addParameterColumnsEdit(PlotData &plotData, QGridLayout *layout, int &row,
                        const CQChartsPlotParameter &parameter)
{
  int column = 0;

  QLineEdit *columnsEdit =
    addLineEdit(layout, row, column, parameter.desc(), parameter.name() + "Columns",
                "Column Names or Numbers");

  columnsEdit->setText(parameter.defValue().toString());

  plotData.columnsEdits[parameter.name()] = columnsEdit;

  //---

  QLineEdit *formatEdit =
    addLineEdit(layout, row, column, "", parameter.name() + "Format", "Columns Format");

  plotData.formatEdits[parameter.name()] = formatEdit;

  //---

  ++row;
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
addLineEdit(QGridLayout *grid, int &row, int &column, const QString &name,
            const QString &objName, const QString &placeholderText) const
{
  if (name != "") {
    QLabel *label = new QLabel(name);

    label->setObjectName(objName + "Label");

    grid->addWidget(label, row, column); ++column;
  }

  QLineEdit *edit = new QLineEdit;

  edit->setObjectName(objName + "Edit" );

  edit->setPlaceholderText(placeholderText);

  grid->addWidget(edit , row, column); ++column;

  return edit;
}

void
CQChartsPlotDlg::
comboSlot(int ind)
{
  stack_->setCurrentIndex(ind);
}

void
CQChartsPlotDlg::
okSlot()
{
  if (applySlot())
    accept();
}

bool
CQChartsPlotDlg::
applySlot()
{
  int ind = stack_->currentIndex();

  // create plot for typename of current tab
  QString typeName = tabTypeName_[ind];

  if (! charts()->isPlotType(typeName))
    return false;

  // TODO: get view from name
  QString viewId = viewEdit_->text();

  CQChartsView *view = charts()->getView(viewId);

  if (! view) {
    view = charts()->addView();

    view->show();
  }

  CQChartsPlotType *type = charts()->plotType(typeName);
  assert(type);

  plot_ = type->create(view, model_);

  //---

  // set plot property for widgets for plot parameters
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
        charts()->errorMsg("Failed to set parameter " + parameter.propName());

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
        charts()->errorMsg("Failed to set parameter " + parameter.propName());

      if (columnType.length() && ! columns.empty())
        model_->setHeaderData(columns[0], Qt::Horizontal, columnType, CQCharts::Role::ColumnType);
    }
    else if (parameter.type() == "bool") {
      bool b = parameter.defValue().toBool();

      parseParameterBoolEdit(parameter, plotData, b);

      if (! CQUtil::setProperty(plot_, parameter.propName(), QVariant(b)))
        charts()->errorMsg("Failed to set parameter " + parameter.propName());
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

  double xmin = 0.0, ymin = 0.0, xmax = 1.0, ymax = 1.0;

  parsePosition(xmin, ymin, xmax, ymax);

  //---

  if (titleEdit_->text().length())
    plot_->setTitle(titleEdit_->text());

  if (xintegralCheck_->isChecked())
    plot_->xAxis()->setIntegral(true);

  if (yintegralCheck_->isChecked())
    plot_->yAxis()->setIntegral(true);

  if (xminEdit_->text().length()) {
    bool ok; double xmin = xminEdit_->text().toDouble(&ok);
    if (ok) plot_->setXMin(xmin);
  }

  if (yminEdit_->text().length()) {
    bool ok; double ymin = yminEdit_->text().toDouble(&ok);
    if (ok) plot_->setYMin(ymin);
  }

  if (xmaxEdit_->text().length()) {
    bool ok; double xmax = xmaxEdit_->text().toDouble(&ok);
    if (ok) plot_->setXMax(xmax);
  }

  if (ymaxEdit_->text().length()) {
    bool ok; double ymax = ymaxEdit_->text().toDouble(&ok);
    if (ok) plot_->setYMax(ymax);
  }

  //---

  int n = view->numPlots();

  CBBox2D bbox(1000*xmin, 1000*ymin, 1000*xmax, 1000*ymax);

  plot_->setId(QString("Chart.%1").arg(n + 1));

  view->addPlot(plot_, bbox);

  //---

  emit plotCreated(plot_);

  return true;
}

bool
CQChartsPlotDlg::
parsePosition(double &xmin, double &ymin, double &xmax, double &ymax) const
{
  QString posStr = posEdit_->text();

  QStringList posStrs = posStr.split(" ", QString::SkipEmptyParts);

  if (posStrs.length() == 4) {
    bool ok1; xmin = posStrs[0].toDouble(&ok1); if (! ok1) xmin = 0.0;
    bool ok2; ymin = posStrs[1].toDouble(&ok2); if (! ok2) ymin = 0.0;
    bool ok3; xmax = posStrs[2].toDouble(&ok3); if (! ok3) xmax = 0.0;
    bool ok4; ymax = posStrs[3].toDouble(&ok4); if (! ok4) ymax = 0.0;

    xmin = CQChartsUtil::clamp(xmin, 0.0, 1.0);
    ymin = CQChartsUtil::clamp(ymin, 0.0, 1.0);
    xmax = CQChartsUtil::clamp(xmax, 0.0, 1.0);
    ymax = CQChartsUtil::clamp(ymax, 0.0, 1.0);

    if (xmin > xmax) std::swap(xmin, xmax);
    if (ymin > ymax) std::swap(ymin, ymax);

    if (xmin == xmax) {
      if (xmin > 0.0) xmin = 0.0;
      else            xmax = 1.0;
    }

    if (ymin == ymax) {
      if (ymin > 0.0) ymin = 0.0;
      else            ymax = 1.0;
    }

    return (ok1 && ok2 && ok3 && ok4);
  }

  return false;
}

bool
CQChartsPlotDlg::
parseParameterColumnEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                         int &column, QString &columnType)
{
  auto pf = plotData.formatEdits.find(parameter.name());
  assert(pf != plotData.formatEdits.end());

  QString format = (*pf).second->text();

  if (format != "")
    columnType = format;

  //---

  bool ok;

  int defColumn = parameter.defValue().toInt(&ok);

  if (! ok)
    defColumn = -1;

  auto pe = plotData.columnEdits.find(parameter.name());
  assert(pe != plotData.columnEdits.end());

  if (! lineEditValue((*pe).second, column, columnType, defColumn))
    return false;

  return true;
}

bool
CQChartsPlotDlg::
parseParameterColumnsEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                          std::vector<int> &columns, QString &columnType)
{
  auto pf = plotData.formatEdits.find(parameter.name());
  assert(pf != plotData.formatEdits.end());

  QString format = (*pf).second->text();

  if (format != "")
    columnType = format;

  //---

  columns.clear();

  auto pe = plotData.columnsEdits.find(parameter.name());
  assert(pe != plotData.columnsEdits.end());

  int column;

  bool ok = lineEditValue((*pe).second, column, columnType, -1);

  if (ok) {
    columns.push_back(column);

    return true;
  }

  return lineEditValues((*pe).second, columns, columnType);
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

int
CQChartsPlotDlg::
exec()
{
  plot_ = nullptr;

  return QDialog::exec();
}
