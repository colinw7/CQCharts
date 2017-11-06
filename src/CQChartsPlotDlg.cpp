#include <CQChartsPlotDlg.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotParameter.h>
#include <CQChartsAxis.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QItemSelectionModel>
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
setSelectionModel(QItemSelectionModel *sm)
{
  selectionModel_ = sm;
}

QItemSelectionModel *
CQChartsPlotDlg::
selectionModel() const
{
  return selectionModel_.data();
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
  int nstr  = 0;
  int nreal = 0;

  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column")
      addParameterColumnEdit(plotData, layout, row, parameter);
    else if (parameter.type() == "columns")
      addParameterColumnsEdit(plotData, layout, row, parameter);
    else if (parameter.type() == "string")
      ++nstr;
    else if (parameter.type() == "real")
      ++nreal;
    else if (parameter.type() == "bool")
      ++nbool;
    else
      assert(false);
  }

  if (nstr > 0 || nreal > 0) {
    QHBoxLayout *strLayout = new QHBoxLayout;

    for (const auto &parameter : type->parameters()) {
      if      (parameter.type() == "string")
        addParameterStringEdit(plotData, strLayout, parameter);
      else if (parameter.type() == "real")
        addParameterRealEdit(plotData, strLayout, parameter);
    }

    strLayout->addStretch(1);

    layout->addLayout(strLayout, row, 0, 1, 2);

    ++row;
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
addParameterStringEdit(PlotData &plotData, QHBoxLayout *layout,
                       const CQChartsPlotParameter &parameter)
{
  QString str = parameter.defValue().toString();

  QHBoxLayout *editLayout = new QHBoxLayout;

  QLabel    *label = new QLabel(parameter.desc());
  QLineEdit *edit  = new QLineEdit;

  label->setObjectName(parameter.name() + "_label");
  label->setObjectName(parameter.name() + "_edit");

  edit->setText(str);

  editLayout->addWidget(label);
  editLayout->addWidget(edit);

  layout->addLayout(editLayout);

  plotData.stringEdits[parameter.name()] = edit;
}

void
CQChartsPlotDlg::
addParameterRealEdit(PlotData &plotData, QHBoxLayout *layout,
                     const CQChartsPlotParameter &parameter)
{
  double r = parameter.defValue().toDouble();

  QHBoxLayout *editLayout = new QHBoxLayout;

  QLabel    *label = new QLabel(parameter.desc());
  QLineEdit *edit  = new QLineEdit;

  label->setObjectName(parameter.name() + "_label");
  label->setObjectName(parameter.name() + "_edit");

  edit->setText(QString("%1").arg(r));

  editLayout->addWidget(label);
  editLayout->addWidget(edit);

  layout->addLayout(editLayout);

  plotData.realEdits[parameter.name()] = edit;
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

    CQChartsWindow *window = new CQChartsWindow(view);

    window->show();
  }

  CQChartsPlotType *type = charts()->plotType(typeName);
  assert(type);

  plot_ = type->create(view, model_);

  if (selectionModel())
    plot_->setSelectionModel(selectionModel());

  //---

  // set plot property for widgets for plot parameters
  PlotData &plotData = typePlotData_[typeName];

  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column") {
      bool ok;

      int column = parameter.defValue().toInt(&ok);

      if (! ok)
        column = -1;

      QString columnStr;
      QString columnTypeStr;

      parseParameterColumnEdit(parameter, plotData, column, columnStr, columnTypeStr);

      if (! CQUtil::setProperty(plot_, parameter.propName(), QVariant(column)))
        charts()->errorMsg("Failed to set parameter " + parameter.propName());

      if (columnTypeStr.length()) {
        CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

        CQChartsNameValues nameValues;

        CQChartsColumnType *typeData = columnTypeMgr->decodeTypeData(columnTypeStr, nameValues);

        if (typeData)
          columnTypeMgr->setModelColumnType(model_.data(), column, typeData->type(), nameValues);
      }
    }
    else if (parameter.type() == "columns") {
      QString columnsStr = parameter.defValue().toString();

      std::vector<int> columns;

      (void) CQChartsUtil::fromString(columnsStr, columns);

      QStringList columnStrs;
      QString     columnTypeStr;

      parseParameterColumnsEdit(parameter, plotData, columns, columnStrs, columnTypeStr);

      QString s = CQChartsUtil::toString(columns);

      if (! CQUtil::setProperty(plot_, parameter.propName(), QVariant(s)))
        charts()->errorMsg("Failed to set parameter " + parameter.propName());

      if (columnTypeStr.length() && ! columns.empty()) {
        CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

        CQChartsNameValues nameValues;

        CQChartsColumnType *typeData = columnTypeMgr->decodeTypeData(columnTypeStr, nameValues);

        if (typeData)
          columnTypeMgr->setModelColumnType(model_.data(), columns[0],
                                            typeData->type(), nameValues);
      }
    }
    else if (parameter.type() == "string") {
      QString str = parameter.defValue().toString();

      parseParameterStringEdit(parameter, plotData, str);

      if (! CQUtil::setProperty(plot_, parameter.propName(), QVariant(str)))
        charts()->errorMsg("Failed to set parameter " + parameter.propName());
    }
    else if (parameter.type() == "real") {
      double r = parameter.defValue().toDouble();

      parseParameterRealEdit(parameter, plotData, r);

      if (! CQUtil::setProperty(plot_, parameter.propName(), QVariant(r)))
        charts()->errorMsg("Failed to set parameter " + parameter.propName());
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

  double vr = CQChartsView::viewportRange();

  int n = view->numPlots();

  CBBox2D bbox(vr*xmin, vr*ymin, vr*xmax, vr*ymax);

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
                         int &column, QString &columnStr, QString &columnType)
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

  if (! lineEditValue((*pe).second, column, columnStr, columnType, defColumn))
    return false;

  return true;
}

