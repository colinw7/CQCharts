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
#include <CQChartsModelViewHolder.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsAnalyzeModel.h>
#include <CQChartsWidgetUtil.h>
#include <CQCharts.h>

#include <CQSummaryModel.h>
#include <CQTabSplit.h>
#include <CQIntegerSpin.h>
#include <CQRealSpin.h>
#include <CQLineEdit.h>
#include <CQUtil.h>
#include <CQCustomCombo.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QStackedWidget>
#include <QTextEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
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

  model_ = modelData->currentModel();

  init();
}

void
CQChartsCreatePlotDlg::
init()
{
  auto createSep = [](const QString &name) -> QFrame * {
    QFrame *sep = CQUtil::makeWidget<QFrame>(name);

    sep->setFixedHeight(4);
    sep->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    return sep;
  };

  //---

  setObjectName("plotDlg");

  setWindowTitle(QString("Create Plot (Model %1)").arg(modelData_->ind()));
//setWindowIcon(QIcon()); TODO

  //----

  if (! CQChartsModelUtil::isHierarchical(model_.data())) {
    (void) modelData_->addSummaryModel();
  }

  //----

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //----

  CQTabSplit *area = CQUtil::makeWidget<CQTabSplit>("area");

  area->setOrientation(Qt::Vertical);

  layout->addWidget(area);

  //----

  QFrame *dataFrame = createDataFrame();

  area->addWidget(dataFrame, "Input Data");

  //----

  QFrame *summaryFrame = createSummaryFrame();

  area->addWidget(summaryFrame, "Summary");

  //----

  QFrame *previewFrame = createPreviewFrame();

  area->addWidget(previewFrame, "Preview");

  //----

  QFrame *sep1 = createSep("sep1");

  layout->addWidget(sep1);

  //----

  msgLabel_ = CQUtil::makeLabelWidget<QLabel>("", "msgLabel");

  layout->addWidget(msgLabel_);

  //----

  QFrame *sep2 = createSep("sep2");

  layout->addWidget(sep2);

  //-------

  // OK, Apply, Cancel Buttons
  CQChartsDialogButtons *buttons = new CQChartsDialogButtons(this);

  buttons->connect(this, SLOT(okSlot()), SLOT(applySlot()), SLOT(cancelSlot()));

  buttons->setToolTips("Create Plot and close dialog",
                       "Create plot and keep dialog open",
                       "Close dialog without creating plot");

  okButton_    = buttons->okButton();
  applyButton_ = buttons->applyButton();

  layout->addWidget(buttons);

  //--

  initialized_ = true;

  validateSlot();
}

QFrame *
CQChartsCreatePlotDlg::
createDataFrame()
{
  QFrame *dataFrame = CQUtil::makeWidget<QFrame>("data");

  QVBoxLayout *dataLayout = CQUtil::makeLayout<QVBoxLayout>(dataFrame, 2, 2);

  //--

  QCheckBox *autoAnalyzeCheck =
    CQUtil::makeLabelWidget<QCheckBox>("Auto Analyze", "autoAnalyzeCheck");

  autoAnalyzeCheck->setChecked(isAutoAnalyzeModel());
  autoAnalyzeCheck->setToolTip("Automatically set columnds from model data");

  connect(autoAnalyzeCheck, SIGNAL(stateChanged(int)), this, SLOT(autoAnalyzeSlot(int)));

  dataLayout->addWidget(autoAnalyzeCheck);

  //--

  QTabWidget *dataTab = CQUtil::makeWidget<QTabWidget>("dataTab");

  QFrame *typeFrame = createTypeDataFrame();

  dataTab->addTab(typeFrame, "Type");

  QFrame *genFrame = createGeneralDataFrame();

  dataTab->addTab(genFrame, "General");

  //---

  dataLayout->addWidget(dataTab);

  return dataFrame;
}

QFrame *
CQChartsCreatePlotDlg::
createTypeDataFrame()
{
  QFrame *typeFrame = CQUtil::makeWidget<QFrame>("type");

  QVBoxLayout *typeLayout = CQUtil::makeLayout<QVBoxLayout>(typeFrame, 2, 2);

  //--

  CQCharts::PlotTypes plotTypes;

  sortedPlotTypes(plotTypes);

  //---

  // type combo
  QHBoxLayout *typeComboLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

  typeLayout->addLayout(typeComboLayout);

  QLabel *typeLabel = CQUtil::makeLabelWidget<QLabel>("Type", "typeLabel");

  typeComboLayout->addWidget(typeLabel);

  CQCustomCombo *typeCombo = CQUtil::makeWidget<CQCustomCombo>("typeCombo");

  typeComboLayout->addWidget(typeCombo);

  typeCombo->setToolTip("Plot Type");

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

  typeComboLayout->addStretch(1);

  //----

  QTabWidget *typeTab = CQUtil::makeWidget<QTabWidget>("typeTab");

  typeLayout->addWidget(typeTab);

  // plot type widgets in stack (one per type)
  stack_ = CQUtil::makeWidget<QStackedWidget>("stack");

  for (std::size_t i = 0; i < plotTypes.size(); ++i)
    addPlotWidgets(plotTypes[i], i);

  typeTab->addTab(stack_, "Input Data");

  //---

  // plot type description
  descText_ = CQUtil::makeWidget<QTextEdit>("descText");

  typeTab->addTab(descText_, "Description");

  //----

  QHBoxLayout *whereLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

  typeLayout->addLayout(whereLayout);

  QLabel *whereLabel = CQUtil::makeLabelWidget<QLabel>("Where", "whereLabel");

  whereLayout->addWidget(whereLabel);

  whereEdit_ = CQUtil::makeWidget<CQLineEdit>("whereEdit");

  whereLayout->addWidget(whereEdit_);

  whereLayout->addStretch(1);

  whereEdit_->setToolTip("Filter for input data");

  //---

  return typeFrame;
}

