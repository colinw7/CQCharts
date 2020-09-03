#include <CQChartsCreatePlotDlg.h>
#include <CQChartsPlotType.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotParameter.h>
#include <CQChartsAxis.h>
#include <CQChartsVariant.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnEdit.h>
#include <CQChartsColumnsEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsModelViewHolder.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetailsWidget.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelChooser.h>
#include <CQChartsAnalyzeModel.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsLineEdit.h>
#include <CQCharts.h>

#include <CQSummaryModel.h>
#include <CQTabSplit.h>
#include <CQIntegerSpin.h>
#include <CQRealSpin.h>
#include <CQUtil.h>
#include <CQCustomCombo.h>
#include <CQGroupBox.h>
#include <CQTclUtil.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QStackedWidget>
#include <QTextEdit>
#include <QCheckBox>
#include <QToolButton>
#include <QLabel>
#include <cassert>

#include <svg/refresh_svg.h>
#include <svg/info_svg.h>

CQChartsCreatePlotDlg::
CQChartsCreatePlotDlg(CQCharts *charts, CQChartsModelData *modelData) :
 charts_(charts), modelData_(modelData)
{
  assert(modelData_);

  model_ = modelData_->currentModel();

  init();
}

CQChartsCreatePlotDlg::
~CQChartsCreatePlotDlg()
{
}

void
CQChartsCreatePlotDlg::
init()
{
#if 0
  auto createSep = [](const QString &name) {
    auto *sep = CQUtil::makeWidget<QFrame>(name);

    sep->setFixedHeight(4);
    sep->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    return sep;
  };
#endif

  //---

  setObjectName("plotDlg");

  setWindowTitle(QString("Create Plot (Model %1)").arg(modelData_->ind()));
//setWindowIcon(QIcon()); TODO

  //----

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //----

  auto *modelChooserFrame  = CQUtil::makeWidget<QFrame>("modelChooserFrame");
  auto *modelChooserLayout = CQUtil::makeLayout<QHBoxLayout>(modelChooserFrame, 2, 2);

  modelChooser_ = new CQChartsModelChooser(charts_);

  connect(modelChooser_, SIGNAL(currentModelChanged()), this, SLOT(modelChangeSlot()));

  modelChooserLayout->addWidget (CQUtil::makeLabelWidget<QLabel>("Model", "modelLabel"));
  modelChooserLayout->addWidget (modelChooser_);
  modelChooserLayout->addStretch(1);

  layout->addWidget(modelChooserFrame);

  //----

  auto *area = CQUtil::makeWidget<CQTabSplit>("area");

  area->setOrientation(Qt::Vertical);
  area->setGrouped(true);
  area->setState(CQTabSplit::State::VSPLIT);

  layout->addWidget(area);

  //----

  // add data frame
  auto *dataFrame = createDataFrame();

  area->addWidget(dataFrame, "Plot Data");

  //----

  auto *dataArea = CQUtil::makeWidget<CQTabSplit>("dataArea");

  dataArea->setOrientation(Qt::Vertical);
  dataArea->setGrouped(true);
  dataArea->setState(CQTabSplit::State::TAB);

  area->addWidget(dataArea, "Data/Preview");

  //---

  // add summary frame
  auto *summaryFrame = createSummaryFrame();

  dataArea->addWidget(summaryFrame, "Model Data");

  //----

  // add details frame
  auto *detailsFrame = createDetailsFrame();

  dataArea->addWidget(detailsFrame, "Model Details");

  //----

  // add preview frame
  auto *previewFrame = createPreviewFrame();

  dataArea->addWidget(previewFrame, "Plot Preview");

  //----

  //auto *sep1 = createSep("sep1");

  //layout->addWidget(sep1);

  //----

  msgLabel_ = CQUtil::makeLabelWidget<QLabel>("", "msgLabel");

  layout->addWidget(msgLabel_);

  //----

  //auto *sep2 = createSep("sep2");

  //layout->addWidget(sep2);

  //-------

  // OK, Apply, Cancel Buttons
  auto *buttons = new CQChartsDialogButtons(this);

  buttons->connect(this, SLOT(okSlot()), SLOT(applySlot()), SLOT(cancelSlot()));

  buttons->setToolTips("Create Plot and close dialog",
                       "Create plot and keep dialog open",
                       "Close dialog without creating plot");

  okButton_    = buttons->okButton();
  applyButton_ = buttons->applyButton();

  layout->addWidget(buttons);

  //--

  connectSlots(true);

  initialized_ = true;

  //---

  updateModelData();

  validateSlot();
}

QFrame *
CQChartsCreatePlotDlg::
createDataFrame()
{
  auto *dataFrame  = CQUtil::makeWidget<QFrame>("data");
  auto *dataLayout = CQUtil::makeLayout<QVBoxLayout>(dataFrame, 2, 2);

  //----

  auto *typeLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  dataLayout->addLayout(typeLayout);

  // create type combo
  auto *typeComboFrame = createTypeCombo();

  typeLayout->addWidget(typeComboFrame);

  //--

  // auto analyze model data
  QCheckBox *autoAnalyzeCheck =
    CQUtil::makeLabelWidget<QCheckBox>("Auto Analyze", "autoAnalyzeCheck");

  autoAnalyzeCheck->setChecked(isAutoAnalyzeModel());
  autoAnalyzeCheck->setToolTip("Automatically set columns from model data");

  connect(autoAnalyzeCheck, SIGNAL(stateChanged(int)), this, SLOT(autoAnalyzeSlot(int)));

  typeLayout->addWidget(autoAnalyzeCheck);

  //----

  auto *dataTab = CQUtil::makeWidget<QTabWidget>("dataTab");

  auto *typeFrame = createTypeDataFrame();

  dataTab->addTab(typeFrame, "Type");

  auto *genFrame = createGeneralDataFrame();

  dataTab->addTab(genFrame, "General");

  //---

  dataLayout->addWidget(dataTab);

  return dataFrame;
}

QFrame *
CQChartsCreatePlotDlg::
createTypeDataFrame()
{
  auto *typeFrame  = CQUtil::makeWidget<QFrame>("type");
  auto *typeLayout = CQUtil::makeLayout<QVBoxLayout>(typeFrame, 2, 2);

  //----

  // get plot types
  CQCharts::PlotTypes plotTypes;

  sortedPlotTypes(plotTypes);

  auto *typeTab = CQUtil::makeWidget<QTabWidget>("typeTab");

  typeLayout->addWidget(typeTab);

  // plot type widgets in stack (one per type)
  auto *plotDataFrame  = CQUtil::makeWidget<QFrame>("plotDataFrame");
  auto *plotDataLayout = CQUtil::makeLayout<QVBoxLayout>(plotDataFrame, 2, 2);

  auto *plotDataCheck = CQUtil::makeLabelWidget<QCheckBox>("Advanced", "plotDataCheck");

  plotDataCheck->setToolTip("Show advanced plot options");

  connect(plotDataCheck, SIGNAL(stateChanged(int)), this, SLOT(plotDataSlot(int)));

  plotDataLayout->addWidget(plotDataCheck);

  plotDataStack_ = CQUtil::makeWidget<QStackedWidget>("stack");

  plotDataLayout->addWidget(plotDataStack_);

  basicPlotDataStack_    = CQUtil::makeWidget<QStackedWidget>("basicStack");
  advancedPlotDataStack_ = CQUtil::makeWidget<QStackedWidget>("advancedStack");

  for (std::size_t i = 0; i < plotTypes.size(); ++i)
    addPlotWidgets(plotTypes[i], i);

  plotDataStack_->addWidget(basicPlotDataStack_   );
  plotDataStack_->addWidget(advancedPlotDataStack_);

  typeTab->addTab(plotDataFrame, "Input Data");

  //---

  // plot type description
  descText_ = CQUtil::makeWidget<QTextEdit>("descText");

  typeTab->addTab(descText_, "Description");

  //----

  // where filter edit
  whereFrame_ = CQUtil::makeWidget<QFrame>("whereFrame");

  auto *whereLayout = CQUtil::makeLayout<QHBoxLayout>(whereFrame_, 0, 2);

  typeLayout->addWidget(whereFrame_);

  auto *whereLabel = CQUtil::makeLabelWidget<QLabel>("Where", "whereLabel");

  whereLayout->addWidget(whereLabel);

  whereEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("whereEdit");

  whereEdit_->setToolTip("Where Expression");

  whereLayout->addWidget(whereEdit_);

  whereLayout->addStretch(1);

  whereEdit_->setToolTip("Filter for input data");

  whereFrame_->setVisible(isAdvanced());

  //---

  return typeFrame;
}

QFrame *
CQChartsCreatePlotDlg::
createTypeCombo()
{
  // type combo
  auto *typeComboFrame  = CQUtil::makeWidget<QFrame>("typeComboFrame");
  auto *typeComboLayout = CQUtil::makeLayout<QHBoxLayout>(typeComboFrame, 0, 2);

  auto *typeLabel = CQUtil::makeLabelWidget<QLabel>("Type", "typeLabel");

  typeComboLayout->addWidget(typeLabel);

  auto *typeCombo = CQUtil::makeWidget<CQCustomCombo>("typeCombo");

  typeComboLayout->addWidget(typeCombo);

  typeCombo->setToolTip("Plot Type");

  //---

  // get plot types
  CQCharts::PlotTypes plotTypes;

  sortedPlotTypes(plotTypes);

  //---

  QStringList items;

  bool                        plotTypeHier = false;
  CQChartsPlotType::Dimension plotTypeDim  = CQChartsPlotType::Dimension::NONE;

  QString title;

  for (auto &plotType : plotTypes) {
    bool                        hier = plotType->isHierarchical();
    CQChartsPlotType::Dimension dim  = plotType->dimension();

    if (hier != plotTypeHier) {
      if (hier)
        typeCombo->addTitle("Hierarchical");

      plotTypeHier = hier;
    }

    if (dim != plotTypeDim) {
      if      (dim == CQChartsPlotType::Dimension::ONE_D)
        typeCombo->addTitle("1D");
      else if (dim == CQChartsPlotType::Dimension::TWO_D)
        typeCombo->addTitle("2D");
      else
        typeCombo->addTitle("Other");

      plotTypeDim = dim;
    }

    typeCombo->addItem(plotType->desc());
  }

  connect(typeCombo, SIGNAL(currentIndexChanged(const QString &)),
          this, SLOT(comboSlot(const QString &)));

  //---

  typeComboLayout->addStretch(1);

  return typeComboFrame;
}

void
CQChartsCreatePlotDlg::
modelChangeSlot()
{
  updateModelData();

  validateSlot();
}

