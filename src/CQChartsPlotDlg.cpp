#include <CQChartsPlotDlg.h>
#include <CQChartsPlotType.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotParameter.h>
#include <CQChartsAxis.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQChartsColumnEdit.h>
#include <CQChartsModelView.h>
#include <CQDividedArea.h>
#include <CQIntegerSpin.h>
#include <CQRealSpin.h>
#include <CQUtil.h>
#include <CQSummaryModel.h>

#include <QItemSelectionModel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QComboBox>
#include <QStackedWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <cassert>

#include <svg/refresh_svg.h>

CQChartsPlotDlg::
CQChartsPlotDlg(CQCharts *charts, const ModelP &model) :
 charts_(charts), model_(model)
{
  init();
}

void
CQChartsPlotDlg::
init()
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
  //setWindowIcon(QIcon()); TODO

  modelData_ = charts_->getModelData(model_.data());

  if (modelData_)
    setWindowTitle(QString("Create Plot (Model %1)").arg(modelData_->ind()));

  //----

  if (! CQChartsUtil::isHierarchical(model_.data())) {
    summaryModel_ = new CQSummaryModel(model_.data());

    summaryModelP_ = ModelP(summaryModel_);

    summaryModelData_ = new CQChartsModelData(charts_, summaryModelP_);
  }

  //----

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //----

  CQDividedArea *area = new CQDividedArea;
  area->setObjectName("area");

  layout->addWidget(area);

  //----

  QFrame *typeFrame = createTypeDataFrame();

  area->addWidget(typeFrame, "Type Data");

  QFrame *genFrame = createGeneralDataFrame();

  area->addWidget(genFrame, "General Data");

  QFrame *previewFrame = createPreviewFrame();

  area->addWidget(previewFrame, "Preview");

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
  buttonLayout->setMargin(2); buttonLayout->setSpacing(2);

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

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));

  buttonLayout->addWidget(cancelButton);

  //--

  buttonLayout->addStretch(1);

  //--

  initialized_ = true;

  validateSlot();
}

QFrame *
CQChartsPlotDlg::
createTypeDataFrame()
{
  QFrame *typeFrame = new QFrame;
  typeFrame->setObjectName("type");

  QVBoxLayout *typeLayout = new QVBoxLayout(typeFrame);
  typeLayout->setMargin(2); typeLayout->setSpacing(2);

  //--

  CQCharts::PlotTypes plotTypes;

  this->charts()->getPlotTypes(plotTypes);

  // create ordered list of types (1D, 2D, other, hierarchical)

  using DimPlotTypesMap     = std::map<int,CQCharts::PlotTypes>;
  using HierDimPlotsTypeMap = std::map<bool,DimPlotTypesMap>;

  HierDimPlotsTypeMap hierDimPlotsTypeMap;

  for (auto &plotType : plotTypes) {
    CQChartsPlotType::Dimension dim = plotType->dimension();

    int dim1 = 999;

    if (dim != CQChartsPlotType::Dimension::NONE)
      dim1 = int(dim);

    hierDimPlotsTypeMap[plotType->isHierarchical()][dim1].push_back(plotType);
  }

  CQCharts::PlotTypes plotTypes1;

  for (auto &p1 : hierDimPlotsTypeMap) {
    const DimPlotTypesMap &dimPlotTypesMap = p1.second;

    for (auto &p2 : dimPlotTypesMap) {
      const CQCharts::PlotTypes &plotTypes = p2.second;

      for (auto &plotType : plotTypes)
        plotTypes1.push_back(plotType);
    }
  }

  //---

  // type combo
  QHBoxLayout *typeComboLayout = new QHBoxLayout;
  typeComboLayout->setMargin(0); typeComboLayout->setSpacing(2);

  typeLayout->addLayout(typeComboLayout);

  QLabel *typeLabel = new QLabel("Type");
  typeLabel->setObjectName("typeLabel");

  typeComboLayout->addWidget(typeLabel);

  QComboBox *typeCombo = new QComboBox;
  typeCombo->setObjectName("typeCombo");

  typeComboLayout->addWidget(typeCombo);

  typeCombo->setToolTip("Plot Type");

  QStringList items;

  for (auto &plotType : plotTypes1)
    items << plotType->desc();

  typeCombo->addItems(items);

  connect(typeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboSlot(int)));

  typeComboLayout->addStretch(1);

  //----

  QTabWidget *typeTab = new QTabWidget;
  typeTab->setObjectName("typeTab");

  typeLayout->addWidget(typeTab);

  // plot type widgets in stack (one per type)
  stack_ = new QStackedWidget;
  stack_->setObjectName("stack");

  for (std::size_t i = 0; i < plotTypes1.size(); ++i)
    addPlotWidgets(plotTypes1[i], i);

  typeTab->addTab(stack_, "Input Data");

  //---

  // plot type description
  descText_ = new QTextEdit;
  descText_->setObjectName("descText");

  typeTab->addTab(descText_, "Description");

  //----

  QHBoxLayout *whereLayout = new QHBoxLayout;
  whereLayout->setMargin(0); whereLayout->setSpacing(2);

  typeLayout->addLayout(whereLayout);

  QLabel *whereLabel = new QLabel("Where");
  whereLabel->setObjectName("whereLabel");

  whereLayout->addWidget(whereLabel);

  whereEdit_ = new QLineEdit;
  whereEdit_->setObjectName("whereEdit");

  whereLayout->addWidget(whereEdit_);

  whereLayout->addStretch(1);

  whereEdit_->setToolTip("Filter for input data");

  //---

  return typeFrame;
}