bool
CQChartsPlotDlg::
parseParameterColumnsEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                          std::vector<int> &columns, QStringList &columnStrs, QString &columnType)
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

  int     column;
  QString columnStr;

  bool ok = lineEditValue((*pe).second, column, columnStr, columnType, -1);

  if (ok) {
    columns   .push_back(column);
    columnStrs.push_back(columnStr);

    return true;
  }

  return lineEditValues((*pe).second, columns, columnStrs, columnType);
}

bool
CQChartsPlotDlg::
parseParameterStringEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                         QString &str)
{
  auto p = plotData.stringEdits.find(parameter.name());
  assert(p != plotData.stringEdits.end());

  str = (*p).second->text();

  return true;
}

bool
CQChartsPlotDlg::
parseParameterRealEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                       double &r)
{
  auto p = plotData.realEdits.find(parameter.name());
  assert(p != plotData.realEdits.end());

  bool ok;

  r = (*p).second->text().toDouble(&ok);

  return true;
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
lineEditValue(QLineEdit *le, int &i, QString &columnStr, QString &columnType, int defi) const
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

  columnStr = str;

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
lineEditValues(QLineEdit *le, std::vector<int> &columns, QStringList &columnStrs,
               QString &columnType) const
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

    QStringList strs1 = lhs.split("-", QString::SkipEmptyParts);

    if (strs1.size() == 2) {
      bool ok1, ok2;

      int startCol = strs1[0].toInt(&ok1);
      int endCol   = strs1[1].toInt(&ok2);

      if (ok1 && ok2) {
        for (int col = startCol; col <= endCol; ++col) {
          columns   .push_back(col);
          columnStrs.push_back(QString("%1").arg(col));
        }

        if (rhs.length())
          columnType = rhs;
      }
      else
        ok = false;
    }
    else {
      bool ok1;

      int col = lhs.toInt(&ok1);

      if (ok1) {
        columns   .push_back(col);
        columnStrs.push_back(lhs);

        if (rhs.length())
          columnType = rhs;
      }
      else
        ok = false;
    }
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
