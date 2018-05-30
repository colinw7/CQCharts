#include <CQChartsPlotDlg.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotParameter.h>
#include <CQChartsAxis.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQDividedArea.h>
#include <CQRealSpin.h>
#include <CQUtil.h>

#include <QItemSelectionModel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QComboBox>
#include <QStackedWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <cassert>

#include <svg/refresh_svg.h>

CQChartsPlotDlg::
CQChartsPlotDlg(CQCharts *charts, const CQChartsModelP &model) :
 charts_(charts), model_(model)
{
  auto createSep = [](const QString &name) -> QFrame * {
    QFrame *sep = new QFrame;
    sep->setObjectName(name);

    sep->setFixedHeight(4);
    sep->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    return sep;
  };

  //---

  setObjectName("plotDlg");

  setWindowTitle("Create Plot");

  //----

  QVBoxLayout *layout = new QVBoxLayout(this);

  //----

  CQDividedArea *area = new CQDividedArea;
  area->setObjectName("area");

  layout->addWidget(area);

  //----

  QFrame *typeFrame = new QFrame;
  typeFrame->setObjectName("type");

  QVBoxLayout *typeLayout = new QVBoxLayout(typeFrame);

  area->addWidget(typeFrame, "Type Data");

  //--

  // type combo
  QHBoxLayout *typeComboLayout = new QHBoxLayout;

  typeLayout->addLayout(typeComboLayout);

  QLabel *typeLabel = new QLabel("Type");
  typeLabel->setObjectName("typeLabel");

  typeComboLayout->addWidget(typeLabel);

  QComboBox *typeCombo = new QComboBox;
  typeCombo->setObjectName("typeCombo");

  typeComboLayout->addWidget(typeCombo);

  typeCombo->setToolTip("Plot Type");

  QStringList names, descs;

  this->charts()->getPlotTypeNames(names, descs);

  typeCombo->addItems(descs);

  connect(typeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboSlot(int)));

  typeComboLayout->addStretch(1);

  //----

  stack_ = new QStackedWidget;
  stack_->setObjectName("stack");

  typeLayout->addWidget(stack_);

  for (int i = 0; i < names.size(); ++i)
    addPlotWidgets(names[i], i);

  //----

  QHBoxLayout *whereLayout = new QHBoxLayout;

  typeLayout->addLayout(whereLayout);

  QLabel *whereLabel = new QLabel("Where");
  whereLabel->setObjectName("whereLabel");

  whereLayout->addWidget(whereLabel);

  whereEdit_ = new QLineEdit;
  whereEdit_->setObjectName("whereEdit");

  whereLayout->addWidget(whereEdit_);

  whereLayout->addStretch(1);

  whereEdit_->setToolTip("Filter for input data");

  //----

  QFrame *genFrame = new QFrame;
  genFrame->setObjectName("general");

  QGridLayout *genLayout = new QGridLayout(genFrame);

  area->addWidget(genFrame, "General Data");

  //--

  int row = 0, column = 0;

  viewEdit_ = addLineEdit(genLayout, row, column, "View Name", "viewEdit", "View Name");

  viewEdit_->setToolTip("View to add plot to. If empty create new view");

  //--

  ++row; column = 0;

  posEdit_ = addLineEdit(genLayout, row, column, "Plot Position", "position", "Position");

  posEdit_->setText("0 0 1 1");

  posEdit_->setToolTip("Position and size of plot in view (values in range 0->1)");

  //--

  ++row; column = 0;

  titleEdit_ = addLineEdit(genLayout, row, column, "Plot Title", "title", "Title");

  titleEdit_->setToolTip("Plot Title");

  //--

  ++row; column = 0;

  xminEdit_ = addLineEdit(genLayout, row, column, "XMin", "xmin", "X Axis Minimum Value");
  yminEdit_ = addLineEdit(genLayout, row, column, "YMin", "ymin", "Y Axis Minimum Value");

  xminEdit_->setToolTip("Custom X Axis Minimum Value");
  yminEdit_->setToolTip("Custom Y Axis Minimum Value");

  ++row; column = 0;

  xmaxEdit_ = addLineEdit(genLayout, row, column, "XMax", "xmax", "X Axis Maximum Value");
  ymaxEdit_ = addLineEdit(genLayout, row, column, "YMax", "ymax", "Y Axis Maximum Value");

  xmaxEdit_->setToolTip("Custom X Axis Maximum Value");
  ymaxEdit_->setToolTip("Custom Y Axis Maximum Value");

  //--

  ++row; column = 0;

  xintegralCheck_ = new QCheckBox("X Integral");
  xintegralCheck_->setObjectName("xintegralCheck");

  yintegralCheck_ = new QCheckBox("Y Integral");
  yintegralCheck_->setObjectName("yintegralCheck");

  genLayout->addWidget(xintegralCheck_, row, column); ++column;
  genLayout->addWidget(yintegralCheck_, row, column);

  xintegralCheck_->setToolTip("X values are Integral");
  yintegralCheck_->setToolTip("Y values are Integral");

  //----

  ++row; column = 0;

  xlogCheck_ = new QCheckBox("X Log");
  xlogCheck_->setObjectName("xlogCheck");

  ylogCheck_ = new QCheckBox("Y Log");
  ylogCheck_->setObjectName("ylogCheck");

  genLayout->addWidget(xlogCheck_, row, column); ++column;
  genLayout->addWidget(ylogCheck_, row, column);

  xlogCheck_->setToolTip("Use log scale for X Axis");
  ylogCheck_->setToolTip("Use log scale for Y Axis");

  //--

  ++row;

  genLayout->setRowStretch(row, 1);

  //----

  QFrame *previewFrame = new QFrame;
  previewFrame->setObjectName("preview");

  QVBoxLayout *previewLayout = new QVBoxLayout(previewFrame);

  area->addWidget(previewFrame, "Preview");

  //--

  previewView_ = charts_->createView();

  previewView_->setPreview(true);

  previewLayout->addWidget(previewView_);

  //----

  QFrame *sep1 = createSep("sep1");

  layout->addWidget(sep1);

  //----

  msgLabel_ = new QLabel;
  msgLabel_->setObjectName("msgLabel");

  layout->addWidget(msgLabel_);

  //----

  QFrame *sep2 = createSep("sep2");

  layout->addWidget(sep2);

  //-------

  // OK, Apply, Cancel Buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout;

  layout->addLayout(buttonLayout);

  //--

  okButton_ = new QPushButton("OK");
  okButton_->setObjectName("ok");

  connect(okButton_, SIGNAL(clicked()), this, SLOT(okSlot()));

  buttonLayout->addWidget(okButton_);

  //--

  applyButton_ = new QPushButton("Apply");
  applyButton_->setObjectName("apply");

  connect(applyButton_, SIGNAL(clicked()), this, SLOT(applySlot()));

  buttonLayout->addWidget(applyButton_);

  //--

  QPushButton *cancelButton = new QPushButton("Cancel");
  cancelButton->setObjectName("cancel");

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  buttonLayout->addWidget(cancelButton);

  //--

  buttonLayout->addStretch(1);

  //--

  initialized_ = true;

  validateSlot();
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

  connect(columnEdit, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  QString tip = parameter.tip();

  if (tip.length())
    columnEdit->setToolTip(tip);

  //----

  FormatEditData formatEditData;

  formatEditData.formatEdit =
    addLineEdit(layout, row, column, "", parameter.name() + "Format", "Column Format");

  connect(formatEditData.formatEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(validateSlot()));

  //--

  formatEditData.formatUpdate = new QToolButton;
  formatEditData.formatUpdate->setObjectName("formatUpdate");

  formatEditData.formatUpdate->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

  connect(formatEditData.formatUpdate, SIGNAL(clicked()),
          this, SLOT(updateFormatSlot()));

  layout->addWidget(formatEditData.formatUpdate, row, column); ++column;

  plotData.formatEdits[parameter.name()] = formatEditData;

  //----

  QLabel *attributesLabel = new QLabel;
  attributesLabel->setObjectName("attributesLabel");

  attributesLabel->setText(parameter.attributes().summary());

  layout->addWidget(attributesLabel, row, column); ++column;

  //----

  ++row;

  //----

  if (parameter.attributes().isMapped()) {
    MapEditData mapEditData;

    column = 1;

    QHBoxLayout *mapLayout = new QHBoxLayout;

    mapEditData.mappedCheck = new QCheckBox("Mapped");
    mapEditData.mappedCheck->setObjectName("mapped");

    connect(mapEditData.mappedCheck, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));

    mapEditData.mappedCheck->setToolTip("Remap column values from actual range to specific range");

    //--

    mapEditData.mapMinSpin = new CQRealSpin;
    mapEditData.mapMinSpin->setObjectName("mapMin");

    mapEditData.mapMaxSpin = new CQRealSpin;
    mapEditData.mapMaxSpin->setObjectName("mapMax");

    connect(mapEditData.mapMinSpin, SIGNAL(valueChanged(double)), this, SLOT(validateSlot()));
    connect(mapEditData.mapMaxSpin, SIGNAL(valueChanged(double)), this, SLOT(validateSlot()));

    mapEditData.mapMinSpin->setToolTip("Min value for mapped values");
    mapEditData.mapMaxSpin->setToolTip("Max value for mapped values");

    //--

    mapLayout->addWidget(mapEditData.mappedCheck);
    mapLayout->addWidget(mapEditData.mapMinSpin);
    mapLayout->addWidget(mapEditData.mapMaxSpin);
    mapLayout->addStretch(1);

    plotData.mappedEdits[parameter.name()] = mapEditData;

    layout->addLayout(mapLayout, row, column, 1, 2);

    ++row;

    //---

    mapEditData.mappedCheck->setChecked(false);

    mapEditData.mapMinSpin ->setValue(parameter.attributes().mapMin());
    mapEditData.mapMaxSpin ->setValue(parameter.attributes().mapMax());
  }
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

  bool ok;

  QString str = CQChartsUtil::toString(parameter.defValue(), ok);

  columnsEdit->setText(str);

  plotData.columnsEdits[parameter.name()] = columnsEdit;

  connect(columnsEdit, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  QString tip = parameter.tip();

  if (tip.length())
    columnsEdit->setToolTip(tip);

  //----

  FormatEditData formatEditData;

  formatEditData.formatEdit =
    addLineEdit(layout, row, column, "", parameter.name() + "Format", "Columns Format");

  connect(formatEditData.formatEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(validateSlot()));

  //--

  formatEditData.formatUpdate = new QToolButton;
  formatEditData.formatUpdate->setObjectName("formatUpdate");

  formatEditData.formatUpdate->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

  connect(formatEditData.formatUpdate, SIGNAL(clicked()),
          this, SLOT(updateFormatSlot()));

  layout->addWidget(formatEditData.formatUpdate, row, column); ++column;

  plotData.formatEdits[parameter.name()] = formatEditData;

  //----

  ++row;
}