QFrame *
CQChartsPlotDlg::
createGeneralDataFrame()
{
  QFrame *genFrame = new QFrame;
  genFrame->setObjectName("general");

  QGridLayout *genLayout = new QGridLayout(genFrame);
  genLayout->setMargin(2); genLayout->setSpacing(2);

  //--

  int row = 0, column = 0;

  viewEdit_ = addStringEdit(genLayout, row, column, "View Name", "viewEdit", "View Name");

  viewEdit_->setToolTip("View to add plot to. If empty create new view");

  //--

  ++row; column = 0;

  posEdit_ = addStringEdit(genLayout, row, column, "Plot Position", "position", "Position");

  posEdit_->setText("0 0 1 1");

  posEdit_->setToolTip("Position and size of plot in view (values in range 0->1)");

  //----

  ++row; column = 0;

  titleEdit_ = addStringEdit(genLayout, row, column, "Plot Title", "title", "Title");

  titleEdit_->setToolTip("Plot Title");

  connect(titleEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  //----

  ++row; column = 0;

  QFrame *xRangeFrame = new QFrame;
  xRangeFrame->setObjectName("xRangeFrame");

  QHBoxLayout *xRangeFrameLayout = new QHBoxLayout(xRangeFrame);
  xRangeFrameLayout->setMargin(0); xRangeFrameLayout->setSpacing(2);

  genLayout->addWidget(xRangeFrame, row, column, 1, 5);

  //--

  xminEdit_ = addRealEdit(xRangeFrameLayout, row, column, "X Min", "xmin", "X Axis Minimum Value");

  xminButton_ = new QToolButton;
  xminButton_->setObjectName("xminButton");

  xminButton_->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

  xminButton_->setToolTip("Get minimum value for x column");

  xRangeFrameLayout->addWidget(xminButton_);

  yminEdit_ = addRealEdit(xRangeFrameLayout, row, column, "Y Min", "ymin", "Y Axis Minimum Value");

  yminButton_ = new QToolButton;
  yminButton_->setObjectName("yminButton");

  yminButton_->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

  yminButton_->setToolTip("Get minimum value for y column");

  xRangeFrameLayout->addWidget(yminButton_);

  xminEdit_->setToolTip("Custom X Axis Minimum Value");
  yminEdit_->setToolTip("Custom Y Axis Minimum Value");

  connect(xminEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));
  connect(yminEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  connect(xminButton_, SIGNAL(clicked()), this, SLOT(xminSlot()));
  connect(yminButton_, SIGNAL(clicked()), this, SLOT(yminSlot()));

  //--

  ++row; column = 0;

  QFrame *yRangeFrame = new QFrame;
  yRangeFrame->setObjectName("yRangeFrame");

  QHBoxLayout *yRangeFrameLayout = new QHBoxLayout(yRangeFrame);
  yRangeFrameLayout->setMargin(0); yRangeFrameLayout->setSpacing(2);

  genLayout->addWidget(yRangeFrame, row, column, 1, 5);

  //--

  xmaxEdit_ = addRealEdit(yRangeFrameLayout, row, column, "X Max", "xmax", "X Axis Maximum Value");

  xmaxButton_ = new QToolButton;
  xmaxButton_->setObjectName("xmaxButton");

  xmaxButton_->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

  xmaxButton_->setToolTip("Get maximum value for x column");

  yRangeFrameLayout->addWidget(xmaxButton_);

  ymaxEdit_ = addRealEdit(yRangeFrameLayout, row, column, "Y Max", "ymax", "Y Axis Maximum Value");

  ymaxButton_ = new QToolButton;
  ymaxButton_->setObjectName("ymaxButton");

  ymaxButton_->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

  ymaxButton_->setToolTip("Get maximum value for y column");

  yRangeFrameLayout->addWidget(ymaxButton_);

  xmaxEdit_->setToolTip("Custom X Axis Maximum Value");
  ymaxEdit_->setToolTip("Custom Y Axis Maximum Value");

  connect(xmaxEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));
  connect(ymaxEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  connect(xmaxButton_, SIGNAL(clicked()), this, SLOT(xmaxSlot()));
  connect(ymaxButton_, SIGNAL(clicked()), this, SLOT(ymaxSlot()));

  //----

  ++row; column = 0;

  QFrame *xyFrame = new QFrame;
  xyFrame->setObjectName("xyFrame");

  QHBoxLayout *xyFrameLayout = new QHBoxLayout(xyFrame);
  xyFrameLayout->setMargin(0); xyFrameLayout->setSpacing(2);

  genLayout->addWidget(xyFrame, row, column, 1, 5);

  xintegralCheck_ = new QCheckBox("X Integral");
  xintegralCheck_->setObjectName("xintegralCheck");

  yintegralCheck_ = new QCheckBox("Y Integral");
  yintegralCheck_->setObjectName("yintegralCheck");

  xintegralCheck_->setToolTip("X values are Integral");
  yintegralCheck_->setToolTip("Y values are Integral");

  connect(xintegralCheck_, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));
  connect(yintegralCheck_, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));

  xyFrameLayout->addWidget(xintegralCheck_);
  xyFrameLayout->addWidget(yintegralCheck_);

  //--

  QFrame *xySpacer = new QFrame;
  xySpacer->setObjectName("xySpacer");

  xySpacer->setFixedWidth(8);

  xyFrameLayout->addWidget(xySpacer);

  //--

  xlogCheck_ = new QCheckBox("X Log");
  xlogCheck_->setObjectName("xlogCheck");

  ylogCheck_ = new QCheckBox("Y Log");
  ylogCheck_->setObjectName("ylogCheck");

  xlogCheck_->setToolTip("Use log scale for X Axis");
  ylogCheck_->setToolTip("Use log scale for Y Axis");

  connect(xlogCheck_, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));
  connect(ylogCheck_, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));

  xyFrameLayout->addWidget(xlogCheck_);
  xyFrameLayout->addWidget(ylogCheck_);

  //--

  xyFrameLayout->addStretch(1);

  //----

  ++row;

  genLayout->setRowStretch(row, 1);

  //---

  return genFrame;
}