void
CQChartsCreatePlotDlg::
sortedPlotTypes(CQCharts::PlotTypes &plotTypes1)
{
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

  //---

  plotTypes1.clear();

  for (auto &p1 : hierDimPlotsTypeMap) {
    const DimPlotTypesMap &dimPlotTypesMap = p1.second;

    for (auto &p2 : dimPlotTypesMap) {
      const CQCharts::PlotTypes &plotTypes = p2.second;

      for (auto &plotType : plotTypes)
        plotTypes1.push_back(plotType);
    }
  }
}

QFrame *
CQChartsCreatePlotDlg::
createGeneralDataFrame()
{
  QFrame *genFrame = CQUtil::makeWidget<QFrame>("general");

  QGridLayout *genLayout = CQUtil::makeLayout<QGridLayout>(genFrame, 2, 2);

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

  xLabelEdit_ = addStringEdit(genLayout, row, column, "X Label", "xlabel", "Label");

  xLabelEdit_->setToolTip("X Axis Label");

  connect(xLabelEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  yLabelEdit_ = addStringEdit(genLayout, row, column, "Y Label", "ylabel", "Label");

  yLabelEdit_->setToolTip("Y Axis Label");

  connect(yLabelEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  //----

  ++row; column = 0;

  QFrame *xRangeFrame = CQUtil::makeWidget<QFrame>("xRangeFrame");

  QHBoxLayout *xRangeFrameLayout = CQUtil::makeLayout<QHBoxLayout>(xRangeFrame, 0, 2);

  genLayout->addWidget(xRangeFrame, row, column, 1, 5);

  //--

  auto createButton = [&](const QString &name, const QString &iconName, const QString &tip,
                          const char *receiver) {
    QToolButton *button = CQUtil::makeWidget<QToolButton>(name);

    button->setIcon(CQPixmapCacheInst->getIcon(iconName));

    connect(button, SIGNAL(clicked()), this, receiver);

    button->setToolTip(tip);

    return button;
  };

  //--

  xminEdit_ =
    addRealEdit(xRangeFrameLayout, row, column, "X Min", "xmin", "Custom X Axis Minimum Value");
  xminButton_ =
    createButton("xminButton", "REFRESH", "Get minimum value for x column", SLOT(xminSlot()));

  xRangeFrameLayout->addWidget(xminButton_);

  yminEdit_ =
    addRealEdit(xRangeFrameLayout, row, column, "Y Min", "ymin", "Custom Y Axis Minimum Value");
  yminButton_ =
    createButton("yminButton", "REFRESH", "Get minimum value for y column", SLOT(yminSlot()));

  xRangeFrameLayout->addWidget(yminButton_);

  connect(xminEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));
  connect(yminEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  //--

  ++row; column = 0;

  QFrame *yRangeFrame = CQUtil::makeWidget<QFrame>("yRangeFrame");

  QHBoxLayout *yRangeFrameLayout = CQUtil::makeLayout<QHBoxLayout>(yRangeFrame, 0, 2);

  genLayout->addWidget(yRangeFrame, row, column, 1, 5);

  //--

  xmaxEdit_ =
    addRealEdit(yRangeFrameLayout, row, column, "X Max", "xmax", "Custom X Axis Maximum Value");
  xmaxButton_ =
    createButton("xmaxButton", "REFRESH", "Get maximum value for x column", SLOT(xmaxSlot()));

  yRangeFrameLayout->addWidget(xmaxButton_);

  ymaxEdit_ =
    addRealEdit(yRangeFrameLayout, row, column, "Y Max", "ymax", "Custom Y Axis Maximum Value");
  ymaxButton_ =
    createButton("ymaxButton", "REFRESH", "Get maximum value for y column", SLOT(ymaxSlot()));

  yRangeFrameLayout->addWidget(ymaxButton_);

  connect(xmaxEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));
  connect(ymaxEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(validateSlot()));

  //----

  ++row; column = 0;

  QFrame *xyFrame = CQUtil::makeWidget<QFrame>("xyFrame");

  QHBoxLayout *xyFrameLayout = CQUtil::makeLayout<QHBoxLayout>(xyFrame, 0, 2);

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

  QFrame *xySpacer = CQUtil::makeWidget<QFrame>("xySpacer");

  xySpacer->setFixedWidth(8);

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
createSummaryFrame()
{
  QFrame *summaryFrame = CQUtil::makeWidget<QFrame>("summary");

  summaryFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QVBoxLayout *summaryLayout = CQUtil::makeLayout<QVBoxLayout>(summaryFrame, 0, 2);

  //--

  CQSummaryModel *summaryModel = modelData_->summaryModel();

  if (summaryModel) {
    int nr = model_.data()->rowCount();
    int nc = model_.data()->columnCount();

    QFrame *summaryControl = CQUtil::makeWidget<QFrame>("summaryControl");

    summaryControl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *summaryControlLayout = CQUtil::makeLayout<QHBoxLayout>(summaryControl, 2, 2);

    //---

    summaryEnabledCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Summary", "summaryEnabled");

    summaryEnabledCheck_->setChecked(modelData_->isSummaryEnabled());

    connect(summaryEnabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(summaryEnabledSlot()));

    summaryControlLayout->addWidget(summaryEnabledCheck_);

    //---

    summaryMaxRows_ = CQUtil::makeWidget<CQIntegerSpin>("summaryMaxRows");

    summaryMaxRows_->setRange(1, nr);
    summaryMaxRows_->setToolTip(QString("Set Preview Row Count (1 -> %1)").arg(nr));

    if (summaryModel)
      summaryMaxRows_->setValue(summaryModel->maxRows());

    connect(summaryMaxRows_, SIGNAL(valueChanged(int)), this, SLOT(updatePreviewSlot()));

    summaryControlLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Max Rows", "maxRows"));
    summaryControlLayout->addWidget(summaryMaxRows_);

    //---

    summaryTypeCombo_ = CQUtil::makeWidget<QComboBox>("summaryTypeCombo");

    summaryTypeCombo_->addItems(QStringList() << "Normal" << "Random" << "Sorted" << "Paged");

    connect(summaryTypeCombo_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateSummaryTypeSlot()));

    summaryControlLayout->addWidget(summaryTypeCombo_);

    //----

    summaryTypeStack_ = CQUtil::makeWidget<QStackedWidget>("summaryTypeStack");

    summaryControlLayout->addWidget(summaryTypeStack_);

    //---

    QFrame *normalTypeFrame = CQUtil::makeWidget<QFrame>("normalTypeFrame");

    summaryTypeStack_->addWidget(normalTypeFrame);

    //---

    QFrame *randomTypeFrame = CQUtil::makeWidget<QFrame>("randomTypeFrame");

    summaryTypeStack_->addWidget(randomTypeFrame);

    //---

    QFrame *sortedTypeFrame = CQUtil::makeWidget<QFrame>("sortedTypeFrame");

    summaryTypeStack_->addWidget(sortedTypeFrame);

    QHBoxLayout *sortedTypeLayout = CQUtil::makeLayout<QHBoxLayout>(sortedTypeFrame, 0, 2);

    //--

    summarySortedColEdit_ = CQUtil::makeWidget<CQIntegerSpin>("summarySortedColEdit");

    summarySortedColEdit_->setRange(0, nc - 1);
    summarySortedColEdit_->setToolTip(QString("Set Preview Sort Column (0 -> %1)").arg(nc - 1));

    connect(summarySortedColEdit_, SIGNAL(valueChanged(int)), this, SLOT(updatePreviewSlot()));

    sortedTypeLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Sort Column", "sortCol"));
    sortedTypeLayout->addWidget(summarySortedColEdit_);

    sortedTypeLayout->addStretch(1);

    //---

    QFrame *pageSizeTypeFrame = CQUtil::makeWidget<QFrame>("pageSizeTypeFrame");

    summaryTypeStack_->addWidget(pageSizeTypeFrame);

    QHBoxLayout *pageSizeTypeLayout = CQUtil::makeLayout<QHBoxLayout>(pageSizeTypeFrame, 0, 2);

    //--

    summaryPageSizeEdit_ = CQUtil::makeWidget<CQIntegerSpin>("summaryPageSizeEdit");

    summaryPageSizeEdit_->setRange(1, nr);
    summaryPageSizeEdit_->setValue(100);
    summaryPageSizeEdit_->setToolTip(QString("Set Preview Page Size (1 -> %1)").arg(nr));

    connect(summaryPageSizeEdit_, SIGNAL(valueChanged(int)), this, SLOT(updatePreviewSlot()));

    pageSizeTypeLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Page Size", "pointSize"));
    pageSizeTypeLayout->addWidget(summaryPageSizeEdit_);

    //--

    summaryCurrentPageEdit_ = CQUtil::makeWidget<CQIntegerSpin>("summaryCurrentPageEdit");

    int np = (nr + summaryPageSizeEdit_->value() - 1)/summaryPageSizeEdit_->value();

    summaryCurrentPageEdit_->setRange(0, np - 1);
    summaryCurrentPageEdit_->setToolTip(QString("Set Preview Page Count (0 -> %1)").arg(np - 1));

    connect(summaryCurrentPageEdit_, SIGNAL(valueChanged(int)), this, SLOT(updatePreviewSlot()));

    pageSizeTypeLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Current Page", "currentPage"));
    pageSizeTypeLayout->addWidget(summaryCurrentPageEdit_);

    pageSizeTypeLayout->addStretch(1);

    //---

    summaryControlLayout->addStretch(1);

    summaryLayout->addWidget(summaryControl);
  }

  //--

  summaryModelView_ = new CQChartsModelViewHolder(charts_);

  if (modelData_->isSummaryEnabled() && summaryModel) {
    ModelP summaryModelP = modelData_->summaryModelP();

    summaryModelView_->setModel(summaryModelP, CQChartsModelUtil::isHierarchical(summaryModel));
  }
  else
    summaryModelView_->setModel(model_, CQChartsModelUtil::isHierarchical(model_.data()));

  summaryLayout->addWidget(summaryModelView_);

  //--

  return summaryFrame;
}

QFrame *
CQChartsCreatePlotDlg::
createPreviewFrame()
{
  QFrame *previewFrame = CQUtil::makeWidget<QFrame>("preview");

  QVBoxLayout *previewLayout = CQUtil::makeLayout<QVBoxLayout>(previewFrame, 0, 2);

  //--

  QFrame *previewControl = CQUtil::makeWidget<QFrame>("previewControl");

  QHBoxLayout *previewControlLayout = CQUtil::makeLayout<QHBoxLayout>(previewControl, 2, 2);

  previewEnabledCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Enabled", "previewEnabled");

  previewEnabledCheck_->setToolTip("Enable plot preview");

  connect(previewEnabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(previewEnabledSlot()));

  previewControlLayout->addWidget(previewEnabledCheck_);

  previewControlLayout->addStretch(1);

  previewLayout->addWidget(previewControl);

  //--

  //QTabWidget *previewTab = CQUtil::makeWidget<QTabWidget>("previewTab");

  //previewLayout->addWidget(previewTab);

  //--

  previewView_ = charts_->createView();

  previewView_->setPreview(true);

  previewLayout->addWidget(previewView_);

  //----

  return previewFrame;
}

CQChartsCreatePlotDlg::
~CQChartsCreatePlotDlg()
{
}

void
CQChartsCreatePlotDlg::
addPlotWidgets(CQChartsPlotType *type, int ind)
{
  QFrame *frame = CQUtil::makeWidget<QFrame>(type->name() + "_frame");

  stack_->addWidget(frame);

  //---

  QGridLayout *frameLayout = CQUtil::makeLayout<QGridLayout>(frame, 2, 2);

  int row = 0;

  PlotData &plotData = typePlotData_[type->name()];

  addParameterEdits(type, plotData, frameLayout, row);

  //---

  plotData.ind = ind;

  tabType_[plotData.ind] = type;
}

void
CQChartsCreatePlotDlg::
addParameterEdits(CQChartsPlotType *type, PlotData &plotData, QGridLayout *layout, int &row)
{
  using GroupTab = std::map<int,QTabWidget*>;

  using ChildGroups = std::vector<CQChartsPlotParameterGroup *>;

  GroupTab    groupTab;
  ChildGroups childGroups;

  for (const auto &p : type->parameterGroups()) {
    CQChartsPlotParameterGroup *parameterGroup = p.second;

    if (parameterGroup->isHidden())
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
      QTabWidget *parameterGroupTab = CQUtil::makeWidget<QTabWidget>("parameterGroupTab");

      layout->addWidget(parameterGroupTab, row, 0, 1, 5);

      ++row;

      pg = groupTab.insert(pg, GroupTab::value_type(-1, parameterGroupTab));
    }

    QTabWidget *parameterGroupTab = (*pg).second;

    //---

    // create frame for widgets and add to tab widget
    QFrame *parameterGroupFrame = CQUtil::makeWidget<QFrame>(parameterGroup->name());

    QGridLayout *parameterGroupLayout =
      CQUtil::makeLayout<QGridLayout>(parameterGroupFrame, 2, 2);

    parameterGroupTab->addTab(parameterGroupFrame, parameterGroup->name());

    int row1 = 0;

    //---

    for (const auto &parameterGroup1 : parameterGroups) {
      if (parameterGroup1->isHidden())
        continue;

      CQChartsPlotType::Parameters parameters1 =
        type->groupParameters(parameterGroup1->groupId());

      CQChartsPlotType::ParameterGroups parameterGroups1 =
        type->groupParameterGroups(parameterGroup1->groupId());

      assert(! parameters1.empty() || ! parameterGroups1.empty());

      //---

      // get tab widget
      auto pg = groupTab.find(parameterGroup->groupId());

      if (pg == groupTab.end()) {
        QTabWidget *parameterGroupTab1 = CQUtil::makeWidget<QTabWidget>("parameterGroupTab");

        parameterGroupLayout->addWidget(parameterGroupTab1, row, 0);

        ++row;

        pg = groupTab.insert(pg,
          GroupTab::value_type(parameterGroup->groupId(), parameterGroupTab1));
      }

      QTabWidget *parameterGroupTab1 = (*pg).second;

      //---

      int row2 = 0;

      QFrame *parameterGroupFrame1 = CQUtil::makeWidget<QFrame>(parameterGroup1->name());

      QGridLayout *parameterGroupLayout1 =
        CQUtil::makeLayout<QGridLayout>(parameterGroupFrame1, 2, 2);

      parameterGroupTab1->addTab(parameterGroupFrame1, parameterGroup1->name());

      if (! parameters1.empty()) {
        addParameterEdits(parameters1, plotData, parameterGroupLayout1, row2);

        parameterGroupLayout1->setRowStretch(row2, 1);
      }

      //---

      ++row1;
    }

    //---

    if (! parameters.empty()) {
      addParameterEdits(parameters, plotData, parameterGroupLayout, row1);

      parameterGroupLayout->setRowStretch(row1, 1);
    }
  }

  //---

  CQChartsPlotType::Parameters parameters = type->nonGroupParameters();

  if (parameters.empty())
    addParameterEdits(parameters, plotData, layout, row);

  //---

  layout->setRowStretch(row, 1);
}