void
CQChartsCreatePlotDlg::
updateModelData()
{
  connectSlots(false);

  //---

  modelData_ = charts_->currentModelData();
  assert(modelData_);

  model_ = modelData_->currentModel();

  //---

  if (! CQChartsModelUtil::isHierarchical(model_.data()))
    (void) modelData_->addSummaryModel();

  //---

  // update summary model
  auto *summaryModel = modelData_->summaryModel();

  int nr = model_.data()->rowCount();
  int nc = model_.data()->columnCount();

  summaryEditData_.enabledCheck->setChecked(modelData_->isSummaryEnabled());

  summaryEditData_.maxRows->setRange(1, nr);
  summaryEditData_.maxRows->setToolTip(QString("Set Preview Row Count (1 -> %1)").arg(nr));

  if (summaryModel)
    summaryEditData_.maxRows->setValue(summaryModel->maxRows());
  else
    summaryEditData_.maxRows->setValue(nr);

  if (summaryModel) {
    if      (summaryModel->mode() == CQSummaryModel::Mode::RANDOM)
      summaryEditData_.typeCombo->setCurrentIndex(1);
    else if (summaryModel->mode() == CQSummaryModel::Mode::SORTED)
      summaryEditData_.typeCombo->setCurrentIndex(2);
    else if (summaryModel->mode() == CQSummaryModel::Mode::PAGED)
      summaryEditData_.typeCombo->setCurrentIndex(3);
  }

  summaryEditData_.sortedColEdit->setRange(0, nc - 1);
  summaryEditData_.sortedColEdit->setToolTip(
    QString("Set Preview Sort Column (0 -> %1)").arg(nc - 1));

  if (summaryModel)
    summaryEditData_.sortedColEdit->setValue(summaryModel->sortColumn());

  summaryEditData_.pageSizeEdit->setRange(1, nr);
  summaryEditData_.pageSizeEdit->setToolTip(QString("Set Preview Page Size (1 -> %1)").arg(nr));

  if (summaryModel)
    summaryEditData_.pageSizeEdit->setValue(summaryModel->pageSize());

  int np = (nr + summaryEditData_.pageSizeEdit->value() - 1)/
           summaryEditData_.pageSizeEdit->value();

  summaryEditData_.currentPageEdit->setRange(0, np - 1);
  summaryEditData_.currentPageEdit->setToolTip(
    QString("Set Preview Page Count (0 -> %1)").arg(np - 1));

  if (summaryModel)
    summaryEditData_.currentPageEdit->setValue(summaryModel->currentPage());

  //--

  if (modelData_->isSummaryEnabled() && summaryModel) {
    ModelP summaryModelP = modelData_->summaryModelP();

    summaryEditData_.modelView->setModel(summaryModelP,
      CQChartsModelUtil::isHierarchical(summaryModel));
  }
  else {
    summaryEditData_.modelView->setModel(model_,
      CQChartsModelUtil::isHierarchical(model_.data()));
  }

  summaryEditData_.maxRows        ->setEnabled(summaryModel);
  summaryEditData_.typeCombo      ->setEnabled(summaryModel);
  summaryEditData_.typeStack      ->setEnabled(summaryModel);
  summaryEditData_.sortedColEdit  ->setEnabled(summaryModel);
  summaryEditData_.pageSizeEdit   ->setEnabled(summaryModel);
  summaryEditData_.currentPageEdit->setEnabled(summaryModel);

  //---

  // update model details
  detailsWidget_->setModelData(modelData_);

  //---

  connectSlots(true);
}