QFrame *
CQChartsPlotDlg::
createPreviewFrame()
{
  QFrame *previewFrame = new QFrame;
  previewFrame->setObjectName("preview");

  QVBoxLayout *previewLayout = new QVBoxLayout(previewFrame);
  previewLayout->setMargin(0); previewLayout->setSpacing(2);

  //--

  int nr = model_.data()->rowCount();
  int nc = model_.data()->columnCount();

  QFrame *previewControl = new QFrame;
  previewControl->setObjectName("previewControl");

  QHBoxLayout *previewControlLayout = new QHBoxLayout(previewControl);
  previewControlLayout->setMargin(2); previewControlLayout->setSpacing(2);

  previewEnabledCheck_ = new QCheckBox("Enabled");
  previewEnabledCheck_->setObjectName("previewEnabled");

  connect(previewEnabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(previewEnabledSlot()));

  previewControlLayout->addWidget(previewEnabledCheck_);

  if (summaryModel_) {
    summaryEnabledCheck_ = new QCheckBox("Summary");
    summaryEnabledCheck_->setObjectName("summaryEnabled");

    summaryEnabledCheck_->setChecked(true);

    connect(summaryEnabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(previewEnabledSlot()));

    previewControlLayout->addWidget(summaryEnabledCheck_);

    //---

    previewMaxRows_ = new CQIntegerSpin;
    previewMaxRows_->setObjectName("previewMaxRows");

    previewMaxRows_->setRange(1, nr);
    previewMaxRows_->setToolTip(QString("Set Preview Row Count (1 -> %1)").arg(nr));

    if (summaryModel_)
      previewMaxRows_->setValue(summaryModel_->maxRows());

    connect(previewMaxRows_, SIGNAL(valueChanged(int)), this, SLOT(updatePreviewSlot()));

    previewControlLayout->addWidget(new QLabel("Max Rows"));
    previewControlLayout->addWidget(previewMaxRows_);

    //--

    previewNormalRadio_ = new QRadioButton("Normal");
    previewNormalRadio_->setObjectName("previewNormal");
    previewNormalRadio_->setChecked(true);

    previewRandomRadio_ = new QRadioButton("Random");
    previewRandomRadio_->setObjectName("previewRandom");

    previewSortedRadio_ = new QRadioButton("Sorted");
    previewSortedRadio_->setObjectName("previewSorted");

    previewPagedRadio_ = new QRadioButton("Paged");
    previewPagedRadio_->setObjectName("previewPaged");

    connect(previewNormalRadio_, SIGNAL(toggled(bool)), this, SLOT(updatePreviewSlot()));
    connect(previewRandomRadio_, SIGNAL(toggled(bool)), this, SLOT(updatePreviewSlot()));
    connect(previewSortedRadio_, SIGNAL(toggled(bool)), this, SLOT(updatePreviewSlot()));
    connect(previewPagedRadio_ , SIGNAL(toggled(bool)), this, SLOT(updatePreviewSlot()));

    previewControlLayout->addWidget(previewNormalRadio_);
    previewControlLayout->addWidget(previewRandomRadio_);
    previewControlLayout->addWidget(previewSortedRadio_);
    previewControlLayout->addWidget(previewPagedRadio_);

    //--

    previewSortedColEdit_ = new CQIntegerSpin;
    previewSortedColEdit_->setObjectName("previewSortedColEdit");

    previewSortedColEdit_->setRange(0, nc - 1);
    previewSortedColEdit_->setToolTip(QString("Set Preview Sort Column (0 -> %1)").arg(nc - 1));

    connect(previewSortedColEdit_, SIGNAL(valueChanged(int)), this, SLOT(updatePreviewSlot()));

    previewControlLayout->addWidget(new QLabel("Sort Column"));
    previewControlLayout->addWidget(previewSortedColEdit_);

    //---

    previewPageSizeEdit_ = new CQIntegerSpin;
    previewPageSizeEdit_->setObjectName("previewPageSizeEdit");

    previewPageSizeEdit_->setRange(1, nr);
    previewPageSizeEdit_->setValue(100);
    previewPageSizeEdit_->setToolTip(QString("Set Preview Page Size (1 -> %1)").arg(nr));

    connect(previewPageSizeEdit_, SIGNAL(valueChanged(int)), this, SLOT(updatePreviewSlot()));

    previewControlLayout->addWidget(new QLabel("Page Size"));
    previewControlLayout->addWidget(previewPageSizeEdit_);

    //--

    previewCurrentPageEdit_ = new CQIntegerSpin;
    previewCurrentPageEdit_->setObjectName("previewCurrentPageEdit");

    int np = (nr + previewPageSizeEdit_->value() - 1)/previewPageSizeEdit_->value();

    previewCurrentPageEdit_->setRange(0, np - 1);
    previewCurrentPageEdit_->setToolTip(QString("Set Preview Page Count (0 -> %1)").arg(np - 1));

    connect(previewCurrentPageEdit_, SIGNAL(valueChanged(int)), this, SLOT(updatePreviewSlot()));

    previewControlLayout->addWidget(new QLabel("Current Page"));
    previewControlLayout->addWidget(previewCurrentPageEdit_);
  }

  previewControlLayout->addStretch(1);

  previewLayout->addWidget(previewControl);

  //--

  QTabWidget *previewTab = new QTabWidget;
  previewTab->setObjectName("previewTab");

  previewLayout->addWidget(previewTab);

  //--

  previewModelView_ = new CQChartsModelView(charts_);

  previewTab->addTab(previewModelView_, "Data");

  if (summaryEnabledCheck_ && summaryEnabledCheck_->isChecked() && summaryModel_)
    previewModelView_->setModel(summaryModelP_, CQChartsUtil::isHierarchical(summaryModel_));
  else
    previewModelView_->setModel(model_, CQChartsUtil::isHierarchical(model_.data()));

  //--

  previewView_ = charts_->createView();

  previewView_->setPreview(true);

  previewTab->addTab(previewView_, "Chart");

  //----

  return previewFrame;
}