void
CQChartsPlotDlg::
addParameterStringEdit(PlotData &plotData, QHBoxLayout *layout,
                       const CQChartsPlotParameter &parameter)
{
  bool ok;

  QString str = CQChartsUtil::toString(parameter.defValue(), ok);

  QHBoxLayout *editLayout = new QHBoxLayout;

  QLabel *label = new QLabel(parameter.desc());
  label->setObjectName(parameter.name() + "_label");

  QLineEdit *edit  = new QLineEdit;
  edit->setObjectName(parameter.name() + "_edit");

  edit->setText(str);

  editLayout->addWidget(label);
  editLayout->addWidget(edit);

  layout->addLayout(editLayout);

  plotData.stringEdits[parameter.name()] = edit;

  connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  QString tip = parameter.tip();

  if (tip.length())
    edit->setToolTip(tip);
}

void
CQChartsPlotDlg::
addParameterRealEdit(PlotData &plotData, QHBoxLayout *layout,
                     const CQChartsPlotParameter &parameter)
{
  double r = parameter.defValue().toDouble();

  QHBoxLayout *editLayout = new QHBoxLayout;

  QLabel *label = new QLabel(parameter.desc());
  label->setObjectName(parameter.name() + "_label");

  QLineEdit *edit  = new QLineEdit;
  edit->setObjectName(parameter.name() + "_edit");

  edit->setText(QString("%1").arg(r));

  editLayout->addWidget(label);
  editLayout->addWidget(edit);

  layout->addLayout(editLayout);

  plotData.realEdits[parameter.name()] = edit;

  connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  QString tip = parameter.tip();

  if (tip.length())
    edit->setToolTip(tip);
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

  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));

  QString tip = parameter.tip();

  if (tip.length())
    checkBox->setToolTip(tip);
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

  grid->addWidget(edit, row, column); ++column;

  return edit;
}