void
CQChartsCreatePlotDlg::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    modelData_, SIGNAL(dataChanged()), this, SLOT(updateModelData()));
  CQChartsWidgetUtil::connectDisconnect(b,
    modelData_, SIGNAL(deleted()), this, SLOT(cancelSlot()));

  CQChartsWidgetUtil::connectDisconnect(b,
    summaryEditData_.enabledCheck, SIGNAL(stateChanged(int)),
    this, SLOT(summaryEnabledSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    summaryEditData_.maxRows, SIGNAL(valueChanged(int)),
    this, SLOT(updatePreviewSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    summaryEditData_.typeCombo, SIGNAL(currentIndexChanged(int)),
    this, SLOT(updateSummaryTypeSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    summaryEditData_.sortedColEdit, SIGNAL(valueChanged(int)),
    this, SLOT(updatePreviewSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    summaryEditData_.pageSizeEdit, SIGNAL(valueChanged(int)),
    this, SLOT(updatePreviewSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    summaryEditData_.currentPageEdit, SIGNAL(valueChanged(int)),
    this, SLOT(updatePreviewSlot()));
}

void
CQChartsCreatePlotDlg::
sortedPlotTypes(CQCharts::PlotTypes &plotTypes1)
{
  CQCharts::PlotTypes plotTypes;

  this->charts()->getPlotTypes(plotTypes);

  // create ordered list of types (1D, 2D, other, hierarchical)

  using DimPlotTypesMap     = std::map<int, CQCharts::PlotTypes>;
  using HierDimPlotsTypeMap = std::map<bool, DimPlotTypesMap>;

  HierDimPlotsTypeMap hierDimPlotsTypeMap;

  for (auto &plotType : plotTypes) {
    CQChartsPlotType::Dimension dim = plotType->dimension();

    int dim1 = 999;

    if (dim != CQChartsPlotType::Dimension::NONE)
      dim1 = int(dim);

    hierDimPlotsTypeMap[plotType->isHierarchical()][dim1].push_back(plotType);
  }

  //---

  plotTypes1.clear();

  for (auto &p1 : hierDimPlotsTypeMap) {
    const auto &dimPlotTypesMap = p1.second;

    for (auto &p2 : dimPlotTypesMap) {
      const auto &plotTypes = p2.second;

      for (auto &plotType : plotTypes)
        plotTypes1.push_back(plotType);
    }
  }
}

QFrame *
CQChartsCreatePlotDlg::
createGeneralDataFrame()
{
  auto createButton = [&](const QString &name, const QString &iconName, const QString &tip,
                          const char *receiver) {
    auto *button = CQUtil::makeWidget<QToolButton>(name);

    button->setIcon(CQPixmapCacheInst->getIcon(iconName));

    connect(button, SIGNAL(clicked()), this, receiver);

    button->setToolTip(tip);

    return button;
  };

  //--

  auto *genFrame  = CQUtil::makeWidget<QFrame>("general");
  auto *genLayout = CQUtil::makeLayout<QGridLayout>(genFrame, 2, 2);

  //----

  int row = 0, column = 0;

  viewEdit_ = addStringEdit(genLayout, row, column, "View Name", "viewEdit", "View Name");

  viewEdit_->setToolTip("View to add plot to. If empty create new view");

  //----

  ++row; column = 0;

  auto *positionGroup       = CQUtil::makeLabelWidget<CQGroupBox>("Position", "positionGroup");
  auto *positionGroupLayout = CQUtil::makeLayout<QVBoxLayout>(positionGroup, 2, 2);

  genLayout->addWidget(positionGroup, row, column, 1, 4);

  //--

  autoRangeEdit_ = CQUtil::makeLabelWidget<QCheckBox>("Auto Place", "autoPlace");

  autoRangeEdit_->setChecked(true);
  autoRangeEdit_->setToolTip("Automatically place plots in view");

  positionGroupLayout->addWidget(autoRangeEdit_);

  connect(autoRangeEdit_, SIGNAL(stateChanged(int)), this, SLOT(autoPlaceSlot(int)));

  auto *rangeEditLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  positionGroupLayout->addLayout(rangeEditLayout);

  posEdit_ = addStringEdit(rangeEditLayout, row, column, "Range", "range", "Plot Range");

  posEdit_->setText("0 0 1 1");
  posEdit_->setEnabled(false);
  posEdit_->setToolTip("Position and size of plot in view (values in range 0->1)");

  //----

  ++row; column = 0;

  titleEdit_ = addStringEdit(genLayout, row, column, "Plot Title", "title", "Title");

  titleEdit_->setToolTip("Plot Title");

  connect(titleEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  //----

  ++row; column = 0;

  xLabelEdit_ = addStringEdit(genLayout, row, column, "X Label", "xlabel", "Label");

  xLabelEdit_->setToolTip("X Axis Label");

  connect(xLabelEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  yLabelEdit_ = addStringEdit(genLayout, row, column, "Y Label", "ylabel", "Label");

  yLabelEdit_->setToolTip("Y Axis Label");

  connect(yLabelEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  //----

  ++row; column = 0;

  auto *rangeGroup       = CQUtil::makeLabelWidget<CQGroupBox>("Range", "rangeGroup");
  auto *rangeGroupLayout = CQUtil::makeLayout<QVBoxLayout>(rangeGroup, 2, 2);

  genLayout->addWidget(rangeGroup, row, column, 1, 4);

  //--

  auto *xRangeFrame       = CQUtil::makeWidget<QFrame>("xRangeFrame");
  auto *xRangeFrameLayout = CQUtil::makeLayout<QHBoxLayout>(xRangeFrame, 0, 2);

  rangeGroupLayout->addWidget(xRangeFrame);

  //--

  rangeEditData_.xminEdit =
    addRealEdit(xRangeFrameLayout, row, column, "X Min", "xmin", "Custom X Axis Minimum Value");
  rangeEditData_.xminButton =
    createButton("xminButton", "REFRESH", "Get minimum value for x column", SLOT(xminSlot()));

  xRangeFrameLayout->addWidget(rangeEditData_.xminButton);

  rangeEditData_.yminEdit =
    addRealEdit(xRangeFrameLayout, row, column, "Y Min", "ymin", "Custom Y Axis Minimum Value");
  rangeEditData_.yminButton =
    createButton("yminButton", "REFRESH", "Get minimum value for y column", SLOT(yminSlot()));

  xRangeFrameLayout->addWidget(rangeEditData_.yminButton);

  connect(rangeEditData_.xminEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(validateSlot()));
  connect(rangeEditData_.yminEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(validateSlot()));

  //--

  auto *yRangeFrame       = CQUtil::makeWidget<QFrame>("yRangeFrame");
  auto *yRangeFrameLayout = CQUtil::makeLayout<QHBoxLayout>(yRangeFrame, 0, 2);

  rangeGroupLayout->addWidget(yRangeFrame);

  //--

  rangeEditData_.xmaxEdit =
    addRealEdit(yRangeFrameLayout, row, column, "X Max", "xmax", "Custom X Axis Maximum Value");
  rangeEditData_.xmaxButton =
    createButton("xmaxButton", "REFRESH", "Get maximum value for x column", SLOT(xmaxSlot()));

  yRangeFrameLayout->addWidget(rangeEditData_.xmaxButton);

  rangeEditData_.ymaxEdit =
    addRealEdit(yRangeFrameLayout, row, column, "Y Max", "ymax", "Custom Y Axis Maximum Value");
  rangeEditData_.ymaxButton =
    createButton("ymaxButton", "REFRESH", "Get maximum value for y column", SLOT(ymaxSlot()));

  yRangeFrameLayout->addWidget(rangeEditData_.ymaxButton);

  connect(rangeEditData_.xmaxEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(validateSlot()));
  connect(rangeEditData_.ymaxEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(validateSlot()));

  //----

  ++row; column = 0;

  auto *xyFrame       = CQUtil::makeWidget<QFrame>("xyFrame");
  auto *xyFrameLayout = CQUtil::makeLayout<QHBoxLayout>(xyFrame, 0, 2);

  genLayout->addWidget(xyFrame, row, column, 1, 5);

  xintegralCheck_ = CQUtil::makeLabelWidget<QCheckBox>("X Integral", "xintegralCheck");
  yintegralCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Y Integral", "yintegralCheck");

  xintegralCheck_->setToolTip("X values are Integral");
  yintegralCheck_->setToolTip("Y values are Integral");

  connect(xintegralCheck_, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));
  connect(yintegralCheck_, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));

  xyFrameLayout->addWidget(xintegralCheck_);
  xyFrameLayout->addWidget(yintegralCheck_);

  //--

  auto *xySpacer = CQChartsWidgetUtil::createHSpacer(1, "xySpacer");

  xyFrameLayout->addWidget(xySpacer);

  //--

  xlogCheck_ = CQUtil::makeLabelWidget<QCheckBox>("X Log", "xlogCheck");
  ylogCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Y Log", "ylogCheck");

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
CQChartsCreatePlotDlg::
createDetailsFrame()
{
  auto *detailsFrame  = CQUtil::makeWidget<QFrame>("details");
  auto *detailsLayout = CQUtil::makeLayout<QVBoxLayout>(detailsFrame, 0, 2);

  detailsWidget_ = new CQChartsModelDetailsWidget(charts_);

  detailsLayout->addWidget(detailsWidget_);

  return detailsFrame;
}

QFrame *
CQChartsCreatePlotDlg::
createSummaryFrame()
{
  auto *summaryFrame = CQUtil::makeWidget<QFrame>("summary");

  summaryFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *summaryLayout = CQUtil::makeLayout<QVBoxLayout>(summaryFrame, 0, 2);

  //--

  auto *summaryControl = CQUtil::makeWidget<QFrame>("summaryControl");

  summaryControl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *summaryControlLayout = CQUtil::makeLayout<QHBoxLayout>(summaryControl, 2, 2);

  //---

  summaryEditData_.enabledCheck =
    CQUtil::makeLabelWidget<QCheckBox>("Summary", "summaryEnabled");

  summaryEditData_.enabledCheck->setToolTip("Enable summary of model data");

  summaryControlLayout->addWidget(summaryEditData_.enabledCheck);

  //---

  summaryEditData_.maxRows = CQUtil::makeWidget<CQIntegerSpin>("summaryMaxRows");

  summaryControlLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Max Rows", "maxRows"));
  summaryControlLayout->addWidget(summaryEditData_.maxRows);

  //---

  summaryEditData_.typeCombo = CQUtil::makeWidget<QComboBox>("summaryTypeCombo");

  summaryEditData_.typeCombo->addItems(
    QStringList() << "Normal" << "Random" << "Sorted" << "Paged");

  summaryEditData_.typeCombo->setToolTip("Summary data selection type");

  summaryControlLayout->addWidget(summaryEditData_.typeCombo);

  //----

  summaryEditData_.typeStack = CQUtil::makeWidget<QStackedWidget>("summaryTypeStack");

  summaryControlLayout->addWidget(summaryEditData_.typeStack);

  //---

  auto *normalTypeFrame = CQUtil::makeWidget<QFrame>("normalTypeFrame");

  summaryEditData_.typeStack->addWidget(normalTypeFrame);

  //---

  auto *randomTypeFrame = CQUtil::makeWidget<QFrame>("randomTypeFrame");

  summaryEditData_.typeStack->addWidget(randomTypeFrame);

  //---

  auto *sortedTypeFrame = CQUtil::makeWidget<QFrame>("sortedTypeFrame");

  summaryEditData_.typeStack->addWidget(sortedTypeFrame);

  auto *sortedTypeLayout = CQUtil::makeLayout<QHBoxLayout>(sortedTypeFrame, 0, 2);

  //--

  summaryEditData_.sortedColEdit = CQUtil::makeWidget<CQIntegerSpin>("summarySortedColEdit");

  sortedTypeLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Sort Column", "sortCol"));
  sortedTypeLayout->addWidget(summaryEditData_.sortedColEdit);

  sortedTypeLayout->addStretch(1);

  //---

  auto *pageSizeTypeFrame = CQUtil::makeWidget<QFrame>("pageSizeTypeFrame");

  summaryEditData_.typeStack->addWidget(pageSizeTypeFrame);

  auto *pageSizeTypeLayout = CQUtil::makeLayout<QHBoxLayout>(pageSizeTypeFrame, 0, 2);

  //--

  summaryEditData_.pageSizeEdit = CQUtil::makeWidget<CQIntegerSpin>("summaryPageSizeEdit");

  pageSizeTypeLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Page Size", "pointSize"));
  pageSizeTypeLayout->addWidget(summaryEditData_.pageSizeEdit);

  //--

  summaryEditData_.currentPageEdit = CQUtil::makeWidget<CQIntegerSpin>("summaryCurrentPageEdit");

  pageSizeTypeLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Current Page", "currentPage"));
  pageSizeTypeLayout->addWidget(summaryEditData_.currentPageEdit);

  pageSizeTypeLayout->addStretch(1);

  //---

  summaryControlLayout->addStretch(1);

  summaryLayout->addWidget(summaryControl);

  //--

  summaryEditData_.modelView = new CQChartsModelViewHolder(charts_);

  summaryLayout->addWidget(summaryEditData_.modelView);

  //--

  return summaryFrame;
}

QFrame *
CQChartsCreatePlotDlg::
createPreviewFrame()
{
  auto *previewFrame  = CQUtil::makeWidget<QFrame>("preview");
  auto *previewLayout = CQUtil::makeLayout<QVBoxLayout>(previewFrame, 0, 2);

  //--

  auto *previewControl       = CQUtil::makeWidget<QFrame>("previewControl");
  auto *previewControlLayout = CQUtil::makeLayout<QHBoxLayout>(previewControl, 2, 2);

  previewEnabledCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Enabled", "previewEnabled");

  previewEnabledCheck_->setToolTip("Enable plot preview");

  connect(previewEnabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(previewEnabledSlot()));

  previewControlLayout->addWidget(previewEnabledCheck_);

  auto *previewFitButton = CQUtil::makeLabelWidget<QPushButton>("Fit", "previewFit");

  connect(previewFitButton, SIGNAL(clicked()), this, SLOT(previewFitSlot()));

  previewControlLayout->addStretch(1);

  previewLayout->addWidget(previewControl);

  //--

  //auto *previewTab = CQUtil::makeWidget<QTabWidget>("previewTab");

  //previewLayout->addWidget(previewTab);

  //--

  previewView_ = charts_->createView();

  previewView_->setPreview(true);

  previewLayout->addWidget(previewView_);

  //----

  return previewFrame;
}

void
CQChartsCreatePlotDlg::
addPlotWidgets(CQChartsPlotType *type, int ind)
{
  // add widgets for type
  auto *basicFrame = CQUtil::makeWidget<QFrame>(type->name() + "_frame");

  basicPlotDataStack_->addWidget(basicFrame);

  // add basic parameter edits
  auto *basicFrameLayout = CQUtil::makeLayout<QGridLayout>(basicFrame, 2, 2);

  int basicRow = 0;

  auto &basicPlotData = basicTypePlotData_[type->name()];

  addParameterEdits(type, basicPlotData, basicFrameLayout, basicRow, /*basic*/true);

  basicPlotData.ind = ind;

  //------

  // add widgets for type
  auto *advancedFrame = CQUtil::makeWidget<QFrame>(type->name() + "_frame");

  advancedPlotDataStack_->addWidget(advancedFrame);

  // add advanced parameter edits
  auto *advancedFrameLayout = CQUtil::makeLayout<QGridLayout>(advancedFrame, 2, 2);

  int advancedRow = 0;

  auto &advancedPlotData = advancedTypePlotData_[type->name()];

  addParameterEdits(type, advancedPlotData, advancedFrameLayout, advancedRow, /*basic*/false);

  advancedPlotData.ind = ind;

  //---

  stackIndexPlotType_[basicPlotData.ind] = type;
}

void
CQChartsCreatePlotDlg::
addParameterEdits(CQChartsPlotType *type, PlotData &plotData,
                  QGridLayout *layout, int &row, bool isBasic)
{
  using GroupTab    = std::map<int, QTabWidget*>;
  using ChildGroups = std::vector<CQChartsPlotParameterGroup *>;

  GroupTab    groupTab;
  ChildGroups childGroups;

  for (const auto &p : type->parameterGroups()) {
    auto *parameterGroup = p.second;

    if (parameterGroup->isHidden())
      continue;

    if (isBasic && (parameterGroup->type() == CQChartsPlotParameterGroup::SECONDARY))
      continue;

    if (parameterGroup->parentGroupId() >= 0) {
      childGroups.push_back(parameterGroup);
      continue;
    }

    //---

    CQChartsPlotType::Parameters parameters =
      type->groupParameters(parameterGroup->groupId());

    CQChartsPlotType::ParameterGroups parameterGroups =
      type->groupParameterGroups(parameterGroup->groupId());

    assert(! parameters.empty() || ! parameterGroups.empty());

    //---

    // get tab widget
    auto pg = groupTab.find(-1);

    if (pg == groupTab.end()) {
      auto *parameterGroupTab = CQUtil::makeWidget<QTabWidget>("parameterGroupTab");

      layout->addWidget(parameterGroupTab, row, 0, 1, 5);

      ++row;

      pg = groupTab.insert(pg, GroupTab::value_type(-1, parameterGroupTab));
    }

    auto *parameterGroupTab = (*pg).second;

    //---

    // create frame for widgets and add to tab widget
    auto *parameterGroupFrame  = CQUtil::makeWidget<QFrame>(parameterGroup->name());
    auto *parameterGroupLayout = CQUtil::makeLayout<QGridLayout>(parameterGroupFrame, 2, 2);

    int row1 = 0;

    //---

    ChildGroups childGroups1;

    for (const auto &parameterGroup1 : parameterGroups) {
      if (parameterGroup1->isHidden())
        continue;

      if (isBasic && (parameterGroup1->type() == CQChartsPlotParameterGroup::SECONDARY))
        continue;

      childGroups1.push_back(parameterGroup1);
    }

    int ng1 = childGroups1.size();

    for (const auto &parameterGroup1 : childGroups1) {
      CQChartsPlotType::Parameters parameters1 =
        type->groupParameters(parameterGroup1->groupId());

      CQChartsPlotType::ParameterGroups parameterGroups1 =
        type->groupParameterGroups(parameterGroup1->groupId());

      assert(! parameters1.empty() || ! parameterGroups1.empty());

      //---

      QTabWidget *parameterGroupTab1 = nullptr;

      if (ng1 > 1) {
        // get tab widget
        auto pg = groupTab.find(parameterGroup->groupId());

        if (pg == groupTab.end()) {
          auto *parameterGroupTab1 = CQUtil::makeWidget<QTabWidget>("parameterGroupTab");

          parameterGroupLayout->addWidget(parameterGroupTab1, row1, 0);

          ++row1;

          pg = groupTab.insert(pg,
            GroupTab::value_type(parameterGroup->groupId(), parameterGroupTab1));
        }

        parameterGroupTab1 = (*pg).second;
      }

      //---

      int row2 = 0;

      auto *parameterGroupFrame1  = CQUtil::makeWidget<QFrame>(parameterGroup1->name());
      auto *parameterGroupLayout1 = CQUtil::makeLayout<QGridLayout>(parameterGroupFrame1, 2, 2);

      if (! parameters1.empty()) {
        addParameterEdits(parameters1, plotData, parameterGroupLayout1, row2, isBasic);

        if (row2 > 0)
          parameterGroupLayout1->setRowStretch(row2, 1);
      }

      if (row2 > 0) {
        if (parameterGroupTab1)
          parameterGroupTab1->addTab(parameterGroupFrame1, parameterGroup1->name());
        else {
          parameterGroupLayout->addWidget(parameterGroupFrame1, row1, 0);

          ++row1;
        }
      }
      else
        delete parameterGroupFrame1;

      //---

      ++row1;
    }

    //---

    if (! parameters.empty()) {
      addParameterEdits(parameters, plotData, parameterGroupLayout, row1, isBasic);

      if (row1 > 0)
        parameterGroupLayout->setRowStretch(row1, 1);
    }

    //---

    if (row1 > 0)
      parameterGroupTab->addTab(parameterGroupFrame, parameterGroup->name());
    else
      delete parameterGroupFrame;
  }

  //---

  CQChartsPlotType::Parameters parameters = type->nonGroupParameters();

  if (! parameters.empty())
    addParameterEdits(parameters, plotData, layout, row, isBasic);

  //---

  layout->setRowStretch(row, 1);
}

void
CQChartsCreatePlotDlg::
addParameterEdits(const CQChartsPlotType::Parameters &parameters, PlotData &plotData,
                  QGridLayout *layout, int &row, bool isBasic)
{
  // add column edits first
  int nstr  = 0;
  int nreal = 0;
  int nint  = 0;
  int nenum = 0;
  int nbool = 0;

  for (const auto &parameter : parameters) {
    if (parameter->isHidden())
      continue;

    if (isBasic && (! parameter->isRequired() && ! parameter->isBasic()))
      continue;

    plotData.names.insert(parameter->name());

    if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN ||
             parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST)
      addParameterEdit(plotData, layout, row, parameter, isBasic);
    else if (parameter->type() == CQChartsPlotParameter::Type::STRING)
      ++nstr;
    else if (parameter->type() == CQChartsPlotParameter::Type::REAL)
      ++nreal;
    else if (parameter->type() == CQChartsPlotParameter::Type::INTEGER)
      ++nint;
    else if (parameter->type() == CQChartsPlotParameter::Type::ENUM)
      ++nenum;
    else if (parameter->type() == CQChartsPlotParameter::Type::BOOLEAN)
      ++nbool;
    else
      assert(false);
  }

  // add string and real edits
  if (nstr > 0 || nreal > 0 || nint > 0) {
    auto *strLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

    for (const auto &parameter : parameters) {
      if (parameter->isHidden())
        continue;

      if (isBasic && (! parameter->isRequired() && ! parameter->isBasic()))
        continue;

      if (parameter->type() == CQChartsPlotParameter::Type::STRING ||
          parameter->type() == CQChartsPlotParameter::Type::REAL ||
          parameter->type() == CQChartsPlotParameter::Type::INTEGER)
        addParameterEdit(plotData, strLayout, parameter, isBasic);
    }

    strLayout->addStretch(1);

    if (! isBasic)
      layout->addLayout(strLayout, row, 0, 1, 5);
    else
      layout->addLayout(strLayout, row, 0, 1, 3);

    ++row;
  }

  // add enum edits
  if (nenum > 0) {
    auto *enumLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

    for (const auto &parameter : parameters) {
      if (parameter->isHidden())
        continue;

      if (isBasic && (! parameter->isRequired() && ! parameter->isBasic()))
        continue;

      if (parameter->type() == CQChartsPlotParameter::Type::ENUM)
        addParameterEdit(plotData, enumLayout, parameter, isBasic);
    }

    enumLayout->addStretch(1);

    if (! isBasic)
      layout->addLayout(enumLayout, row, 0, 1, 5);
    else
      layout->addLayout(enumLayout, row, 0, 1, 3);

    ++row;
  }

  // add bool edits
  if (nbool > 0) {
    auto *boolLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

    for (const auto &parameter : parameters) {
      if (parameter->isHidden())
        continue;

      if (isBasic && (! parameter->isRequired() && ! parameter->isBasic()))
        continue;

      if (parameter->type() == CQChartsPlotParameter::Type::BOOLEAN)
        addParameterEdit(plotData, boolLayout, parameter, isBasic);
    }

    boolLayout->addStretch(1);

    if (! isBasic)
      layout->addLayout(boolLayout, row, 0, 1, 5);
    else
      layout->addLayout(boolLayout, row, 0, 1, 3);

    ++row;
  }

  if (isBasic)
    layout->setColumnStretch(2, 1);
}

void
CQChartsCreatePlotDlg::
addParameterEdit(PlotData &plotData, QGridLayout *layout, int &row,
                 CQChartsPlotParameter *parameter, bool isBasic)
{
  if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN) {
    if (isBasic)
      addParameterBasicColumnEdit(plotData, layout, row, parameter);
    else
      addParameterColumnEdit(plotData, layout, row, parameter);
  }
  else if (parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST)
    addParameterColumnsEdit(plotData, layout, row, parameter, isBasic);
  else
    assert(false);
}

void
CQChartsCreatePlotDlg::
addParameterEdit(PlotData &plotData, QHBoxLayout *layout,
                 CQChartsPlotParameter *parameter, bool /*isBasic*/)
{
  if      (parameter->type() == CQChartsPlotParameter::Type::STRING)
    addParameterStringEdit(plotData, layout, parameter);
  else if (parameter->type() == CQChartsPlotParameter::Type::REAL)
    addParameterRealEdit(plotData, layout, parameter);
  else if (parameter->type() == CQChartsPlotParameter::Type::INTEGER)
    addParameterIntEdit(plotData, layout, parameter);
  else if (parameter->type() == CQChartsPlotParameter::Type::ENUM)
    addParameterEnumEdit(plotData, layout, parameter);
  else if (parameter->type() == CQChartsPlotParameter::Type::BOOLEAN)
    addParameterBoolEdit(plotData, layout, parameter);
  else
    assert(false);
}

void
CQChartsCreatePlotDlg::
addParameterBasicColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                            CQChartsPlotParameter *parameter)
{
  QString objName = parameter->name() + "Column";

  auto *label = CQUtil::makeLabelWidget<QLabel>(parameter->desc(), objName + "Label");

  auto *columnCombo = CQUtil::makeWidget<CQChartsColumnCombo>(objName + "Combo");

  CQChartsColumn column = parameter->defValue().value<CQChartsColumn>();

  columnCombo->setModelData(modelData_);
  columnCombo->setColumn   (column);

  plotData.columnEdits[parameter->name()].basicEdit = columnCombo;

  connect(columnCombo, SIGNAL(columnChanged()), this, SLOT(validateSlot()));

  QString tip = parameter->tip();

  if (tip.length())
    columnCombo->setToolTip(tip);

  //---

  layout->addWidget(label      , row, 0);
  layout->addWidget(columnCombo, row, 1);

  ++row;
}

void
CQChartsCreatePlotDlg::
addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                       CQChartsPlotParameter *parameter)
{
  // add column edit
  int col = 0;

  CQChartsColumnLineEdit *columnEdit =
    addColumnEdit(layout, row, col, parameter->desc(), parameter->name() + "Column",
                  "Column Name or Number");

  CQChartsColumn column = parameter->defValue().value<CQChartsColumn>();

  columnEdit->setModelData(modelData_);
  columnEdit->setColumn   (column);

  plotData.columnEdits[parameter->name()].advancedEdit = columnEdit;

  connect(columnEdit, SIGNAL(columnChanged()), this, SLOT(validateSlot()));

  QString tip = parameter->tip();

  if (tip.length())
    columnEdit->setToolTip(tip);

  //----

  // add format edit
  FormatEditData formatEditData;

  formatEditData.formatEdit =
    addStringEdit(layout, row, col, "", parameter->name() + "Format", "Column Format");

  formatEditData.formatEdit->setToolTip("Column format");

  connect(formatEditData.formatEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(validateSlot()));

  //--

  // add format update button
  formatEditData.formatUpdate = CQUtil::makeWidget<QToolButton>("formatUpdate");

  formatEditData.formatUpdate->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

  connect(formatEditData.formatUpdate, SIGNAL(clicked()),
          this, SLOT(updateFormatSlot()));

  formatEditData.formatUpdate->setToolTip("Get current column format");

  layout->addWidget(formatEditData.formatUpdate, row, col); ++col;

  plotData.formatEdits[parameter->name()] = formatEditData;

  //----

  // add attributes tip label
  QSize is = formatEditData.formatUpdate->iconSize();

  auto *attributesLabel = CQUtil::makeLabelWidget<QLabel>("", "attributesLabel");

  attributesLabel->setPixmap(CQPixmapCacheInst->getSizedPixmap("INFO", is));
  attributesLabel->setToolTip(parameter->attributes().summary());

  layout->addWidget(attributesLabel, row, col); ++col;

  //----

  ++row;

  //----

  // add mapping widgets
  if (parameter->isMapped()) {
    MapEditData mapEditData;

    col = 1;

    auto *mapLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

    //---

    // add map check
    mapEditData.mappedCheck = CQUtil::makeLabelWidget<QCheckBox>("Mapped", "mapped");

    connect(mapEditData.mappedCheck, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));

    mapEditData.mappedCheck->setToolTip("Remap column values from actual range to specific range");

    //--

    // add map min/max
    mapEditData.mapMinSpin = CQUtil::makeWidget<CQRealSpin>("mapMin");
    mapEditData.mapMaxSpin = CQUtil::makeWidget<CQRealSpin>("mapMax");

    connect(mapEditData.mapMinSpin, SIGNAL(valueChanged(double)), this, SLOT(validateSlot()));
    connect(mapEditData.mapMaxSpin, SIGNAL(valueChanged(double)), this, SLOT(validateSlot()));

    mapEditData.mapMinSpin->setToolTip("Min value for mapped values");
    mapEditData.mapMaxSpin->setToolTip("Max value for mapped values");

    //--

    mapLayout->addWidget(mapEditData.mappedCheck);
    mapLayout->addWidget(mapEditData.mapMinSpin);
    mapLayout->addWidget(mapEditData.mapMaxSpin);
    mapLayout->addStretch(1);

    plotData.mappedEdits[parameter->name()] = mapEditData;

    layout->addLayout(mapLayout, row, col, 1, 2);

    ++row;

    //---

    mapEditData.mappedCheck->setChecked(false);

    mapEditData.mapMinSpin->setValue(parameter->attributes().mapMin());
    mapEditData.mapMaxSpin->setValue(parameter->attributes().mapMax());
  }
}

void
CQChartsCreatePlotDlg::
addParameterColumnsEdit(PlotData &plotData, QGridLayout *layout, int &row,
                        CQChartsPlotParameter *parameter, bool isBasic)
{
  // add column edit
  int col = 0;

  CQChartsColumnsLineEdit *columnsEdit =
    addColumnsEdit(layout, row, col, parameter->desc(), parameter->name() + "Columns",
                   "Column Names or Numbers", isBasic);

  CQChartsColumns columns = parameter->defValue().value<CQChartsColumns>();

  columnsEdit->setModelData(modelData_);
  columnsEdit->setColumns  (columns);

  plotData.columnsEdits[parameter->name()] = columnsEdit;

  connect(columnsEdit, SIGNAL(columnsChanged()), this, SLOT(validateSlot()));

  QString tip = parameter->tip();

  if (tip.length())
    columnsEdit->setToolTip(tip);

  //----

  // add format edit
  if (! isBasic) {
    FormatEditData formatEditData;

    formatEditData.formatEdit =
      addStringEdit(layout, row, col, "", parameter->name() + "Format", "Columns Format");

    formatEditData.formatEdit->setToolTip("Columns format");

    connect(formatEditData.formatEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(validateSlot()));

    //--

    // add format update button
    formatEditData.formatUpdate = CQUtil::makeWidget<QToolButton>("formatUpdate");

    formatEditData.formatUpdate->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

    connect(formatEditData.formatUpdate, SIGNAL(clicked()),
            this, SLOT(updateFormatSlot()));

    formatEditData.formatUpdate->setToolTip("Get current column format");

    layout->addWidget(formatEditData.formatUpdate, row, col); ++col;

    plotData.formatEdits[parameter->name()] = formatEditData;

    //----

    // add attributes tip label
    QSize is = formatEditData.formatUpdate->iconSize();

    auto *attributesLabel = CQUtil::makeLabelWidget<QLabel>("", "attributesLabel");

    attributesLabel->setPixmap(CQPixmapCacheInst->getSizedPixmap("INFO", is));
    attributesLabel->setToolTip(parameter->attributes().summary());

    layout->addWidget(attributesLabel, row, col); ++col;
  }

  //----

  ++row;
}

void
CQChartsCreatePlotDlg::
addParameterStringEdit(PlotData &plotData, QHBoxLayout *layout,
                       CQChartsPlotParameter *parameter)
{
  bool ok;

  QString str = CQChartsVariant::toString(parameter->defValue(), ok);

  auto *editLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

  auto *label = CQUtil::makeLabelWidget<QLabel>(parameter->desc(), parameter->name() + "_label");

  auto *edit = CQUtil::makeWidget<CQChartsLineEdit>(parameter->name() + "_edit");

  edit->setText(str);

  editLayout->addWidget(label);
  editLayout->addWidget(edit);

  layout->addLayout(editLayout);

  plotData.stringEdits[parameter->name()] = edit;

  connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  QString tip = parameter->tip();

  if (tip.length())
    edit->setToolTip(tip);
}

void
CQChartsCreatePlotDlg::
addParameterRealEdit(PlotData &plotData, QHBoxLayout *layout,
                     CQChartsPlotParameter *parameter)
{
  bool ok;

  double r = CQChartsVariant::toReal(parameter->defValue(), ok);

  auto *editLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

  auto *label = CQUtil::makeLabelWidget<QLabel>(parameter->desc(), parameter->name() + "_label");

  editLayout->addWidget(label);

  CQRealSpin       *spinEdit = nullptr;
  CQChartsLineEdit *lineEdit = nullptr;

  QWidget *edit = nullptr;

  if (parameter->isRequired()) {
    spinEdit = CQUtil::makeWidget<CQRealSpin>("spinEdit");
    edit     = spinEdit;

    spinEdit->setValue(r);

    connect(spinEdit, SIGNAL(valueChanged(double)), this, SLOT(validateSlot()));
  }
  else {
    lineEdit = CQUtil::makeWidget<CQChartsLineEdit>("edit");
    edit     = lineEdit;

    lineEdit->setText(QString("%1").arg(r));

    connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));
  }

  edit->setObjectName(parameter->name() + "_edit");

  editLayout->addWidget(edit);

  layout->addLayout(editLayout);

  plotData.realEdits[parameter->name()] = edit;

  QString tip = parameter->tip();

  if (tip.length())
    edit->setToolTip(tip);
}