CQChartsPlotDlg::
~CQChartsPlotDlg()
{
  delete summaryModelData_;
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
addPlotWidgets(CQChartsPlotType *type, int ind)
{
  QFrame *frame = new QFrame;
  frame->setObjectName("frame");

  stack_->addWidget(frame);

  //---

  QGridLayout *frameLayout = new QGridLayout(frame);
  frameLayout->setMargin(2); frameLayout->setSpacing(2);

  int row = 0;

  PlotData &plotData = typePlotData_[type->name()];

  addParameterEdits(type, plotData, frameLayout, row);

  //---

  plotData.ind = ind;

  tabType_[plotData.ind] = type;
}

void
CQChartsPlotDlg::
addParameterEdits(CQChartsPlotType *type, PlotData &plotData, QGridLayout *layout, int &row)
{
  QTabWidget *parameterGroupTab = nullptr;

  for (const auto &p : type->parameterGroups()) {
    const CQChartsPlotParameterGroup &parameterGroup = p.second;

    CQChartsPlotType::Parameters parameters = type->groupParameters(parameterGroup.groupId());
    assert(! parameters.empty());

    if (! parameterGroupTab) {
      parameterGroupTab = new QTabWidget;
      parameterGroupTab->setObjectName("parameterGroupTab");

      layout->addWidget(parameterGroupTab, row, 0, 1, 5);

      ++row;
    }

    QFrame *parameterGroupFrame = new QFrame;
    parameterGroupFrame->setObjectName(parameterGroup.name());

    QGridLayout *parameterGroupLayout = new QGridLayout(parameterGroupFrame);

    parameterGroupTab->addTab(parameterGroupFrame, parameterGroup.name());

    int row1 = 0;

    addParameterEdits(parameters, plotData, parameterGroupLayout, row1);

    parameterGroupLayout->setRowStretch(row1, 1);
  }

  //---

  CQChartsPlotType::Parameters parameters = type->nonGroupParameters();

  addParameterEdits(parameters, plotData, layout, row);

  layout->setRowStretch(row, 1);
}

void
CQChartsPlotDlg::
addParameterEdits(const CQChartsPlotType::Parameters &parameters, PlotData &plotData,
                  QGridLayout *layout, int &row)
{
  // add column edits first
  int nbool = 0;
  int nstr  = 0;
  int nreal = 0;

  for (const auto &parameter : parameters) {
    if      (parameter.type() == "column" || parameter.type() == "columns")
      addParameterEdit(plotData, layout, row, parameter);
    else if (parameter.type() == "string")
      ++nstr;
    else if (parameter.type() == "real")
      ++nreal;
    else if (parameter.type() == "bool")
      ++nbool;
    else
      assert(false);
  }

  // add string and real edits
  if (nstr > 0 || nreal > 0) {
    QHBoxLayout *strLayout = new QHBoxLayout;
    strLayout->setMargin(0); strLayout->setSpacing(2);

    for (const auto &parameter : parameters) {
      if (parameter.type() == "string" || parameter.type() == "real")
        addParameterEdit(plotData, strLayout, parameter);
    }

    strLayout->addStretch(1);

    layout->addLayout(strLayout, row, 0, 1, 5);

    ++row;
  }

  // add bool edits
  if (nbool > 0) {
    QHBoxLayout *boolLayout = new QHBoxLayout;
    boolLayout->setMargin(0); boolLayout->setSpacing(2);

    for (const auto &parameter : parameters) {
      if (parameter.type() == "bool")
        addParameterEdit(plotData, boolLayout, parameter);
    }

    boolLayout->addStretch(1);

    layout->addLayout(boolLayout, row, 0, 1, 5);

    ++row;
  }
}

void
CQChartsPlotDlg::
addParameterEdit(PlotData &plotData, QGridLayout *layout, int &row,
                 const CQChartsPlotParameter &parameter)
{
  if      (parameter.type() == "column")
    addParameterColumnEdit(plotData, layout, row, parameter);
  else if (parameter.type() == "columns")
    addParameterColumnsEdit(plotData, layout, row, parameter);
  else
    assert(false);
}

void
CQChartsPlotDlg::
addParameterEdit(PlotData &plotData, QHBoxLayout *layout,
                 const CQChartsPlotParameter &parameter)
{
  if      (parameter.type() == "string")
    addParameterStringEdit(plotData, layout, parameter);
  else if (parameter.type() == "real")
    addParameterRealEdit(plotData, layout, parameter);
  else if (parameter.type() == "bool")
    addParameterBoolEdit(plotData, layout, parameter);
  else
    assert(false);
}

void
CQChartsPlotDlg::
addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                       const CQChartsPlotParameter &parameter)
{
  int column = 0;

  bool ok;

  int pColumn = parameter.defValue().toInt(&ok);

  CQChartsColumnEdit *columnEdit =
    addColumnEdit(layout, row, column, parameter.desc(), parameter.name() + "Column",
                  "Column Name or Number");

  columnEdit->setModel(model_.data());

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
    addStringEdit(layout, row, column, "", parameter.name() + "Format", "Column Format");

  connect(formatEditData.formatEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(validateSlot()));

  //--

  formatEditData.formatUpdate = new QToolButton;
  formatEditData.formatUpdate->setObjectName("formatUpdate");

  formatEditData.formatUpdate->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

  connect(formatEditData.formatUpdate, SIGNAL(clicked()),
          this, SLOT(updateFormatSlot()));

  formatEditData.formatUpdate->setToolTip("Get current column format");

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
    mapLayout->setMargin(0); mapLayout->setSpacing(2);

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

    mapEditData.mapMinSpin->setValue(parameter.attributes().mapMin());
    mapEditData.mapMaxSpin->setValue(parameter.attributes().mapMax());
  }
}