void
CQChartsPlotDlg::
comboSlot(int ind)
{
  stack_->setCurrentIndex(ind);

  validateSlot();
}

void
CQChartsPlotDlg::
validateSlot()
{
  if (! initialized_)
    return;

  QString msg;

  bool ok = validate(msg);

  okButton_   ->setEnabled(ok);
  applyButton_->setEnabled(ok);

  if (! ok) {
    msgLabel_->setText(msg);
    msgLabel_->setFixedHeight(msgLabel_->sizeHint().height());
    return;
  }

  msgLabel_->setText(" ");
  msgLabel_->setFixedHeight(msgLabel_->sizeHint().height());

  // create plot for typename of current tab
  int ind = stack_->currentIndex();

  QString typeName = tabTypeName_[ind];

  CQChartsPlotType *type = charts()->plotType(typeName);
  assert(type);

  if (! previewPlot_ || previewPlot_->type() != type) {
    previewView_->removeAllPlots();

    previewPlot_ = type->create(previewView_, model_);

    previewPlot_->setPreview(true);

    double vr = CQChartsView::viewportRange();

    CQChartsGeom::BBox bbox(0, 0, vr, vr);

    previewPlot_->setId("Preview");

    previewView_->addPlot(previewPlot_, bbox);
  }

  applyPlot(previewPlot_, /*preview*/true);
}