void
CQChartsCreatePlotDlg::
addParameterIntEdit(PlotData &plotData, QHBoxLayout *layout,
                    CQChartsPlotParameter *parameter)
{
  bool ok;

  long i = CQChartsVariant::toInt(parameter->defValue(), ok);

  auto *editLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

  auto *label = CQUtil::makeLabelWidget<QLabel>(parameter->desc(), parameter->name() + "_label");

  editLayout->addWidget(label);

  CQIntegerSpin    *spinEdit = nullptr;
  CQChartsLineEdit *lineEdit = nullptr;

  QWidget *edit = nullptr;

  if (parameter->isRequired()) {
    spinEdit = CQUtil::makeWidget<CQIntegerSpin>("spinEdit");
    edit     = spinEdit;

    spinEdit->setValue(int(i));

    connect(spinEdit, SIGNAL(valueChanged(int)), this, SLOT(validateSlot()));
  }
  else {
    lineEdit = CQUtil::makeWidget<CQChartsLineEdit>("edit");
    edit     = lineEdit;

    lineEdit->setText(QString("%1").arg(i));

    connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));
  }

  edit->setObjectName(parameter->name() + "_edit");

  editLayout->addWidget(edit);

  layout->addLayout(editLayout);

  plotData.intEdits[parameter->name()] = edit;

  QString tip = parameter->tip();

  if (tip.length())
    edit->setToolTip(tip);
}