void
CQChartsCreatePlotDlg::
addParameterEdits(const CQChartsPlotType::Parameters &parameters, PlotData &plotData,
                  QGridLayout *layout, int &row)
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

    if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN ||
             parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST)
      addParameterEdit(plotData, layout, row, parameter);
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
    QHBoxLayout *strLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

    for (const auto &parameter : parameters) {
      if (parameter->isHidden())
        continue;

      if (parameter->type() == CQChartsPlotParameter::Type::STRING ||
          parameter->type() == CQChartsPlotParameter::Type::REAL ||
          parameter->type() == CQChartsPlotParameter::Type::INTEGER)
        addParameterEdit(plotData, strLayout, parameter);
    }

    strLayout->addStretch(1);

    layout->addLayout(strLayout, row, 0, 1, 5);

    ++row;
  }

  // add enum edits
  if (nenum > 0) {
    QHBoxLayout *enumLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

    for (const auto &parameter : parameters) {
      if (parameter->isHidden())
        continue;

      if (parameter->type() == CQChartsPlotParameter::Type::ENUM)
        addParameterEdit(plotData, enumLayout, parameter);
    }

    enumLayout->addStretch(1);

    layout->addLayout(enumLayout, row, 0, 1, 5);

    ++row;
  }

  // add bool edits
  if (nbool > 0) {
    QHBoxLayout *boolLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

    for (const auto &parameter : parameters) {
      if (parameter->isHidden())
        continue;

      if (parameter->type() == CQChartsPlotParameter::Type::BOOLEAN)
        addParameterEdit(plotData, boolLayout, parameter);
    }

    boolLayout->addStretch(1);

    layout->addLayout(boolLayout, row, 0, 1, 5);

    ++row;
  }
}