void
CQChartsPlotDlg::
updateFormatSlot()
{
  QToolButton *formatUpdate = qobject_cast<QToolButton *>(sender());

  if (! formatUpdate)
    return;

  int ind = stack_->currentIndex();

  QString typeName = tabTypeName_[ind];

  PlotData &plotData = typePlotData_[typeName];

  QString    parameterName;
  QLineEdit *formatEdit { nullptr };

  for (const auto &fe : plotData.formatEdits) {
    const FormatEditData &formatEditData = fe.second;

    if (formatUpdate == formatEditData.formatUpdate) {
      parameterName = fe.first;
      formatEdit    = formatEditData.formatEdit;
      break;
    }
  }

  if (! parameterName.length())
    return;

  int     column;
  QString columnStr;
  QString columnType;
  int     defColumn = -1;

  auto pce = plotData.columnEdits.find(parameterName);

  if (pce != plotData.columnEdits.end()) {
    if (! lineEditValue((*pce).second, column, columnStr, columnType, defColumn))
      return;
  }
  else {
    auto pce = plotData.columnsEdits.find(parameterName);

    if (pce != plotData.columnEdits.end()) {
      if (! lineEditValue((*pce).second, column, columnStr, columnType, defColumn))
        return;
    }
  }

  if (column < 0)
    return;

  CQChartsModelData *modelData = charts_->getModelData(model_.data());
  if (! modelData) return;

  CQChartsModelDetails &details = modelData->details();

  const CQChartsModelColumnDetails &columnDetails = details.columnDetails(column);

  formatEdit->setText(columnDetails.typeName());
}

bool
CQChartsPlotDlg::
validate(QString &msg)
{
  msg = "";

  CQChartsModelData *modelData = charts_->getModelData(model_.data());
  if (! modelData) { msg = "no model data"; return false; }

  const CQChartsModelDetails &details = modelData->details();

  //---

  // create plot for typename of current tab
  int ind = stack_->currentIndex();

  QString typeName = tabTypeName_[ind];

  if (! charts()->isPlotType(typeName)) {
    msg = "invalid plot type";
    return false;
  }

  CQChartsPlotType *type = charts()->plotType(typeName);
  assert(type);

  // set plot property for widgets for plot parameters
  PlotData &plotData = typePlotData_[type->name()];

  bool rc = true;

  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column") {
      bool ok;

      int column = parameter.defValue().toInt(&ok);

      if (! ok)
        column = -1;

      QString      columnStr;
      QString      columnTypeStr;
      MapValueData mapValueData;

      if (! parseParameterColumnEdit(parameter, plotData, column, columnStr,
                                     columnTypeStr, mapValueData)) {
        if (parameter.attributes().isRequired()) {
          msg += "missing required column value\n";
          rc = false;
        }

        continue;
      }

      if (column < 0 || column >= details.numColumns()) {
        msg += "invalid column number\n";
        rc = false;
        continue;
      }

      const CQChartsModelColumnDetails &columnDetails = details.columnDetails(column);

      if (parameter.attributes().isMonotonic()) {
        if (! columnDetails.isMonotonic())
          msg += "non-monotonic column\n";
      }

      if      (parameter.attributes().isNumeric()) {
        if (columnDetails.type() != CQBaseModel::Type::INTEGER &&
            columnDetails.type() != CQBaseModel::Type::REAL &&
            columnDetails.type() != CQBaseModel::Type::TIME)
          msg += "non-numeric column\n";
      }
      else if (parameter.attributes().isString()) {
        if (columnDetails.type() != CQBaseModel::Type::STRING)
          msg += "non-string column\n";
      }
      else if (parameter.attributes().isColor()) {
        if (columnDetails.type() != CQBaseModel::Type::COLOR)
          msg += "non-color column\n";
      }
    }
  }

  return rc;
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

  // get or create view
  QString viewId = viewEdit_->text();

  CQChartsView *view = charts()->getView(viewId);

  if (! view) {
    view = charts()->addView();

    CQChartsWindow *window = new CQChartsWindow(view);

    window->show();

    viewEdit_->setText(view->id());
  }

  // create plot
  CQChartsPlotType *type = charts()->plotType(typeName);
  assert(type);

  plot_ = type->create(view, model_);

  if (selectionModel())
    plot_->setSelectionModel(selectionModel());

  //---

  applyPlot(plot_);

  //---

  double vr = CQChartsView::viewportRange();

  int n = view->numPlots();

  double xmin = 0.0, ymin = 0.0, xmax = 1.0, ymax = 1.0;

  parsePosition(xmin, ymin, xmax, ymax);

  CQChartsGeom::BBox bbox(vr*xmin, vr*ymin, vr*xmax, vr*ymax);

  plot_->setId(QString("Chart.%1").arg(n + 1));

  view->addPlot(plot_, bbox);

  //---

  emit plotCreated(plot_);

  return true;
}