void
CQChartsCreatePlotDlg::
addParameterEnumEdit(PlotData &plotData, QHBoxLayout *layout,
                     CQChartsPlotParameter *parameter)
{
  auto *eparameter = dynamic_cast<CQChartsEnumParameter *>(parameter);
  assert(eparameter);

  bool ok;

  long i = CQChartsVariant::toInt(eparameter->defValue(), ok);

  auto *editLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

  QLabel *label =
    CQUtil::makeLabelWidget<QLabel>(eparameter->desc(), eparameter->name() + "_label");

  editLayout->addWidget(label);

  auto *combo = CQUtil::makeWidget<QComboBox>(eparameter->name() + "_combo");

  QStringList names = eparameter->names();

  combo->addItems(names);

  combo->setCurrentIndex(combo->findText(eparameter->valueName(int(i))));

  connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(validateSlot()));

  editLayout->addWidget(combo);

  layout->addLayout(editLayout);

  plotData.enumEdits[eparameter->name()] = combo;

  QString tip = eparameter->tip();

  if (tip.length())
    combo->setToolTip(tip);
}

void
CQChartsCreatePlotDlg::
addParameterBoolEdit(PlotData &plotData, QHBoxLayout *layout,
                     CQChartsPlotParameter *parameter)
{
  bool b = parameter->defValue().toBool();

  auto *checkBox = CQUtil::makeLabelWidget<QCheckBox>(parameter->desc(), parameter->name());

  checkBox->setChecked(b);

  layout->addWidget(checkBox);

  plotData.boolEdits[parameter->name()] = checkBox;

  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));

  QString tip = parameter->tip();

  if (tip.length())
    checkBox->setToolTip(tip);
}

CQChartsLineEdit *
CQChartsCreatePlotDlg::
addRealEdit(QLayout *layout, int &row, int &column, const QString &name,
            const QString &objName, const QString &placeholderText) const
{
  QLabel *label = nullptr;

  if (name != "") {
    label = CQUtil::makeLabelWidget<QLabel>(name, objName + "Label");
  }

  auto *edit = CQUtil::makeWidget<CQChartsLineEdit>(objName + "Edit");

  edit->setPlaceholderText(placeholderText);

  auto *grid    = qobject_cast<QGridLayout *>(layout);
  auto *blayout = qobject_cast<QBoxLayout  *>(layout);

  if      (grid) {
    if (label) {
      grid->addWidget(label, row, column); ++column;
    }

    grid->addWidget(edit, row, column); ++column;
  }
  else if (blayout) {
    if (label)
      blayout->addWidget(label);

    blayout->addWidget(edit );
  }

  return edit;
}

CQChartsLineEdit *
CQChartsCreatePlotDlg::
addStringEdit(QLayout *layout, int &row, int &column, const QString &name,
              const QString &objName, const QString &placeholderText) const
{
  QLabel *label = nullptr;

  if (name != "")
    label = CQUtil::makeLabelWidget<QLabel>(name, objName + "Label");

  auto *edit = CQUtil::makeWidget<CQChartsLineEdit>(objName + "Edit");

  edit->setPlaceholderText(placeholderText);

  auto *grid    = qobject_cast<QGridLayout *>(layout);
  auto *blayout = qobject_cast<QBoxLayout  *>(layout);

  if      (grid) {
    if (label) {
      grid->addWidget(label, row, column); ++column;
    }

    grid->addWidget(edit, row, column); ++column;
  }
  else if (blayout) {
    if (label)
      blayout->addWidget(label);

    blayout->addWidget(edit);
  }

  return edit;
}

CQChartsColumnLineEdit *
CQChartsCreatePlotDlg::
addColumnEdit(QLayout *layout, int &row, int &column, const QString &name,
              const QString &objName, const QString &placeholderText) const
{
  QLabel *label = nullptr;

  if (name != "")
    label = CQUtil::makeLabelWidget<QLabel>(name, objName + "Label");

  auto *edit = CQUtil::makeWidget<CQChartsColumnLineEdit>(objName + "Edit" );

  edit->setPlaceholderText(placeholderText);

  auto *grid    = qobject_cast<QGridLayout *>(layout);
  auto *blayout = qobject_cast<QBoxLayout  *>(layout);

  if      (grid) {
    if (label) {
      grid->addWidget(label, row, column); ++column;
    }

    grid->addWidget(edit, row, column); ++column;
  }
  else if (blayout) {
    if (label)
      blayout->addWidget(label);

    blayout->addWidget(edit);
  }

  return edit;
}

CQChartsColumnsLineEdit *
CQChartsCreatePlotDlg::
addColumnsEdit(QLayout *layout, int &row, int &column, const QString &name,
               const QString &objName, const QString &placeholderText, bool isBasic) const
{
  QLabel *label = nullptr;

  if (name != "")
    label = CQUtil::makeLabelWidget<QLabel>(name, objName + "Label");

  auto *edit = new CQChartsColumnsLineEdit(nullptr, isBasic);
  edit->setObjectName(objName + "Edit" );

  edit->setPlaceholderText(placeholderText);

  auto *grid    = qobject_cast<QGridLayout *>(layout);
  auto *blayout = qobject_cast<QBoxLayout  *>(layout);

  if      (grid) {
    if (label) {
      grid->addWidget(label, row, column); ++column;
    }

    grid->addWidget(edit, row, column); ++column;
  }
  else if (blayout) {
    if (label)
      blayout->addWidget(label);

    blayout->addWidget(edit);
  }

  return edit;
}

void
CQChartsCreatePlotDlg::
comboSlot(const QString &desc)
{
  CQCharts::PlotTypes plotTypes;

  sortedPlotTypes(plotTypes);

  int ind = -1;

  int ind1 = 0;

  for (auto &plotType : plotTypes) {
    if (plotType->desc() == desc) {
      ind = ind1;
      break;
    }

    ++ind1;
  }

  if (ind >= 0) {
    basicPlotDataStack_   ->setCurrentIndex(ind);
    advancedPlotDataStack_->setCurrentIndex(ind);

    validateSlot();
  }
}