void
CQChartsCreatePlotDlg::
addParameterEdit(PlotData &plotData, QGridLayout *layout, int &row,
                 CQChartsPlotParameter *parameter)
{
  if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN)
    addParameterColumnEdit(plotData, layout, row, parameter);
  else if (parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST)
    addParameterColumnsEdit(plotData, layout, row, parameter);
  else
    assert(false);
}

void
CQChartsCreatePlotDlg::
addParameterEdit(PlotData &plotData, QHBoxLayout *layout,
                 CQChartsPlotParameter *parameter)
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
addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                       CQChartsPlotParameter *parameter)
{
  int col = 0;

  CQChartsColumnLineEdit *columnEdit =
    addColumnEdit(layout, row, col, parameter->desc(), parameter->name() + "Column",
                  "Column Name or Number");

  CQChartsColumn column = parameter->defValue().value<CQChartsColumn>();

  columnEdit->setModel (model_.data());
  columnEdit->setColumn(column);

  plotData.columnEdits[parameter->name()] = columnEdit;

  connect(columnEdit, SIGNAL(columnChanged()), this, SLOT(validateSlot()));

  QString tip = parameter->tip();

  if (tip.length())
    columnEdit->setToolTip(tip);

  //----

  FormatEditData formatEditData;

  formatEditData.formatEdit =
    addStringEdit(layout, row, col, "", parameter->name() + "Format", "Column Format");

  formatEditData.formatEdit->setToolTip("Column format");

  connect(formatEditData.formatEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(validateSlot()));

  //--

  formatEditData.formatUpdate = CQUtil::makeWidget<QToolButton>("formatUpdate");

  formatEditData.formatUpdate->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

  connect(formatEditData.formatUpdate, SIGNAL(clicked()),
          this, SLOT(updateFormatSlot()));

  formatEditData.formatUpdate->setToolTip("Get current column format");

  layout->addWidget(formatEditData.formatUpdate, row, col); ++col;

  plotData.formatEdits[parameter->name()] = formatEditData;

  //----

  QSize is = formatEditData.formatUpdate->iconSize();

  QLabel *attributesLabel = CQUtil::makeLabelWidget<QLabel>("", "attributesLabel");

  attributesLabel->setPixmap(CQPixmapCacheInst->getSizedPixmap("INFO", is));
  attributesLabel->setToolTip(parameter->attributes().summary());

  layout->addWidget(attributesLabel, row, col); ++col;

  //----

  ++row;

  //----

  if (parameter->isMapped()) {
    MapEditData mapEditData;

    col = 1;

    QHBoxLayout *mapLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

    mapEditData.mappedCheck = CQUtil::makeLabelWidget<QCheckBox>("Mapped", "mapped");

    connect(mapEditData.mappedCheck, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));

    mapEditData.mappedCheck->setToolTip("Remap column values from actual range to specific range");

    //--

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
                        CQChartsPlotParameter *parameter)
{
  int column = 0;

  CQChartsColumnsLineEdit *columnsEdit =
    addColumnsEdit(layout, row, column, parameter->desc(), parameter->name() + "Columns",
                   "Column Names or Numbers");

  CQChartsColumns columns = parameter->defValue().value<CQChartsColumns>();

  columnsEdit->setModel  (model_.data());
  columnsEdit->setColumns(columns);

  plotData.columnsEdits[parameter->name()] = columnsEdit;

  connect(columnsEdit, SIGNAL(columnsChanged()), this, SLOT(validateSlot()));

  QString tip = parameter->tip();

  if (tip.length())
    columnsEdit->setToolTip(tip);

  //----

  FormatEditData formatEditData;

  formatEditData.formatEdit =
    addStringEdit(layout, row, column, "", parameter->name() + "Format", "Columns Format");

  formatEditData.formatEdit->setToolTip("Columns format");

  connect(formatEditData.formatEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(validateSlot()));

  //--

  formatEditData.formatUpdate = CQUtil::makeWidget<QToolButton>("formatUpdate");

  formatEditData.formatUpdate->setIcon(CQPixmapCacheInst->getIcon("REFRESH"));

  connect(formatEditData.formatUpdate, SIGNAL(clicked()),
          this, SLOT(updateFormatSlot()));

  formatEditData.formatUpdate->setToolTip("Get current column format");

  layout->addWidget(formatEditData.formatUpdate, row, column); ++column;

  plotData.formatEdits[parameter->name()] = formatEditData;

  //----

  QSize is = formatEditData.formatUpdate->iconSize();

  QLabel *attributesLabel = CQUtil::makeLabelWidget<QLabel>("", "attributesLabel");

  attributesLabel->setPixmap(CQPixmapCacheInst->getSizedPixmap("INFO", is));
  attributesLabel->setToolTip(parameter->attributes().summary());

  layout->addWidget(attributesLabel, row, column); ++column;

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

  QHBoxLayout *editLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

  QLabel *label = CQUtil::makeLabelWidget<QLabel>(parameter->desc(), parameter->name() + "_label");

  CQLineEdit *edit = CQUtil::makeWidget<CQLineEdit>(parameter->name() + "_edit");

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

  QHBoxLayout *editLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

  QLabel *label = CQUtil::makeLabelWidget<QLabel>(parameter->desc(), parameter->name() + "_label");

  editLayout->addWidget(label);

  CQRealSpin *spinEdit = nullptr;
  CQLineEdit *lineEdit = nullptr;

  QWidget *edit = nullptr;

  if (parameter->isRequired()) {
    spinEdit = CQUtil::makeWidget<CQRealSpin>("spinEdit");
    edit     = spinEdit;

    spinEdit->setValue(r);

    connect(spinEdit, SIGNAL(valueChanged(double)), this, SLOT(validateSlot()));
  }
  else {
    lineEdit = CQUtil::makeWidget<CQLineEdit>("edit");
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

  QHBoxLayout *editLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

  QLabel *label = CQUtil::makeLabelWidget<QLabel>(parameter->desc(), parameter->name() + "_label");

  editLayout->addWidget(label);

  CQIntegerSpin *spinEdit = nullptr;
  CQLineEdit    *lineEdit = nullptr;

  QWidget *edit = nullptr;

  if (parameter->isRequired()) {
    spinEdit = CQUtil::makeWidget<CQIntegerSpin>("spinEdit");
    edit     = spinEdit;

    spinEdit->setValue(i);

    connect(spinEdit, SIGNAL(valueChanged(int)), this, SLOT(validateSlot()));
  }
  else {
    lineEdit = CQUtil::makeWidget<CQLineEdit>("edit");
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
  CQChartsEnumParameter *eparameter = dynamic_cast<CQChartsEnumParameter *>(parameter);
  assert(eparameter);

  bool ok;

  long i = CQChartsVariant::toInt(eparameter->defValue(), ok);

  QHBoxLayout *editLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 0, 2);

  QLabel *label =
    CQUtil::makeLabelWidget<QLabel>(eparameter->desc(), eparameter->name() + "_label");

  editLayout->addWidget(label);

  QComboBox *combo = CQUtil::makeWidget<QComboBox>(eparameter->name() + "_combo");

  QStringList names = eparameter->names();

  combo->addItems(names);

  combo->setCurrentIndex(combo->findText(eparameter->valueName(i)));

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

  QCheckBox *checkBox = CQUtil::makeLabelWidget<QCheckBox>(parameter->desc(), parameter->name());

  checkBox->setChecked(b);

  layout->addWidget(checkBox);

  plotData.boolEdits[parameter->name()] = checkBox;

  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(validateSlot()));

  QString tip = parameter->tip();

  if (tip.length())
    checkBox->setToolTip(tip);
}