void
CQChartsPlotDlg::
applyPlot(CQChartsPlot *plot, bool preview)
{
  CQChartsPlotType *type = plot->type();

  // set plot property for widgets for plot parameters
  PlotData &plotData = typePlotData_[type->name()];

  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column") {
      bool ok;

      int defValue = parameter.defValue().toInt(&ok);

      if (! ok)
        defValue = -1;

      int column = defValue;

      QString      columnStr;
      QString      columnTypeStr;
      MapValueData mapValueData;

      if (parseParameterColumnEdit(parameter, plotData, column, columnStr,
                                   columnTypeStr, mapValueData)) {
        if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(column)))
          charts()->errorMsg("Failed to set parameter '" + parameter.propName() + "'");

        if (columnTypeStr.length())
          CQChartsUtil::setColumnTypeStr(charts_, model(), column, columnTypeStr);

        if (parameter.attributes().isMapped()) {
          QString mappedPropName, mapMinPropName, mapMaxPropName;

          if (parameter.mapPropNames(mappedPropName, mapMinPropName, mapMaxPropName)) {
            if (! CQUtil::setProperty(plot, mappedPropName, QVariant(mapValueData.mapped)))
              charts()->errorMsg("Failed to set parameter '" + mappedPropName + "'");

            if (! CQUtil::setProperty(plot, mapMinPropName, QVariant(mapValueData.min)))
              charts()->errorMsg("Failed to set parameter '" + mapMinPropName + "'");

            if (! CQUtil::setProperty(plot, mapMaxPropName, QVariant(mapValueData.max)))
              charts()->errorMsg("Failed to set parameter '" + mapMaxPropName + "'");
          }
          else {
            charts()->errorMsg("Invalid column parameter name '" + parameter.propName() + "'");
          }
        }
      }
      else {
        if (preview)
          CQUtil::setProperty(plot, parameter.propName(), QVariant(defValue));
      }
    }
    else if (parameter.type() == "columns") {
      bool ok;

      QString defValue = CQChartsUtil::toString(parameter.defValue(), ok);

      std::vector<int> columns;

      (void) CQChartsUtil::fromString(defValue, columns);

      QStringList columnStrs;
      QString     columnTypeStr;

      if (parseParameterColumnsEdit(parameter, plotData, columns, columnStrs, columnTypeStr)) {
        QString s = CQChartsUtil::toString(columns);

        if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(s)))
          charts()->errorMsg("Failed to set parameter '" + parameter.propName() + "'");

        if (columnTypeStr.length() && ! columns.empty())
          CQChartsUtil::setColumnTypeStr(charts_, model(), columns[0], columnTypeStr);
      }
      else {
        if (preview)
          CQUtil::setProperty(plot, parameter.propName(), defValue);
      }
    }
    else if (parameter.type() == "string") {
      bool ok;

      QString defStr = CQChartsUtil::toString(parameter.defValue(), ok);

      QString str = defStr;

      if (parseParameterStringEdit(parameter, plotData, str)) {
        if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(str)))
          charts()->errorMsg("Failed to set parameter '" + parameter.propName() + "'");
      }
      else {
        if (preview)
          CQUtil::setProperty(plot, parameter.propName(), QVariant(defStr));
      }
    }
    else if (parameter.type() == "real") {
      double defValue = parameter.defValue().toDouble();

      double r = defValue;

      if (parseParameterRealEdit(parameter, plotData, r)) {
        if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(r)))
          charts()->errorMsg("Failed to set parameter '" + parameter.propName() + "'");
      }
      else {
        if (preview)
          CQUtil::setProperty(plot, parameter.propName(), QVariant(defValue));
      }
    }
    else if (parameter.type() == "bool") {
      bool defValue = parameter.defValue().toBool();

      bool b = defValue;

      if (parseParameterBoolEdit(parameter, plotData, b)) {
        if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(b)))
          charts()->errorMsg("Failed to set parameter '" + parameter.propName() + "'");
      }
      else {
        if (preview)
          CQUtil::setProperty(plot, parameter.propName(), QVariant(defValue));
      }
    }
    else
      assert(false);
  }

  //---

  if (titleEdit_->text().length())
    plot->setTitleStr(titleEdit_->text());

  if (plot->xAxis())
    plot->xAxis()->setIntegral(xintegralCheck_->isChecked());

  if (plot->yAxis())
    plot->yAxis()->setIntegral(yintegralCheck_->isChecked());

  plot->setLogX(xlogCheck_->isChecked());
  plot->setLogY(ylogCheck_->isChecked());

  //---

  bool xminOk = false, yminOk = false, xmaxOk = false, ymaxOk = false;

  if (xminEdit_->text().length()) {
    double xmin = xminEdit_->text().toDouble(&xminOk);
    if (xminOk) plot->setXMin(xmin);
  }

  if (yminEdit_->text().length()) {
    double ymin = yminEdit_->text().toDouble(&yminOk);
    if (yminOk) plot->setYMin(ymin);
  }

  if (xmaxEdit_->text().length()) {
    double xmax = xmaxEdit_->text().toDouble(&xmaxOk);
    if (xmaxOk) plot->setXMax(xmax);
  }

  if (ymaxEdit_->text().length()) {
    double ymax = ymaxEdit_->text().toDouble(&ymaxOk);
    if (ymaxOk) plot->setYMax(ymax);
  }

  if (preview) {
    using OptReal = CQChartsPlot::OptReal;

    if (! xminOk) plot->setXMin(OptReal());
    if (! yminOk) plot->setYMin(OptReal());
    if (! xmaxOk) plot->setXMax(OptReal());
    if (! ymaxOk) plot->setYMax(OptReal());
  }
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
    bool ok3; xmax = posStrs[2].toDouble(&ok3); if (! ok3) xmax = 1.0;
    bool ok4; ymax = posStrs[3].toDouble(&ok4); if (! ok4) ymax = 1.0;

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
                         int &column, QString &columnStr, QString &columnType,
                         MapValueData &mapValueData)
{
  auto pf = plotData.formatEdits.find(parameter.name());
  assert(pf != plotData.formatEdits.end());

  const FormatEditData &formatEditData = (*pf).second;

  QString format = formatEditData.formatEdit->text();

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

  auto pm = plotData.mappedEdits.find(parameter.name());

  if (pm != plotData.mappedEdits.end()) {
    const MapEditData &mapEditData = (*pm).second;

    mapValueData.mapped = mapEditData.mappedCheck->isChecked();
    mapValueData.min    = mapEditData.mapMinSpin ->value();
    mapValueData.max    = mapEditData.mapMaxSpin ->value();
  }
  else {
    mapValueData.mapped = false;
  }

  return true;
}

bool
CQChartsPlotDlg::
parseParameterColumnsEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                          std::vector<int> &columns, QStringList &columnStrs, QString &columnType)
{
  auto pf = plotData.formatEdits.find(parameter.name());
  assert(pf != plotData.formatEdits.end());

  const FormatEditData &formatEditData = (*pf).second;

  QString format = formatEditData.formatEdit->text();

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

    // support column numeric range <n>-<m>
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
      int col;

      if (stringToColumn(lhs, col)) {
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

  int nc = model()->columnCount();

  for (int column1 = 0; column1 < nc; ++column1) {
    QVariant var = model()->headerData(column1, Qt::Horizontal, Qt::DisplayRole);

    if (! var.isValid())
      continue;

    bool ok;

    QString str1 = CQChartsUtil::toString(var, ok);

    if (str1 == str) {
      column = column1;
      return true;
    }
  }

  return false;
}

int
CQChartsPlotDlg::
exec()
{
  plot_ = nullptr;

  return QDialog::exec();
}