void
CQChartsCreatePlotDlg::
plotDataSlot(int state)
{
  advanced_ = (state != 0);

  plotDataStack_->setCurrentIndex(advanced_ ? 1 : 0);

  whereFrame_->setVisible(advanced_);

  validateSlot();
}

void
CQChartsCreatePlotDlg::
autoAnalyzeSlot(int state)
{
  setAutoAnalyzeModel(state);

  validateSlot();
}

void
CQChartsCreatePlotDlg::
xminSlot()
{
  setXYMin("xmin");
}

void
CQChartsCreatePlotDlg::
yminSlot()
{
  setXYMin("ymin");
}

void
CQChartsCreatePlotDlg::
xmaxSlot()
{
  setXYMin("xmax");
}

void
CQChartsCreatePlotDlg::
ymaxSlot()
{
  setXYMin("ymax");
}

void
CQChartsCreatePlotDlg::
setXYMin(const QString &id)
{
  if (! model_)
    return;

  auto *type = getPlotType();
  if (! type) return;

  auto &plotData = (isAdvanced() ? advancedTypePlotData_[type->name()] :
                                   basicTypePlotData_   [type->name()]);

  CQChartsColumn column;

  QString colName = (id == "xmin" || id == "xmax" ? type->xColumnName() : type->yColumnName());
  if (colName == "") return;

  for (const auto &parameter : type->parameters()) {
    if (parameter->isHidden())
      continue;

    if (plotData.names.find(parameter->name()) == plotData.names.end())
      continue;

    if (parameter->name() != colName)
      continue;

    if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN) {
      CQChartsColumn column1 = parameter->defValue().value<CQChartsColumn>();

      QString      columnTypeStr;
      MapValueData mapValueData;

      if (! parseParameterColumnEdit(parameter, plotData, column1, columnTypeStr, mapValueData))
        return;

      column = column1;
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST) {
      CQChartsColumns columns = parameter->defValue().value<CQChartsColumns>();

      QString columnTypeStr;

      if (! parseParameterColumnsEdit(parameter, plotData, columns, columnTypeStr))
        return;

      column = columns.column();
    }

    break;
  }

  if (! column.isValid())
    return;

  if (! modelData_)
    return;

  auto *details = modelData_->details();
  if (! details) return;

  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return;

  auto *columnDetails = details->columnDetails(column);
  if (! columnDetails) return;

  if      (id == "xmin") {
    QVariant xmin = columnDetails->minValue();

    rangeEditData_.xminEdit->setText(QString("%1").arg(xmin.toString()));
  }
  else if (id == "ymin") {
    QVariant ymin = columnDetails->minValue();

    rangeEditData_.yminEdit->setText(QString("%1").arg(ymin.toString()));
  }
  else if (id == "xmax") {
    QVariant xmax = columnDetails->maxValue();

    rangeEditData_.xmaxEdit->setText(QString("%1").arg(xmax.toString()));
  }
  else if (id == "ymax") {
    QVariant ymax = columnDetails->maxValue();

    rangeEditData_.ymaxEdit->setText(QString("%1").arg(ymax.toString()));
  }
}

void
CQChartsCreatePlotDlg::
autoPlaceSlot(int state)
{
  posEdit_->setEnabled(! state);
}

void
CQChartsCreatePlotDlg::
validateSlot()
{
  if (! initialized_)
    return;

  auto *type = getPlotType();
  if (! type) return;

  //---

  if (isAutoAnalyzeModel()) {
    bool isInitialized = false;

    if (isAdvanced())
      isInitialized = advancedTypeInitialzed_[type->description()];
    else
      isInitialized = basicTypeInitialzed_[type->description()];

    if (! isInitialized) {
      auto &plotData = (isAdvanced() ? advancedTypePlotData_[type->name()] :
                                       basicTypePlotData_   [type->name()]);

      CQChartsAnalyzeModel analyzeModel(charts_, modelData_);

      analyzeModel.analyzeType(type);

      const auto &analyzeModelData = analyzeModel.analyzeModelData(type);

      //---

      // init column edits
      for (const auto &nc : analyzeModelData.parameterNameColumn) {
        auto pe = plotData.columnEdits.find(nc.first);

        auto column = nc.second;

        if (pe != plotData.columnEdits.end()) {
          if (isAdvanced()) {
            auto *edit = (*pe).second.advancedEdit;

            CQChartsWidgetUtil::AutoDisconnect autoDisconnect(
              edit, SIGNAL(columnChanged()), this, SLOT(validateSlot()));

            edit->setColumn(column);
          }
          else {
            auto *edit = (*pe).second.basicEdit;

            CQChartsWidgetUtil::AutoDisconnect autoDisconnect(
              edit, SIGNAL(columnChanged()), this, SLOT(validateSlot()));

            edit->setColumn(column);
          }
        }
        else {
          auto pe = plotData.columnsEdits.find(nc.first);

          if (pe != plotData.columnsEdits.end()) {
            auto *edit = (*pe).second;

            CQChartsWidgetUtil::AutoDisconnect autoDisconnect(
              edit, SIGNAL(columnsChanged()), this, SLOT(validateSlot()));

            edit->setColumns(CQChartsColumns(column));
          }
        }
      }

      // init columns edits
      for (const auto &nc : analyzeModelData.parameterNameColumns) {
        auto pe = plotData.columnsEdits.find(nc.first);

        if (pe != plotData.columnsEdits.end()) {
          auto *edit = (*pe).second;

          CQChartsWidgetUtil::AutoDisconnect autoDisconnect(
            edit, SIGNAL(columnsChanged()), this, SLOT(validateSlot()));

          edit->setColumns(nc.second);
        }
      }

      // init bool edits
      for (const auto &nc : analyzeModelData.parameterNameBool) {
        auto pe = plotData.boolEdits.find(nc.first);

        if (pe != plotData.boolEdits.end()) {
          auto *edit = (*pe).second;

          CQChartsWidgetUtil::AutoDisconnect autoDisconnect(
            edit, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));

          edit->setChecked(nc.second);
        }
      }

      if (isAdvanced())
        advancedTypeInitialzed_[type->description()] = true;
      else
        basicTypeInitialzed_[type->description()] = true;
    }
  }

  //---

  // set description
  descText_->setText(type->description());

  // can only set x/y range if plot supports custom range
  rangeEditData_.xminEdit->setEnabled(type->customXRange());
  rangeEditData_.yminEdit->setEnabled(type->customYRange());
  rangeEditData_.xmaxEdit->setEnabled(type->customXRange());
  rangeEditData_.ymaxEdit->setEnabled(type->customYRange());

  // can only set range if plot type supplies which column refers to these axes
  QString xcolName = type->xColumnName();
  QString ycolName = type->yColumnName();

  rangeEditData_.xminButton->setEnabled(xcolName != "");
  rangeEditData_.yminButton->setEnabled(ycolName != "");
  rangeEditData_.xmaxButton->setEnabled(xcolName != "");
  rangeEditData_.ymaxButton->setEnabled(ycolName != "");

  titleEdit_ ->setEnabled(type->hasTitle());
  xLabelEdit_->setEnabled(type->hasXAxis());
  yLabelEdit_->setEnabled(type->hasYAxis());

  xintegralCheck_->setEnabled(type->hasXAxis() && type->allowXAxisIntegral());
  yintegralCheck_->setEnabled(type->hasYAxis() && type->allowYAxisIntegral());

  xlogCheck_->setEnabled(type->allowXLog());
  ylogCheck_->setEnabled(type->allowYLog());

  //---

  QStringList msgs;

  bool valid = validate(msgs);

  okButton_   ->setEnabled(valid);
  applyButton_->setEnabled(valid);

  if (! valid) {
    msgLabel_->setText(msgs.at(0));
    msgLabel_->setToolTip(msgs.join("\n"));
  }
  else {
    msgLabel_->setText(" ");
    msgLabel_->setToolTip("");
  }

  msgLabel_->setFixedHeight(msgLabel_->sizeHint().height());

  //---

  updatePreviewPlot(valid);
}