CQLineEdit *
CQChartsCreatePlotDlg::
addRealEdit(QLayout *layout, int &row, int &column, const QString &name,
            const QString &objName, const QString &placeholderText) const
{
  QLabel *label = nullptr;

  if (name != "") {
    label = CQUtil::makeLabelWidget<QLabel>(name, objName + "Label");
  }

  CQLineEdit *edit = CQUtil::makeWidget<CQLineEdit>(objName + "Edit");

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

CQLineEdit *
CQChartsCreatePlotDlg::
addStringEdit(QLayout *layout, int &row, int &column, const QString &name,
              const QString &objName, const QString &placeholderText) const
{
  QLabel *label = nullptr;

  if (name != "") {
    label = CQUtil::makeLabelWidget<QLabel>(name, objName + "Label");
  }

  CQLineEdit *edit = CQUtil::makeWidget<CQLineEdit>(objName + "Edit");

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

CQChartsColumnLineEdit *
CQChartsCreatePlotDlg::
addColumnEdit(QLayout *layout, int &row, int &column, const QString &name,
              const QString &objName, const QString &placeholderText) const
{
  QLabel *label = nullptr;

  if (name != "") {
    label = CQUtil::makeLabelWidget<QLabel>(name, objName + "Label");
  }

  CQChartsColumnLineEdit *edit = CQUtil::makeWidget<CQChartsColumnLineEdit>(objName + "Edit" );

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

CQChartsColumnsLineEdit *
CQChartsCreatePlotDlg::
addColumnsEdit(QLayout *layout, int &row, int &column, const QString &name,
               const QString &objName, const QString &placeholderText) const
{
  QLabel *label = nullptr;

  if (name != "") {
    label = CQUtil::makeLabelWidget<QLabel>(name, objName + "Label");
  }

  CQChartsColumnsLineEdit *edit = CQUtil::makeWidget<CQChartsColumnsLineEdit>(objName + "Edit" );

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
    stack_->setCurrentIndex(ind);

    validateSlot();
  }
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

  CQChartsPlotType *type = getPlotType();
  if (! type) return;

  PlotData &plotData = typePlotData_[type->name()];

  CQChartsColumn column;

  QString colName = (id == "xmin" || id == "xmax" ? type->xColumnName() : type->yColumnName());
  if (colName == "") return;

  for (const auto &parameter : type->parameters()) {
    if (parameter->isHidden())
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

  CQChartsModelDetails *details = modelData_->details();
  if (! details) return;

  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return;

  const CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);
  if (! columnDetails) return;

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
CQChartsCreatePlotDlg::
validateSlot()
{
  if (! initialized_)
    return;

  CQChartsPlotType *type = getPlotType();

  if (! type)
    return;

  //---

  if (isAutoAnalyzeModel()) {
    if (! typeInitialzed_[type->description()]) {
      PlotData &plotData = typePlotData_[type->name()];

      CQChartsAnalyzeModel analyzeModel(charts_, modelData_);

      analyzeModel.analyzeType(type);

      for (const auto &tnc : analyzeModel.typeNameColumns()) {
        for (const auto &nc : tnc.second) {
          auto pe = plotData.columnEdits.find(nc.first);

          if (pe != plotData.columnEdits.end()) {
            CQChartsColumnLineEdit *edit = (*pe).second;

            disconnect(edit, SIGNAL(columnChanged()), this, SLOT(validateSlot()));

            edit->setColumn(nc.second);

            connect(edit, SIGNAL(columnChanged()), this, SLOT(validateSlot()));
          }
          else {
            auto pe = plotData.columnsEdits.find(nc.first);

            if (pe != plotData.columnsEdits.end()) {
              CQChartsColumnsLineEdit *edit = (*pe).second;

              disconnect(edit, SIGNAL(columnsChanged()), this, SLOT(validateSlot()));

              edit->setColumns(nc.second);

              connect(edit, SIGNAL(columnsChanged()), this, SLOT(validateSlot()));
            }
          }
        }
      }

      typeInitialzed_[type->description()] = true;
    }
  }

  //---

  // set description
  descText_->setText(type->description());

  // can only set x/y range if plot supports custom range
  xminEdit_->setEnabled(type->customXRange());
  yminEdit_->setEnabled(type->customYRange());
  xmaxEdit_->setEnabled(type->customXRange());
  ymaxEdit_->setEnabled(type->customYRange());

  // can only set range if plot type supplies which column refers to these axes
  QString xcolName = type->xColumnName();
  QString ycolName = type->yColumnName();

  xminButton_->setEnabled(xcolName != "");
  yminButton_->setEnabled(ycolName != "");
  xmaxButton_->setEnabled(xcolName != "");
  ymaxButton_->setEnabled(ycolName != "");

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
    CQChartsPlotType *type = getPlotType();
    assert(type);

    ModelP previewModel;

    CQSummaryModel *summaryModel = modelData_->summaryModel();

    if (modelData_->isSummaryEnabled() && summaryModel)
      previewModel = modelData_->summaryModelP();
    else
      previewModel = model_;

    if (! previewPlot_ || previewPlot_->type() != type || previewPlot_->model() != previewModel) {
      previewView_->removeAllPlots();

      previewPlot_ = type->create(previewView_, previewModel);

      previewPlot_->setPreview(true);

      previewPlot_->setId("Preview");

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
  QToolButton *formatUpdate = qobject_cast<QToolButton *>(sender());

  if (! formatUpdate)
    return;

  CQChartsPlotType *type = getPlotType();

  if (! type)
    return;

  PlotData &plotData = typePlotData_[type->name()];

  QString     parameterName;
  CQLineEdit *formatEdit { nullptr };

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

  auto pce = plotData.columnEdits.find(parameterName);

  if (pce != plotData.columnEdits.end()) {
    CQChartsColumn defColumn;

    if (! columnLineEditValue((*pce).second, column, defColumn))
      return;
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

  CQSummaryModel *summaryModel = modelData_->summaryModel();

  if (modelData_->isSummaryEnabled() && summaryModel) {
    if (! CQChartsModelUtil::columnTypeStr(charts_, summaryModel, column, typeStr))
      return;
  }
  else {
    if (! CQChartsModelUtil::columnTypeStr(charts_, model_.data(), column, typeStr))
      return;
  }

  formatEdit->setText(typeStr);
}

bool
CQChartsCreatePlotDlg::
validate(QStringList &msgs)
{
  msgs.clear();

  CQChartsModelData *modelData = nullptr;

  CQSummaryModel *summaryModel = modelData_->summaryModel();

  if (modelData_->isSummaryEnabled() && summaryModel)
    modelData = modelData_->summaryModelData();
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
    if (parameter->isHidden())
      continue;

    if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN) {
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
        const CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);

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

  if (num_valid == 0) {
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
summaryEnabledSlot()
{
  if (modelData_)
    modelData_->setSummaryEnabled(summaryEnabledCheck_->isChecked());

  validateSlot();
}

void
CQChartsCreatePlotDlg::
updateSummaryTypeSlot()
{
  summaryTypeStack_->setCurrentIndex(summaryTypeCombo_->currentIndex());

  updatePreviewSlot();
}

void
CQChartsCreatePlotDlg::
updatePreviewSlot()
{
  CQSummaryModel *summaryModel = modelData_->summaryModel();

  if (modelData_->isSummaryEnabled() && summaryModel) {
    int  n = summaryMaxRows_->value();

    bool random = (summaryTypeCombo_->currentText() == "Random");
    bool sorted = (summaryTypeCombo_->currentText() == "Sorted");
    bool paged  = (summaryTypeCombo_->currentText() == "Paged" );

    if (n <= 0) return;

    if (n != summaryModel->maxRows()) {
      summaryModel->setMode(CQSummaryModel::Mode::NORMAL);
      summaryModel->setMaxRows(n);
    }

    if      (random) {
      summaryModel->setRandom(true);
    }
    else if (sorted) {
      int sortCol = summarySortedColEdit_->value();

      summaryModel->setSortColumn(sortCol);
      summaryModel->setSorted(true);
    }
    else if (paged) {
      int ps = summaryPageSizeEdit_   ->value();
      int np = summaryCurrentPageEdit_->value();

      int nr = model_.data()->rowCount();

      int np1 = (nr + ps - 1)/ps;

      np = std::min(np, np1 - 1);

      summaryCurrentPageEdit_->setRange(0, np1 - 1);
      summaryCurrentPageEdit_->setToolTip(QString("Set Preview Page Count (0 -> %1)").arg(np1 - 1));

      summaryModel->setPageSize(ps);
      summaryModel->setCurrentPage(np);
      summaryModel->setPaged(true);
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
  CQChartsPlotType *type = getPlotType();

  if (! type)
    return false;

  // get or create view
  QString viewId = viewEdit_->text();

  CQChartsView *view = charts()->getView(viewId);

  if (! view) {
    view = charts()->addView();

    CQChartsWindow *window = charts()->createWindow(view);

    window->show();

    viewEdit_->setText(view->id());
  }

  // create plot
  plot_ = type->create(view, model_);

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
CQChartsCreatePlotDlg::
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
CQChartsCreatePlotDlg::
applyPlot(CQChartsPlot *plot, bool preview)
{
  ModelP model = plot->model();

  CQChartsPlotType *type = plot->type();

  // set plot property for widgets for plot parameters
  PlotData &plotData = typePlotData_[type->name()];

  for (const auto &parameter : type->parameters()) {
    if (parameter->isHidden())
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

      int defValue = CQChartsVariant::toInt(parameter->defValue(), ok);

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

      int defValue = CQChartsVariant::toInt(parameter->defValue(), ok);

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

  if (xminEdit_->text().length()) {
    double xmin = CQChartsUtil::toReal(xminEdit_->text(), xminOk);
    if (xminOk) plot->setXMin(CQChartsOptReal(xmin));
  }

  if (yminEdit_->text().length()) {
    double ymin = CQChartsUtil::toReal(yminEdit_->text(), yminOk);
    if (yminOk) plot->setYMin(CQChartsOptReal(ymin));
  }

  if (xmaxEdit_->text().length()) {
    double xmax = CQChartsUtil::toReal(xmaxEdit_->text(), xmaxOk);
    if (xmaxOk) plot->setXMax(CQChartsOptReal(xmax));
  }

  if (ymaxEdit_->text().length()) {
    double ymax = CQChartsUtil::toReal(ymaxEdit_->text(), ymaxOk);
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

  QStringList posStrs = posStr.split(" ", QString::SkipEmptyParts);

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
  auto pf = plotData.formatEdits.find(parameter->name());
  assert(pf != plotData.formatEdits.end());

  const FormatEditData &formatEditData = (*pf).second;

  QString format = formatEditData.formatEdit->text();

  if (format != "")
    columnType = format;

  //---

  CQChartsColumn defColumn = parameter->defValue().value<CQChartsColumn>();

  auto pe = plotData.columnEdits.find(parameter->name());
  assert(pe != plotData.columnEdits.end());

  if (! columnLineEditValue((*pe).second, column, defColumn))
    return false;

  auto pm = plotData.mappedEdits.find(parameter->name());

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
CQChartsCreatePlotDlg::
parseParameterColumnsEdit(CQChartsPlotParameter *parameter, const PlotData &plotData,
                          CQChartsColumns &columns, QString &columnType)
{
  auto pf = plotData.formatEdits.find(parameter->name());
  assert(pf != plotData.formatEdits.end());

  const FormatEditData &formatEditData = (*pf).second;

  QString format = formatEditData.formatEdit->text();

  if (format != "")
    columnType = format;

  //---

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

  QWidget *edit = (*p).second;

  CQRealSpin *spinEdit = qobject_cast<CQRealSpin *>(edit);
  CQLineEdit *lineEdit = qobject_cast<CQLineEdit *>(edit);

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

  QWidget *edit = (*p).second;

  CQIntegerSpin *spinEdit = qobject_cast<CQIntegerSpin *>(edit);
  CQLineEdit    *lineEdit = qobject_cast<CQLineEdit    *>(edit);

  if (spinEdit)
    i = spinEdit->value();
  else {
    bool ok;

    i = CQChartsUtil::toInt(lineEdit->text(), ok);
  }

  return true;
}

bool
CQChartsCreatePlotDlg::
parseParameterEnumEdit(CQChartsPlotParameter *parameter, const PlotData &plotData, int &i)
{
  CQChartsEnumParameter *eparameter = dynamic_cast<CQChartsEnumParameter *>(parameter);
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
columnLineEditValue(CQChartsColumnLineEdit *le, CQChartsColumn &column,
                    const CQChartsColumn &defColumn) const
{
  column = le->column();

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