void
CQChartsPlotDlg::
addParameterColumnsEdit(PlotData &plotData, QGridLayout *layout, int &row,
                        const CQChartsPlotParameter &parameter)
{
  int column = 0;

  QLineEdit *columnsEdit =
    addStringEdit(layout, row, column, parameter.desc(), parameter.name() + "Columns",
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
    addStringEdit(layout, row, column, "", parameter.name() + "Format", "Columns Format");

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
}

void
CQChartsPlotDlg::
addParameterStringEdit(PlotData &plotData, QHBoxLayout *layout,
                       const CQChartsPlotParameter &parameter)
{
  bool ok;

  QString str = CQChartsUtil::toString(parameter.defValue(), ok);

  QHBoxLayout *editLayout = new QHBoxLayout;
  editLayout->setMargin(0); editLayout->setSpacing(2);

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
  editLayout->setMargin(0); editLayout->setSpacing(2);

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
addRealEdit(QLayout *layout, int &row, int &column, const QString &name,
            const QString &objName, const QString &placeholderText) const
{
  QLabel *label = nullptr;

  if (name != "") {
    label = new QLabel(name);
    label->setObjectName(objName + "Label");
  }

  QLineEdit *edit = new QLineEdit;
  edit->setObjectName(objName + "Edit" );

  edit->setPlaceholderText(placeholderText);

  QGridLayout *grid    = qobject_cast<QGridLayout *>(layout);
  QHBoxLayout *hlayout = qobject_cast<QHBoxLayout *>(layout);

  if      (grid) {
    if (label) {
      grid->addWidget(label, row, column); ++column;
    }

    grid->addWidget(edit, row, column); ++column;
  }
  else if (hlayout) {
    if (label)
      hlayout->addWidget(label);

    hlayout->addWidget(edit );
  }

  return edit;
}

QLineEdit *
CQChartsPlotDlg::
addStringEdit(QLayout *layout, int &row, int &column, const QString &name,
              const QString &objName, const QString &placeholderText) const
{
  QLabel *label = nullptr;

  if (name != "") {
    label = new QLabel(name);
    label->setObjectName(objName + "Label");
  }

  QLineEdit *edit = new QLineEdit;
  edit->setObjectName(objName + "Edit" );

  edit->setPlaceholderText(placeholderText);

  QGridLayout *grid    = qobject_cast<QGridLayout *>(layout);
  QHBoxLayout *hlayout = qobject_cast<QHBoxLayout *>(layout);

  if      (grid) {
    if (label) {
      grid->addWidget(label, row, column); ++column;
    }

    grid->addWidget(edit, row, column); ++column;
  }
  else if (hlayout) {
    if (label)
      hlayout->addWidget(label);

    hlayout->addWidget(edit );
  }

  return edit;
}

CQChartsColumnEdit *
CQChartsPlotDlg::
addColumnEdit(QLayout *layout, int &row, int &column, const QString &name,
              const QString &objName, const QString &placeholderText) const
{
  QLabel *label = nullptr;

  if (name != "") {
    label = new QLabel(name);
    label->setObjectName(objName + "Label");
  }

  CQChartsColumnEdit *edit = new CQChartsColumnEdit;
  edit->setObjectName(objName + "Edit" );

  edit->setPlaceholderText(placeholderText);

  QGridLayout *grid    = qobject_cast<QGridLayout *>(layout);
  QHBoxLayout *hlayout = qobject_cast<QHBoxLayout *>(layout);

  if      (grid) {
    if (label) {
      grid->addWidget(label, row, column); ++column;
    }

    grid->addWidget(edit, row, column); ++column;
  }
  else if (hlayout) {
    if (label)
      hlayout->addWidget(label);

    hlayout->addWidget(edit);
  }

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
xminSlot()
{
  setXYMin("xmin");
}

void
CQChartsPlotDlg::
yminSlot()
{
  setXYMin("ymin");
}

void
CQChartsPlotDlg::
xmaxSlot()
{
  setXYMin("xmax");
}

void
CQChartsPlotDlg::
ymaxSlot()
{
  setXYMin("ymax");
}

void
CQChartsPlotDlg::
setXYMin(const QString &id)
{
  if (! model_)
    return;

  CQChartsPlotType *type = getPlotType();
  if (! type) return;

  PlotData &plotData = typePlotData_[type->name()];

  CQChartsColumn column;

  const char *colName = (id == "xmin" || id == "xmax" ? type->xColumnName() : type->yColumnName());
  if (! colName) return;

  for (const auto &parameter : type->parameters()) {
    if (parameter.name() != colName)
      continue;

    if      (parameter.type() == "column") {
      bool ok;

      int icolumn = parameter.defValue().toInt(&ok);

      if (ok)
        column = icolumn;

      QString      columnStr;
      QString      columnTypeStr;
      MapValueData mapValueData;

      if (! parseParameterColumnEdit(parameter, plotData, column, columnStr,
                                     columnTypeStr, mapValueData))
        return;
    }
    else if (parameter.type() == "columns") {
      bool ok;

      QString defValue = CQChartsUtil::toString(parameter.defValue(), ok);

      std::vector<CQChartsColumn> columns;

      (void) CQChartsUtil::fromString(defValue, columns);

      QStringList columnStrs;
      QString     columnTypeStr;

      if (! parseParameterColumnsEdit(parameter, plotData, columns, columnStrs, columnTypeStr))
        return;

      if (columns.empty())
        return;

      column = columns[0];
    }

    break;
  }

  if (! column.isValid())
    return;

  if (! modelData_)
    return;

  CQChartsModelDetails *details = modelData_->details();
  if (! details) return;

  if (column.type() != CQChartsColumn::Type::DATA)
    return;

  const CQChartsModelColumnDetails *columnDetails = details->columnDetails(column.column());
  if (! details) return;

  if      (id == "xmin") {
    QVariant xmin = columnDetails->minValue();

    xminEdit_->setText(QString("%1").arg(xmin.toString()));
  }
  else if (id == "ymin") {
    QVariant ymin = columnDetails->minValue();

    yminEdit_->setText(QString("%1").arg(ymin.toString()));
  }
  else if (id == "xmax") {
    QVariant xmax = columnDetails->maxValue();

    xmaxEdit_->setText(QString("%1").arg(xmax.toString()));
  }
  else if (id == "ymax") {
    QVariant ymax = columnDetails->maxValue();

    ymaxEdit_->setText(QString("%1").arg(ymax.toString()));
  }
}

void
CQChartsPlotDlg::
validateSlot()
{
  if (! initialized_)
    return;

  CQChartsPlotType *type = getPlotType();

  if (! type)
    return;

  // set description
  descText_->setText(type->description());

  // can only set x/y range if plot supports custom range
  xminEdit_->setEnabled(type->customXRange());
  yminEdit_->setEnabled(type->customYRange());
  xmaxEdit_->setEnabled(type->customXRange());
  ymaxEdit_->setEnabled(type->customYRange());

  // can only set range if plot type supplies which column referes to these axes
  const char *xcolName = type->xColumnName();
  const char *ycolName = type->yColumnName();

  xminButton_->setEnabled(xcolName);
  yminButton_->setEnabled(ycolName);
  xmaxButton_->setEnabled(xcolName);
  ymaxButton_->setEnabled(ycolName);

  titleEdit_->setEnabled(type->hasTitle());

  xintegralCheck_->setEnabled(type->hasAxes() && type->allowXAxisIntegral());
  yintegralCheck_->setEnabled(type->hasAxes() && type->allowYAxisIntegral());

  xlogCheck_->setEnabled(type->allowXLog());
  ylogCheck_->setEnabled(type->allowYLog());

  //---

  QStringList msgs;

  bool valid = validate(msgs);

  okButton_   ->setEnabled(valid);
  applyButton_->setEnabled(valid);

  if (! valid)
    msgLabel_->setText(msgs.at(0));
  else
    msgLabel_->setText(" ");

  msgLabel_->setFixedHeight(msgLabel_->sizeHint().height());

  //---

  updatePreviewPlot(valid);
}

void
CQChartsPlotDlg::
updatePreviewPlot(bool valid)
{
  if (valid && previewEnabledCheck_->isChecked()) {
    // create plot for typename of current tab
    CQChartsPlotType *type = getPlotType();
    assert(type);

    ModelP previewModel;

    if (summaryEnabledCheck_ && summaryEnabledCheck_->isChecked() && summaryModel_)
      previewModel = summaryModelP_;
    else
      previewModel = model_;

    if (! previewPlot_ || previewPlot_->type() != type || previewPlot_->model() != previewModel) {
      previewView_->removeAllPlots();

      previewPlot_ = type->create(previewView_, previewModel);

      previewPlot_->setPreview(true);

      double vr = CQChartsView::viewportRange();

      CQChartsGeom::BBox bbox(0, 0, vr, vr);

      previewPlot_->setId("Preview");

      previewView_->addPlot(previewPlot_, bbox);

      previewPlot_->setAutoFit(true);
    }

    applyPlot(previewPlot_, /*preview*/true);
  }
  else {
    if (previewPlot_) {
      previewView_->removeAllPlots();

      previewPlot_ = nullptr;

      previewView_->update();
    }
  }
}

void
CQChartsPlotDlg::
updateFormatSlot()
{
  QToolButton *formatUpdate = qobject_cast<QToolButton *>(sender());

  if (! formatUpdate)
    return;

  CQChartsPlotType *type = getPlotType();

  if (! type)
    return;

  PlotData &plotData = typePlotData_[type->name()];

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

  CQChartsColumn column;
  QString        columnStr;
  QString        columnType;
  CQChartsColumn defColumn;

  auto pce = plotData.columnEdits.find(parameterName);

  if (pce != plotData.columnEdits.end()) {
    if (! columnLineEditValue((*pce).second, column, columnStr, columnType, defColumn))
      return;
  }
  else {
    auto pce = plotData.columnsEdits.find(parameterName);

    if (pce != plotData.columnsEdits.end()) {
      if (! columnLineEditValue((*pce).second, column, columnStr, columnType, defColumn))
        return;
    }
  }

  if (! column.isValid())
    return;

  QString typeStr;

  if (summaryEnabledCheck_ && summaryEnabledCheck_->isChecked() && summaryModel_) {
    if (! CQChartsUtil::columnTypeStr(charts_, summaryModel_, column, typeStr))
      return;
  }
  else {
    if (! CQChartsUtil::columnTypeStr(charts_, model_.data(), column, typeStr))
      return;
  }

  formatEdit->setText(typeStr);
}

bool
CQChartsPlotDlg::
validate(QStringList &msgs)
{
  msgs.clear();

  CQChartsModelData *modelData = nullptr;

  if (summaryEnabledCheck_ && summaryEnabledCheck_->isChecked() && summaryModel_)
    modelData = summaryModelData_;
  else
    modelData = modelData_;

  if (! modelData) { msgs << "no model data"; return false; }

  const CQChartsModelDetails *details = modelData->details();

  //---

  // create plot for typename of current tab
  CQChartsPlotType *type = getPlotType();

  if (! type) {
    msgs << "invalid plot type";
    return false;
  }

  // set plot property for widgets for plot parameters
  PlotData &plotData = typePlotData_[type->name()];

  bool rc = true;

  int num_valid = 0;

  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column") {
      CQChartsColumn column;

      bool ok;

      int icolumn = parameter.defValue().toInt(&ok);

      if (ok)
        column = icolumn;

      QString      columnStr;
      QString      columnTypeStr;
      MapValueData mapValueData;

      if (! parseParameterColumnEdit(parameter, plotData, column, columnStr,
                                     columnTypeStr, mapValueData)) {
        if (parameter.attributes().isRequired()) {
          msgs << "missing required column value";
          rc = false;
        }

        continue;
      }

      if (! column.isValid()) {
        msgs << "invalid column value";
        rc = false;
        continue;
      }

      bool rc1 = true;

      if (column.type() == CQChartsColumn::Type::DATA) {
        const CQChartsModelColumnDetails *columnDetails = details->columnDetails(column.column());

        if (parameter.attributes().isMonotonic()) {
          if (! columnDetails->isMonotonic()) {
            msgs << "non-monotonic column";
            rc1 = false;
          }
        }

        if      (parameter.attributes().isNumeric()) {
          if (columnDetails->type() != CQBaseModel::Type::INTEGER &&
              columnDetails->type() != CQBaseModel::Type::REAL &&
              columnDetails->type() != CQBaseModel::Type::TIME) {
            msgs << "non-numeric column";
            rc1 = false;
          }
        }
        else if (parameter.attributes().isString()) {
          if (columnDetails->type() != CQBaseModel::Type::STRING) {
            msgs << "non-string column";
            rc1 = false;
          }
        }
        else if (parameter.attributes().isColor()) {
          if (columnDetails->type() != CQBaseModel::Type::COLOR) {
            msgs << "non-color column";
            rc1 = false;
          }
        }
      }

      if (rc1)
        ++num_valid;
      else
        rc = rc1;
    }
    else if (parameter.type() == "columns") {
      bool ok;

      QString defValue = CQChartsUtil::toString(parameter.defValue(), ok);

      std::vector<CQChartsColumn> columns;

      (void) CQChartsUtil::fromString(defValue, columns);

      QStringList columnStrs;
      QString     columnTypeStr;

      if (! parseParameterColumnsEdit(parameter, plotData, columns, columnStrs, columnTypeStr)) {
        if (parameter.attributes().isRequired()) {
          msgs << "missing required column value";
          rc = false;
        }

        continue;
      }

      if (! columns.empty()) {
        ++num_valid;
      }
    }
  }

  if (num_valid == 0) {
    msgs << "no columns specified";
    rc = false;
  }

  return rc;
}

void
CQChartsPlotDlg::
previewEnabledSlot()
{
  validateSlot();
}

void
CQChartsPlotDlg::
updatePreviewSlot()
{
  if (summaryEnabledCheck_ && summaryEnabledCheck_->isChecked() && summaryModel_) {
    int  n = previewMaxRows_->value();

    bool random = previewRandomRadio_->isChecked();
    bool sorted = previewSortedRadio_->isChecked();
    bool paged  = previewPagedRadio_ ->isChecked();

    if (n <= 0) return;

    if (n != summaryModel_->maxRows()) {
      summaryModel_->setMode(CQSummaryModel::Mode::NORMAL);
      summaryModel_->setMaxRows(n);
    }

    if      (random) {
      summaryModel_->setRandom(true);
    }
    else if (sorted) {
      int sortCol = previewSortedColEdit_->value();

      summaryModel_->setSortColumn(sortCol);
      summaryModel_->setSorted(true);
    }
    else if (paged) {
      int ps = previewPageSizeEdit_   ->value();
      int np = previewCurrentPageEdit_->value();

      int nr = model_.data()->rowCount();

      int np1 = (nr + ps - 1)/ps;

      np = std::min(np, np1 - 1);

      previewCurrentPageEdit_->setRange(0, np1 - 1);
      previewCurrentPageEdit_->setToolTip(QString("Set Preview Page Count (0 -> %1)").arg(np1 - 1));

      summaryModel_->setPageSize(ps);
      summaryModel_->setCurrentPage(np);
      summaryModel_->setPaged(true);
    }
    else {
      summaryModel_->setMode(CQSummaryModel::Mode::NORMAL);
    }
  }

  validateSlot();
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
  plot_ = nullptr;

  // create plot for typename of current tab
  CQChartsPlotType *type = getPlotType();

  if (! type)
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

  return true;
}

CQChartsPlotType *
CQChartsPlotDlg::
getPlotType() const
{
  int ind = stack_->currentIndex();

  auto p = tabType_.find(ind);

  if (p == tabType_.end())
    return nullptr;

  CQChartsPlotType *type = (*p).second;

  return type;
}

void
CQChartsPlotDlg::
applyPlot(CQChartsPlot *plot, bool preview)
{
  ModelP model = plot->model();

  CQChartsPlotType *type = plot->type();

  // set plot property for widgets for plot parameters
  PlotData &plotData = typePlotData_[type->name()];

  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column") {
      CQChartsColumn column;

      bool ok;

      int icolumn = parameter.defValue().toInt(&ok);

      if (ok)
        column = icolumn;

      QString      columnStr;
      QString      columnTypeStr;
      MapValueData mapValueData;

      if (parseParameterColumnEdit(parameter, plotData, column, columnStr,
                                   columnTypeStr, mapValueData)) {
        if (! CQUtil::setProperty(plot, parameter.propName(), column.toString()))
          charts()->errorMsg("Failed to set parameter '" + parameter.propName() + "'");

        if (columnTypeStr.length())
          CQChartsUtil::setColumnTypeStr(charts_, model.data(), column, columnTypeStr);

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
        if (preview && ok)
          CQUtil::setProperty(plot, parameter.propName(), QVariant(icolumn));
      }
    }
    else if (parameter.type() == "columns") {
      bool ok;

      QString defValue = CQChartsUtil::toString(parameter.defValue(), ok);

      std::vector<CQChartsColumn> columns;

      (void) CQChartsUtil::fromString(defValue, columns);

      QStringList columnStrs;
      QString     columnTypeStr;

      if (parseParameterColumnsEdit(parameter, plotData, columns, columnStrs, columnTypeStr)) {
        QString s = CQChartsUtil::toString(columns);

        if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(s)))
          charts()->errorMsg("Failed to set parameter '" + parameter.propName() + "'");

        if (columnTypeStr.length() && ! columns.empty())
          CQChartsUtil::setColumnTypeStr(charts_, model.data(), columns[0], columnTypeStr);
      }
      else {
        if (preview && ok)
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
        if (preview && ok)
          CQUtil::setProperty(plot, parameter.propName(), QVariant(defStr));
      }
    }
    else if (parameter.type() == "real") {
      bool ok;

      double defValue = parameter.defValue().toDouble(&ok);

      double r = defValue;

      if (parseParameterRealEdit(parameter, plotData, r)) {
        if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(r)))
          charts()->errorMsg("Failed to set parameter '" + parameter.propName() + "'");
      }
      else {
        if (preview && ok)
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

  if (plot->xAxis() && type->allowXAxisIntegral())
    plot->xAxis()->setIntegral(xintegralCheck_->isChecked());

  if (plot->yAxis() && type->allowYAxisIntegral())
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

void
CQChartsPlotDlg::
cancelSlot()
{
  plot_ = nullptr;

  reject();
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
                         CQChartsColumn &column, QString &columnStr, QString &columnType,
                         MapValueData &mapValueData)
{
  auto pf = plotData.formatEdits.find(parameter.name());
  assert(pf != plotData.formatEdits.end());

  const FormatEditData &formatEditData = (*pf).second;

  QString format = formatEditData.formatEdit->text();

  if (format != "")
    columnType = format;

  //---

  CQChartsColumn defColumn;

  bool ok;

  int icolumn = parameter.defValue().toInt(&ok);

  if (ok)
    defColumn = icolumn;

  auto pe = plotData.columnEdits.find(parameter.name());
  assert(pe != plotData.columnEdits.end());

  if (! columnLineEditValue((*pe).second, column, columnStr, columnType, defColumn))
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
                          std::vector<CQChartsColumn> &columns, QStringList &columnStrs,
                          QString &columnType)
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

  CQChartsColumn column;
  QString        columnStr;

  bool ok = columnLineEditValue((*pe).second, column, columnStr, columnType);

  if (ok) {
    columns   .push_back(column);
    columnStrs.push_back(columnStr);

    return true;
  }

  return columnLineEditValues((*pe).second, columns, columnStrs, columnType);
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
columnLineEditValue(CQChartsColumnEdit *le, CQChartsColumn &column, QString &columnStr,
                    QString &columnType, const CQChartsColumn &defColumn) const
{
  QString str = le->text().simplified();

  if (! str.length())
    return false;

  //--

  return columnTextValue(str, column, columnStr, columnType, defColumn);
}

bool
CQChartsPlotDlg::
columnLineEditValue(QLineEdit *le, CQChartsColumn &column, QString &columnStr,
                    QString &columnType, const CQChartsColumn &defColumn) const
{
  QString str = le->text().simplified();

  if (! str.length())
    return false;

  //--

  return columnTextValue(str, column, columnStr, columnType, defColumn);
}

bool
CQChartsPlotDlg::
columnTextValue(QString &str, CQChartsColumn &column, QString &columnStr,
                QString &columnType, const CQChartsColumn &defColumn) const
{
  if (str.left(1) != "(") {
    int pos = str.indexOf(":");

    if (pos >= 0) {
      str        = str.mid(0, pos).simplified();
      columnType = str.mid(pos + 1).simplified();
    }
  }

  if (! stringToColumn(str, column)) {
    column = defColumn;

    return false;
  }

  columnStr = str;

  return true;
}

bool
CQChartsPlotDlg::
columnLineEditValues(QLineEdit *le, std::vector<CQChartsColumn> &columns, QStringList &columnStrs,
                     QString &columnType) const
{
  bool ok = true;

  // TODO: better split to handle spaces in column names/expressions
  QStringList strs = le->text().split(" ", QString::SkipEmptyParts);

  for (int i = 0; i < strs.size(); ++i) {
    const QString &str = strs[i];

    if (str.left(1) != "(") {
      int pos = str.indexOf(":");

      QString lhs, rhs;

      if (pos > 0) {
        lhs = str.mid(0, pos).simplified();
        rhs = str.mid(pos + 1).simplified();
      }
      else
        lhs = str.simplified();

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
        CQChartsColumn col;

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
    else {
      CQChartsColumn col;

      if (stringToColumn(str, col)) {
        columns   .push_back(col);
        columnStrs.push_back(str);
      }
      else
        ok = false;
    }
  }

  return ok;
}

bool
CQChartsPlotDlg::
stringToColumn(const QString &str, CQChartsColumn &column) const
{
  return CQChartsUtil::stringToColumn(model(), str, column);
}

void
CQChartsPlotDlg::
setViewName(const QString &viewName)
{
  if (viewEdit_)
    viewEdit_->setText(viewName);
}

int
CQChartsPlotDlg::
exec()
{
  plot_ = nullptr;

  return QDialog::exec();
}