void
CQChartsCreatePlotDlg::
updatePreviewPlot(bool valid)
{
  if (valid && previewEnabledCheck_->isChecked()) {
    // create plot for typename of current tab
    auto *type = getPlotType();
    assert(type);

    ModelP previewModel;

    auto *summaryModel = modelData_->summaryModel();

    if (modelData_->isSummaryEnabled() && summaryModel)
      previewModel = modelData_->summaryModelP();
    else
      previewModel = model_;

    if (! previewPlot_ || previewPlot_->type() != type || previewPlot_->model() != previewModel) {
      previewView_->removeAllPlots();

      previewPlot_ = type->create(previewView_, previewModel);

      previewPlot_->setPreview(true);

      previewPlot_->setId("Preview");

      if (isAdvanced())
        previewPlot_->setFilterStr(whereEdit_->text());

      previewView_->addPlot(previewPlot_);

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
CQChartsCreatePlotDlg::
updateFormatSlot()
{
  auto *formatUpdate = qobject_cast<QToolButton *>(sender());
  if (! formatUpdate) return;

  auto *type = getPlotType();
  if (! type) return;

  auto &plotData = (isAdvanced() ? advancedTypePlotData_[type->name()] :
                                   basicTypePlotData_   [type->name()]);

  QString           parameterName;
  CQChartsLineEdit *formatEdit { nullptr };

  for (const auto &fe : plotData.formatEdits) {
    const auto &formatEditData = fe.second;

    if (formatUpdate == formatEditData.formatUpdate) {
      parameterName = fe.first;
      formatEdit    = formatEditData.formatEdit;
      break;
    }
  }

  if (! parameterName.length())
    return;

  CQChartsColumn column;

  auto pce = plotData.columnEdits.find(parameterName);

  if (pce != plotData.columnEdits.end()) {
    CQChartsColumn defColumn;

    if (isAdvanced()) {
      if (! columnLineEditValue((*pce).second, column, defColumn))
        return;
    }
    else {
    }
  }
  else {
    auto pce = plotData.columnsEdits.find(parameterName);

    if (pce != plotData.columnsEdits.end()) {
      CQChartsColumns columns;
      CQChartsColumns defColumns;

      if (! columnsLineEditValue((*pce).second, columns, defColumns))
        return;

      column = columns.column();
    }
  }

  QString typeStr;

  auto *summaryModel = modelData_->summaryModel();

  if (modelData_->isSummaryEnabled() && summaryModel) {
    if (! CQChartsModelUtil::columnTypeStr(charts_, summaryModel, column, typeStr))
      return;
  }
  else {
    if (! CQChartsModelUtil::columnTypeStr(charts_, model_.data(), column, typeStr))
      return;
  }

  if (formatEdit)
    formatEdit->setText(typeStr);
}

bool
CQChartsCreatePlotDlg::
validate(QStringList &msgs)
{
  msgs.clear();

  CQChartsModelData *modelData = nullptr;

  auto *summaryModel = modelData_->summaryModel();

  if (modelData_->isSummaryEnabled() && summaryModel)
    modelData = modelData_->summaryModelData();
  else
    modelData = modelData_;

  if (! modelData) { msgs << "no model data"; return false; }

  auto *details = modelData->details();

  //---

  // create plot for typename of current tab
  auto *type = getPlotType();

  if (! type) {
    msgs << "invalid plot type";
    return false;
  }

  // set plot property for widgets for plot parameters
  auto &plotData = (isAdvanced() ? advancedTypePlotData_[type->name()] :
                                   basicTypePlotData_   [type->name()]);

  bool rc = true;

  int num_valid = 0;
  int num_cols  = 0;

  for (const auto &parameter : type->parameters()) {
    if (parameter->isHidden())
      continue;

    if (plotData.names.find(parameter->name()) == plotData.names.end())
      continue;

    if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN) {
      ++num_cols;

      CQChartsColumn column = parameter->defValue().value<CQChartsColumn>();

      QString      columnTypeStr;
      MapValueData mapValueData;

      if (! parseParameterColumnEdit(parameter, plotData, column, columnTypeStr, mapValueData)) {
        if (parameter->isRequired()) {
          msgs << QString("missing required column value (%1)").arg(parameter->name());
          rc = false;
        }

        continue;
      }

      if (! column.isValid()) {
        msgs << QString("invalid column value (%1)").arg(parameter->name());
        rc = false;
        continue;
      }

      bool rc1 = true;

      if (column.type() == CQChartsColumn::Type::DATA ||
          column.type() == CQChartsColumn::Type::DATA_INDEX) {
        auto *columnDetails = details->columnDetails(column);
        assert(columnDetails);

        if (parameter->isMonotonic()) {
          if (! columnDetails->isMonotonic()) {
            msgs << QString("non-monotonic column (%1)").arg(parameter->name());
            rc1 = false;
          }
        }

        if      (parameter->isNumeric()) {
          if (columnDetails->type() != CQBaseModelType::INTEGER &&
              columnDetails->type() != CQBaseModelType::REAL &&
              columnDetails->type() != CQBaseModelType::TIME) {
            msgs << QString("non-numeric column (%1)").arg(parameter->name());
            rc1 = false;
          }
        }
        else if (parameter->isString()) {
#if 0
          if (columnDetails->type() != CQBaseModelType::STRING) {
            msgs << QString("non-string column (%1)").arg(parameter->name());
            rc1 = false;
          }
#endif
        }
        else if (parameter->isBool()) {
          if (columnDetails->type() != CQBaseModelType::BOOLEAN) {
            msgs << QString("non-bool column (%1)").arg(parameter->name());
            rc1 = false;
          }
        }
        else if (parameter->isColor()) {
          if (columnDetails->type() != CQBaseModelType::COLOR) {
            msgs << QString("non-color column (%1)").arg(parameter->name());
            rc1 = false;
          }
        }
      }

      if (rc1)
        ++num_valid;
      else
        rc = rc1;
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST) {
      ++num_cols;

      CQChartsColumns columns = parameter->defValue().value<CQChartsColumns>();

      QString columnTypeStr;

      if (! parseParameterColumnsEdit(parameter, plotData, columns, columnTypeStr)) {
        if (parameter->isRequired()) {
          msgs << QString("missing required columns value (%1)").arg(parameter->name());
          rc = false;
        }

        continue;
      }

      if (columns.isValid()) {
        ++num_valid;
      }
    }
  }

  if (num_valid == 0 && num_cols > 0) {
    msgs << "no columns specified";
    rc = false;
  }

  return rc;
}

void
CQChartsCreatePlotDlg::
previewEnabledSlot()
{
  validateSlot();
}

void
CQChartsCreatePlotDlg::
previewFitSlot()
{
  if (previewPlot_)
    previewPlot_->autoFit();
}

void
CQChartsCreatePlotDlg::
summaryEnabledSlot()
{
  if (modelData_)
    modelData_->setSummaryEnabled(summaryEditData_.enabledCheck->isChecked());

  validateSlot();
}

void
CQChartsCreatePlotDlg::
updateSummaryTypeSlot()
{
  summaryEditData_.typeStack->setCurrentIndex(summaryEditData_.typeCombo->currentIndex());

  updatePreviewSlot();
}

void
CQChartsCreatePlotDlg::
updatePreviewSlot()
{
  auto *summaryModel = modelData_->summaryModel();

  if (modelData_->isSummaryEnabled() && summaryModel) {
    CQChartsWidgetUtil::AutoDisconnect autoDisconnect(
      modelData_, SIGNAL(dataChanged()), this, SLOT(updateModelData()));

    int  n = summaryEditData_.maxRows->value();

    bool random = (summaryEditData_.typeCombo->currentText() == "Random");
    bool sorted = (summaryEditData_.typeCombo->currentText() == "Sorted");
    bool paged  = (summaryEditData_.typeCombo->currentText() == "Paged" );

    if (n <= 0) return;

    if (n != summaryModel->maxRows()) {
      summaryModel->setMode(CQSummaryModel::Mode::NORMAL);
      summaryModel->setMaxRows(n);
    }

    if      (random) {
      summaryModel->setRandomMode(true);
    }
    else if (sorted) {
      int sortCol = summaryEditData_.sortedColEdit->value();

      summaryModel->setSortColumn(sortCol);
      summaryModel->setSortMode(true);
    }
    else if (paged) {
      int ps = summaryEditData_.pageSizeEdit   ->value();
      int np = summaryEditData_.currentPageEdit->value();

      int nr = model_.data()->rowCount();

      int np1 = (nr + ps - 1)/ps;

      np = std::min(np, np1 - 1);

      summaryEditData_.currentPageEdit->setRange(0, np1 - 1);
      summaryEditData_.currentPageEdit->setToolTip(
        QString("Set Preview Page Count (0 -> %1)").arg(np1 - 1));

      summaryModel->setPageSize(ps);
      summaryModel->setCurrentPage(np);
      summaryModel->setPagedMode(true);
    }
    else {
      summaryModel->setMode(CQSummaryModel::Mode::NORMAL);
    }
  }

  validateSlot();
}

void
CQChartsCreatePlotDlg::
okSlot()
{
  if (applySlot())
    accept();
}

bool
CQChartsCreatePlotDlg::
applySlot()
{
  plot_ = nullptr;

  // create plot for typename of current tab
  auto *type = getPlotType();
  if (! type) return false;

  // get or create view
  QString viewId = viewEdit_->text();

  auto *view = charts()->getView(viewId);

  if (! view) {
    view = charts()->addView();

    auto *window = charts()->createWindow(view);

    window->show();

    viewEdit_->setText(view->id());
  }

  // create plot
  plot_ = type->create(view, model_);

  if (isAdvanced())
    plot_->setFilterStr(whereEdit_->text());

  //---

  applyPlot(plot_);

  //---

  double vr = CQChartsView::viewportRange();

  CQChartsGeom::BBox bbox(0, 0, vr, vr);

  if (! autoRangeEdit_->isChecked()) {
    double xmin = 0.0, ymin = 0.0, xmax = 1.0, ymax = 1.0;

    parsePosition(xmin, ymin, xmax, ymax);

    bbox = CQChartsGeom::BBox(vr*xmin, vr*ymin, vr*xmax, vr*ymax);
  }

  //---

  int n = view->numPlots();

  plot_->setId(QString("Chart.%1").arg(n + 1));

  //---

  view->addPlot(plot_, bbox);

  //---

  if (autoRangeEdit_->isChecked()) {
    view->autoPlacePlots();
  }

  return true;
}

CQChartsPlotType *
CQChartsCreatePlotDlg::
getPlotType() const
{
  int ind = basicPlotDataStack_->currentIndex();

  auto p = stackIndexPlotType_.find(ind);

  if (p == stackIndexPlotType_.end())
    return nullptr;

  auto *type = (*p).second;

  return type;
}

void
CQChartsCreatePlotDlg::
applyPlot(CQChartsPlot *plot, bool preview)
{
  ModelP model = plot->model();

  auto *type = plot->type();

  // set plot property for widgets for plot parameters
  auto &plotData = (isAdvanced() ? advancedTypePlotData_[type->name()] :
                                   basicTypePlotData_   [type->name()]);

  for (const auto &parameter : type->parameters()) {
    if (parameter->isHidden())
      continue;

    if (plotData.names.find(parameter->name()) == plotData.names.end())
      continue;

    if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN) {
      CQChartsColumn column = parameter->defValue().value<CQChartsColumn>();

      QString      columnTypeStr;
      MapValueData mapValueData;

      if (parseParameterColumnEdit(parameter, plotData, column, columnTypeStr, mapValueData)) {
        if (! plot->setParameter(parameter, column.toString()))
          charts()->errorMsg("Failed to set parameter '" + parameter->propName() + "'");

        if (columnTypeStr.length())
          CQChartsModelUtil::setColumnTypeStr(charts_, model.data(), column, columnTypeStr);

        if (parameter->isMapped()) {
          QString mappedPropName, mapMinPropName, mapMaxPropName;

          if (parameter->mapPropNames(mappedPropName, mapMinPropName, mapMaxPropName)) {
            if (! CQUtil::setProperty(plot, mappedPropName, QVariant(mapValueData.mapped)))
              charts()->errorMsg("Failed to set parameter '" + mappedPropName + "'");

            if (! CQUtil::setProperty(plot, mapMinPropName, QVariant(mapValueData.min)))
              charts()->errorMsg("Failed to set parameter '" + mapMinPropName + "'");

            if (! CQUtil::setProperty(plot, mapMaxPropName, QVariant(mapValueData.max)))
              charts()->errorMsg("Failed to set parameter '" + mapMaxPropName + "'");
          }
          else {
            charts()->errorMsg("Invalid column parameter name '" + parameter->propName() + "'");
          }
        }
      }
      else {
        if (parameter->isRequired()) {
          if (column.isValid())
            plot->setParameter(parameter, column.toString());
        }
        else {
          plot->setParameter(parameter, QVariant(-1));
        }
      }
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST) {
      CQChartsColumns columns = parameter->defValue().value<CQChartsColumns>();

      QString columnTypeStr;

      if (parseParameterColumnsEdit(parameter, plotData, columns, columnTypeStr)) {
        QString s = columns.toString();

        if (! plot->setParameter(parameter, QVariant(s)))
          charts()->errorMsg("Failed to set parameter '" + parameter->propName() + "'");

        if (columnTypeStr.length() && columns.isValid())
          CQChartsModelUtil::setColumnTypeStr(charts_, model.data(), columns.column(),
                                              columnTypeStr);
      }
      else {
        if (parameter->isRequired()) {
          if (columns.isValid())
            plot->setParameter(parameter, columns.toString());
        }
        else {
          plot->setParameter(parameter, QString());
        }
      }
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::STRING) {
      bool ok;

      QString defStr = CQChartsVariant::toString(parameter->defValue(), ok);

      QString str = defStr;

      if (parseParameterStringEdit(parameter, plotData, str)) {
        if (! plot->setParameter(parameter, QVariant(str)))
          charts()->errorMsg("Failed to set parameter '" + parameter->propName() + "'");
      }
      else {
        if (parameter->isRequired()) {
          if (ok)
            plot->setParameter(parameter, QVariant(defStr));
        }
        else {
          plot->setParameter(parameter, QString());
        }
      }
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::REAL) {
      bool ok;

      double defValue = CQChartsVariant::toReal(parameter->defValue(), ok);

      double r = defValue;

      if (parseParameterRealEdit(parameter, plotData, r)) {
        if (! plot->setParameter(parameter, QVariant(r)))
          charts()->errorMsg("Failed to set parameter '" + parameter->propName() + "'");
      }
      else {
        if (parameter->isRequired()) {
          if (ok)
            plot->setParameter(parameter, QVariant(defValue));
        }
      }
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::INTEGER) {
      bool ok;

      int defValue = (int) CQChartsVariant::toInt(parameter->defValue(), ok);

      int i = defValue;

      if (parseParameterIntEdit(parameter, plotData, i)) {
        if (! plot->setParameter(parameter, QVariant(i)))
          charts()->errorMsg("Failed to set parameter '" + parameter->propName() + "'");
      }
      else {
        if (parameter->isRequired()) {
          if (ok)
            plot->setParameter(parameter, QVariant(defValue));
        }
      }
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::ENUM) {
      bool ok;

      int defValue = (int) CQChartsVariant::toInt(parameter->defValue(), ok);

      int i = defValue;

      if (parseParameterEnumEdit(parameter, plotData, i)) {
        if (! plot->setParameter(parameter, QVariant(i)))
          charts()->errorMsg("Failed to set parameter '" + parameter->propName() + "'");
      }
      else {
        if (preview)
          plot->setParameter(parameter, QVariant(defValue));
      }
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::BOOLEAN) {
      bool defValue = parameter->defValue().toBool();

      bool b = defValue;

      if (parseParameterBoolEdit(parameter, plotData, b)) {
        if (! plot->setParameter(parameter, QVariant(b)))
          charts()->errorMsg("Failed to set parameter '" + parameter->propName() + "'");
      }
      else {
        if (preview)
          plot->setParameter(parameter, QVariant(defValue));
      }
    }
    else
      assert(false);
  }

  //---

  if (titleEdit_->text().length())
    plot->setTitleStr(titleEdit_->text());

  if (xLabelEdit_->text().length())
    plot->setXLabel(xLabelEdit_->text());

  if (yLabelEdit_->text().length())
    plot->setYLabel(yLabelEdit_->text());

  if (plot->xAxis())
    plot->xAxis()->setValueType(CQChartsAxisValueType::Type::REAL);

  if (plot->yAxis())
    plot->yAxis()->setValueType(CQChartsAxisValueType::Type::REAL);

  if (plot->xAxis() && type->allowXAxisIntegral()) {
    if (xintegralCheck_->isChecked())
      plot->xAxis()->setValueType(CQChartsAxisValueType::Type::INTEGER);
  }

  if (plot->yAxis() && type->allowYAxisIntegral()) {
    if (yintegralCheck_->isChecked())
      plot->yAxis()->setValueType(CQChartsAxisValueType::Type::INTEGER);
  }

  plot->setLogX(xlogCheck_->isChecked());
  plot->setLogY(ylogCheck_->isChecked());

  //---

  // set range
  bool xminOk = false, yminOk = false, xmaxOk = false, ymaxOk = false;

  if (rangeEditData_.xminEdit->text().length()) {
    double xmin = CQChartsUtil::toReal(rangeEditData_.xminEdit->text(), xminOk);
    if (xminOk) plot->setXMin(CQChartsOptReal(xmin));
  }

  if (rangeEditData_.yminEdit->text().length()) {
    double ymin = CQChartsUtil::toReal(rangeEditData_.yminEdit->text(), yminOk);
    if (yminOk) plot->setYMin(CQChartsOptReal(ymin));
  }

  if (rangeEditData_.xmaxEdit->text().length()) {
    double xmax = CQChartsUtil::toReal(rangeEditData_.xmaxEdit->text(), xmaxOk);
    if (xmaxOk) plot->setXMax(CQChartsOptReal(xmax));
  }

  if (rangeEditData_.ymaxEdit->text().length()) {
    double ymax = CQChartsUtil::toReal(rangeEditData_.ymaxEdit->text(), ymaxOk);
    if (ymaxOk) plot->setYMax(CQChartsOptReal(ymax));
  }

  if (preview) {
    if (! xminOk) plot->setXMin(CQChartsOptReal());
    if (! yminOk) plot->setYMin(CQChartsOptReal());
    if (! xmaxOk) plot->setXMax(CQChartsOptReal());
    if (! ymaxOk) plot->setYMax(CQChartsOptReal());
  }
}

void
CQChartsCreatePlotDlg::
cancelSlot()
{
  plot_ = nullptr;

  reject();
}

bool
CQChartsCreatePlotDlg::
parsePosition(double &xmin, double &ymin, double &xmax, double &ymax) const
{
  QString posStr = posEdit_->text();

  QStringList posStrs;

  if (! CQTcl::splitList(posStr, posStrs))
    return false;

  if (posStrs.length() == 4) {
    bool ok1; xmin = CQChartsUtil::toReal(posStrs[0], ok1); if (! ok1) xmin = 0.0;
    bool ok2; ymin = CQChartsUtil::toReal(posStrs[1], ok2); if (! ok2) ymin = 0.0;
    bool ok3; xmax = CQChartsUtil::toReal(posStrs[2], ok3); if (! ok3) xmax = 1.0;
    bool ok4; ymax = CQChartsUtil::toReal(posStrs[3], ok4); if (! ok4) ymax = 1.0;

    xmin = CMathUtil::clamp(xmin, 0.0, 1.0);
    ymin = CMathUtil::clamp(ymin, 0.0, 1.0);
    xmax = CMathUtil::clamp(xmax, 0.0, 1.0);
    ymax = CMathUtil::clamp(ymax, 0.0, 1.0);

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
CQChartsCreatePlotDlg::
parseParameterColumnEdit(CQChartsPlotParameter *parameter, const PlotData &plotData,
                         CQChartsColumn &column, QString &columnType,
                         MapValueData &mapValueData)
{
  // get format value
  if (isAdvanced()) {
    auto pf = plotData.formatEdits.find(parameter->name());
    assert(pf != plotData.formatEdits.end());

    const auto &formatEditData = (*pf).second;

    QString format = formatEditData.formatEdit->text();

    if (format != "")
      columnType = format;
  }

  //---

  // get column value
  CQChartsColumn defColumn = parameter->defValue().value<CQChartsColumn>();

  auto pe = plotData.columnEdits.find(parameter->name());
  assert(pe != plotData.columnEdits.end());

  if (! columnLineEditValue((*pe).second, column, defColumn))
    return false;

  //---

  // get mapped values
  if (isAdvanced()) {
    auto pm = plotData.mappedEdits.find(parameter->name());

    if (pm != plotData.mappedEdits.end()) {
      const auto &mapEditData = (*pm).second;

      mapValueData.mapped = mapEditData.mappedCheck->isChecked();
      mapValueData.min    = mapEditData.mapMinSpin ->value();
      mapValueData.max    = mapEditData.mapMaxSpin ->value();
    }
    else {
      mapValueData.mapped = false;
    }
  }
  else {
    mapValueData.mapped = false;
  }

  return true;
}

bool
CQChartsCreatePlotDlg::
parseParameterColumnsEdit(CQChartsPlotParameter *parameter, const PlotData &plotData,
                          CQChartsColumns &columns, QString &columnType)
{
  // get format value
  if (isAdvanced()) {
    auto pf = plotData.formatEdits.find(parameter->name());
    if (pf == plotData.formatEdits.end()) return true;

    const auto &formatEditData = (*pf).second;

    QString format = formatEditData.formatEdit->text();

    if (format != "")
      columnType = format;
  }

  //---

  // get columns value
  CQChartsColumns defColumns = parameter->defValue().value<CQChartsColumns>();

  auto pe = plotData.columnsEdits.find(parameter->name());
  assert(pe != plotData.columnsEdits.end());

  if (! columnsLineEditValue((*pe).second, columns, defColumns))
    return false;

  return true;
}

bool
CQChartsCreatePlotDlg::
parseParameterStringEdit(CQChartsPlotParameter *parameter, const PlotData &plotData,
                         QString &str)
{
  auto p = plotData.stringEdits.find(parameter->name());
  assert(p != plotData.stringEdits.end());

  str = (*p).second->text();

  return true;
}

bool
CQChartsCreatePlotDlg::
parseParameterRealEdit(CQChartsPlotParameter *parameter, const PlotData &plotData, double &r)
{
  auto p = plotData.realEdits.find(parameter->name());
  assert(p != plotData.realEdits.end());

  auto *edit = (*p).second;

  auto *spinEdit = qobject_cast<CQRealSpin       *>(edit);
  auto *lineEdit = qobject_cast<CQChartsLineEdit *>(edit);

  if (spinEdit)
    r = spinEdit->value();
  else {
    bool ok;

    r = CQChartsUtil::toReal(lineEdit->text(), ok);
  }

  return true;
}

bool
CQChartsCreatePlotDlg::
parseParameterIntEdit(CQChartsPlotParameter *parameter, const PlotData &plotData, int &i)
{
  auto p = plotData.intEdits.find(parameter->name());
  assert(p != plotData.intEdits.end());

  auto *edit = (*p).second;

  auto *spinEdit = qobject_cast<CQIntegerSpin    *>(edit);
  auto *lineEdit = qobject_cast<CQChartsLineEdit *>(edit);

  if (spinEdit)
    i = spinEdit->value();
  else {
    bool ok;

    i = (int) CQChartsUtil::toInt(lineEdit->text(), ok);
  }

  return true;
}

bool
CQChartsCreatePlotDlg::
parseParameterEnumEdit(CQChartsPlotParameter *parameter, const PlotData &plotData, int &i)
{
  auto *eparameter = dynamic_cast<CQChartsEnumParameter *>(parameter);
  assert(eparameter);

  auto p = plotData.enumEdits.find(eparameter->name());
  assert(p != plotData.enumEdits.end());

  i = eparameter->nameValue((*p).second->currentText());

  return true;
}

bool
CQChartsCreatePlotDlg::
parseParameterBoolEdit(CQChartsPlotParameter *parameter, const PlotData &plotData, bool &b)
{
  auto p = plotData.boolEdits.find(parameter->name());
  assert(p != plotData.boolEdits.end());

  b = (*p).second->isChecked();

  return true;
}

bool
CQChartsCreatePlotDlg::
columnLineEditValue(const ColumnEditData &editData, CQChartsColumn &column,
                    const CQChartsColumn &defColumn) const
{
  if (isAdvanced()) {
    if (! editData.advancedEdit)
      return false;

    column = editData.advancedEdit->column();
  }
  else {
    if (! editData.basicEdit)
      return false;

    column = editData.basicEdit->getColumn();
  }

  if (! column.isValid())
    column = defColumn;

  return column.isValid();
}

bool
CQChartsCreatePlotDlg::
columnsLineEditValue(CQChartsColumnsLineEdit *le, CQChartsColumns &columns,
                     const CQChartsColumns &defColumns) const
{
  columns = le->columns();

  if (! columns.isValid())
    columns = defColumns;

  return columns.isValid();
}

void
CQChartsCreatePlotDlg::
setViewName(const QString &viewName)
{
  if (viewEdit_)
    viewEdit_->setText(viewName);
}

int
CQChartsCreatePlotDlg::
exec()
{
  plot_ = nullptr;

  return QDialog::exec();
}
