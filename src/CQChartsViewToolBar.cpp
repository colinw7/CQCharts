#include <CQChartsViewToolBar.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsManageModelsDlg.h>
#include <CQChartsCreatePlotDlg.h>
#include <CQChartsModelData.h>
#include <CQChartsHelpDlg.h>
#include <CQCharts.h>

#include <CQPixmapCache.h>
#include <CQIconCombo.h>
#include <CQUtil.h>

#include <svg/select_light_svg.h>
#include <svg/select_dark_svg.h>
#include <svg/zoom_light_svg.h>
#include <svg/zoom_dark_svg.h>
#include <svg/pan_light_svg.h>
#include <svg/pan_dark_svg.h>
#include <svg/probe_light_svg.h>
#include <svg/probe_dark_svg.h>
#include <svg/edit_light_svg.h>
#include <svg/edit_dark_svg.h>
#include <svg/zoom_fit_light_svg.h>
#include <svg/zoom_fit_dark_svg.h>
#include <svg/left_light_svg.h>
#include <svg/left_dark_svg.h>
#include <svg/right_light_svg.h>
#include <svg/right_dark_svg.h>
#include <svg/charts_light_svg.h>
#include <svg/charts_dark_svg.h>
#include <svg/models_light_svg.h>
#include <svg/models_dark_svg.h>
#include <svg/options_svg.h>
#include <svg/table_svg.h>
#include <svg/info_svg.h>

#include <QStackedWidget>
#include <QToolButton>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>
#include <QHBoxLayout>

CQChartsViewToolBar::
CQChartsViewToolBar(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("toolbar");

  setAutoFillBackground(true);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  //---

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  auto createButton = [&](const QString &name, const QString &iconName, const QString &tip,
                          const char *receiver, bool checkable=false, bool checked=false) {
    QToolButton *button = CQUtil::makeWidget<QToolButton>(name);

    if (CQPixmapCacheInst->hasPixmap(iconName + "_LIGHT"))
      button->setIcon(CQPixmapCacheInst->getIcon(iconName + "_LIGHT", iconName + "_DARK"));
    else
      button->setIcon(CQPixmapCacheInst->getIcon(iconName));

    if (checkable) {
      button->setCheckable(true);
      button->setChecked(checked);
    }

    button->setFocusPolicy(Qt::NoFocus);

    connect(button, SIGNAL(clicked(bool)), this, receiver);

    button->setToolTip(tip);

    button->setFocusPolicy(Qt::NoFocus);

    return button;
  };

  auto createCheckedButton = [&](const QString &name, const QString &iconName, const QString &tip,
                                 const char *receiver, bool checked=false) {
    return createButton(name, iconName, tip, receiver, true, checked);
  };

  //---

  modeCombo_ = CQUtil::makeWidget<CQIconCombo>("modeCombo");

  modeCombo_->addItem(CQPixmapCacheInst->getIcon("SELECT_LIGHT", "SELECT_DARK"), "Select");
  modeCombo_->addItem(CQPixmapCacheInst->getIcon("ZOOM_LIGHT"  , "ZOOM_DARK"  ), "Zoom"  );
  modeCombo_->addItem(CQPixmapCacheInst->getIcon("PAN_LIGHT"   , "PAN_DARK"   ), "Pan"   );
  modeCombo_->addItem(CQPixmapCacheInst->getIcon("PROBE_LIGHT" , "PROBE_DARK" ), "Probe" );
  modeCombo_->addItem(CQPixmapCacheInst->getIcon("EDIT_LIGHT"  , "EDIT_DARK"  ), "Edit"  );

  modeCombo_->setFocusPolicy(Qt::NoFocus);

  connect(modeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(modeSlot(int)));

  layout->addWidget(modeCombo_);

  //---

  controlsStack_ = CQUtil::makeWidget<QStackedWidget>("controls");

  controlsStack_->layout()->setMargin(0); controlsStack_->layout()->setSpacing(0);

  layout->addWidget(controlsStack_);

  QFrame *selectControls = CQUtil::makeWidget<QFrame>("select");
  QFrame *zoomControls   = CQUtil::makeWidget<QFrame>("zoom");
  QFrame *panControls    = CQUtil::makeWidget<QFrame>("pan");
  QFrame *probeControls  = CQUtil::makeWidget<QFrame>("probe");
  QFrame *editControls   = CQUtil::makeWidget<QFrame>("edit");

  controlsStack_->addWidget(selectControls);
  controlsStack_->addWidget(zoomControls);
  controlsStack_->addWidget(panControls);
  controlsStack_->addWidget(probeControls);
  controlsStack_->addWidget(editControls);

  layout->addStretch(1);

  //-----

  QHBoxLayout *selectControlsLayout = CQUtil::makeLayout<QHBoxLayout>(selectControls, 0, 2);

  QButtonGroup *selectButtonGroup = new QButtonGroup(this);

  selectPointButton_ = CQUtil::makeLabelWidget<QRadioButton>("Point", "point");

  selectPointButton_->setFocusPolicy(Qt::NoFocus);
  selectPointButton_->setToolTip("Select objects at point");

  selectRectButton_ = CQUtil::makeLabelWidget<QRadioButton>("Rect", "rect");

  selectRectButton_->setFocusPolicy(Qt::NoFocus);
  selectRectButton_->setToolTip("Select objects inside/touching rectangle");

  if (view()->selectMode() == CQChartsView::SelectMode::POINT)
    selectPointButton_->setChecked(true);
  else
    selectRectButton_ ->setChecked(true);

  selectButtonGroup->addButton(selectPointButton_, 0);
  selectButtonGroup->addButton(selectRectButton_ , 1);

  selectControlsLayout->addWidget(selectPointButton_);
  selectControlsLayout->addWidget(selectRectButton_ );

  connect(selectButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(selectButtonClicked(int)));

  //--

  selectInsideCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Inside", "selectInside");

  selectInsideCheck_->setFocusPolicy(Qt::NoFocus);
  selectInsideCheck_->setToolTip("Rectangle intersect inside (checked) or touching (unchecked)");

  selectInsideCheck_->setChecked(view()->isSelectInside());

  connect(selectInsideCheck_, SIGNAL(stateChanged(int)), this, SLOT(selectInsideSlot(int)));

  selectControlsLayout->addWidget(selectInsideCheck_);

  //--

  selectControlsLayout->addStretch(1);

  //-----

  QHBoxLayout *zoomControlsLayout = CQUtil::makeLayout<QHBoxLayout>(zoomControls, 0, 2);

  QPushButton *zoomButton = CQUtil::makeLabelWidget<QPushButton>("Reset", "reset");

  zoomButton->setFocusPolicy(Qt::NoFocus);
  zoomButton->setToolTip("Reset Zoom");

  connect(zoomButton, SIGNAL(clicked()), this, SLOT(zoomFullSlot()));

  zoomControlsLayout->addWidget(zoomButton);

  //-----

  QHBoxLayout *panControlsLayout = CQUtil::makeLayout<QHBoxLayout>(panControls, 0, 2);

  QPushButton *panButton = CQUtil::makeLabelWidget<QPushButton>("Reset", "reset");

  panButton->setFocusPolicy(Qt::NoFocus);
  panButton->setToolTip("Reset Pan");

  connect(panButton, SIGNAL(clicked()), this, SLOT(panResetSlot()));

  panControlsLayout->addWidget(panButton);

  //-----

  viewSettingsButton_ =
    createCheckedButton("settings", "OPTIONS", "Show/Hide View Settings",
                        SLOT(viewSettingsSlot(bool)), true);

  layout->addWidget(viewSettingsButton_);

  //-----

  bool showTable = (window_ && window_->isDataTable());

  viewTableButton_ =
    createCheckedButton("table", "TABLE", "Show/Hide View Table",
                        SLOT(viewTableSlot(bool)), showTable);

  layout->addWidget(viewTableButton_);

  //-----

  manageModelsDlgButton_ =
    createButton("modelDlg", "MODELS", "Manage Models", SLOT(manageModelsSlot()));
  createPlotDlgButton_ =
    createButton("plotDlg" , "CHARTS", "Add Plot", SLOT(addPlotSlot()));

  layout->addWidget(manageModelsDlgButton_);
  layout->addWidget(createPlotDlgButton_);

  autoFitButton_ = createButton("fit"  , "ZOOM_FIT", "Zoom Fit"    , SLOT(autoFitSlot()));

  layout->addWidget(autoFitButton_);

  //---

  leftButton_  = createButton("left" , "LEFT"    , "Scroll Left" , SLOT(leftSlot()));
  rightButton_ = createButton("right", "RIGHT"   , "Scroll Right", SLOT(rightSlot()));

  layout->addWidget(leftButton_);
  layout->addWidget(rightButton_);

  //---

  QToolButton *helpButton = createButton("help", "INFO", "Help" , SLOT(helpSlot()));

  layout->addWidget(helpButton);

  //---

  connect(view(), SIGNAL(plotAdded(const QString &)), this, SLOT(updateState()));

  connect(view(), SIGNAL(scrollDataChanged()), this, SLOT(updateState()));

  //---

  updateState();
}

CQChartsView *
CQChartsViewToolBar::
view() const
{
  return window()->view();
}

void
CQChartsViewToolBar::
updateState()
{
  bool scrolled = view()->isScrolled();

  int numScrolled = (scrolled ? view()->numPlots() : 1);

  leftButton_ ->setVisible(numScrolled > 1);
  rightButton_->setVisible(numScrolled > 1);

  if (numScrolled > 1) {
    leftButton_ ->setEnabled(view()->scrollPage() > 0);
    rightButton_->setEnabled(view()->scrollPage() < view()->scrollNumPages() - 1);
  }
}

void
CQChartsViewToolBar::
modeSlot(int ind)
{
  if      (ind == 0)
    view()->setMode(CQChartsView::Mode::SELECT);
  else if (ind == 1)
    view()->setMode(CQChartsView::Mode::ZOOM);
  else if (ind == 2)
    view()->setMode(CQChartsView::Mode::PAN);
  else if (ind == 3)
    view()->setMode(CQChartsView::Mode::PROBE);
  else if (ind == 4)
    view()->setMode(CQChartsView::Mode::EDIT);

  updateMode();
}

void
CQChartsViewToolBar::
selectButtonClicked(int ind)
{
  if (ind == 0)
    view()->setSelectMode(CQChartsView::SelectMode::POINT);
  else
    view()->setSelectMode(CQChartsView::SelectMode::RECT);
}

void
CQChartsViewToolBar::
selectInsideSlot(int state)
{
  view()->setSelectInside(state);
}

void
CQChartsViewToolBar::
zoomFullSlot()
{
  CQChartsPlot *plot = view()->currentPlot(/*remap*/true);

  if (plot)
    plot->zoomFull();
}

void
CQChartsViewToolBar::
panResetSlot()
{
}

void
CQChartsViewToolBar::
updateMode()
{
  if      (view()->mode() == CQChartsView::Mode::SELECT) {
    modeCombo_    ->setCurrentIndex(0);
    controlsStack_->setCurrentIndex(0);

    if (view()->selectMode() == CQChartsView::SelectMode::POINT)
      selectPointButton_->setChecked(true);
    else
      selectRectButton_->setChecked(true);
  }
  else if (view()->mode() == CQChartsView::Mode::ZOOM) {
    modeCombo_    ->setCurrentIndex(1);
    controlsStack_->setCurrentIndex(1);
  }
  else if (view()->mode() == CQChartsView::Mode::PAN) {
    modeCombo_    ->setCurrentIndex(2);
    controlsStack_->setCurrentIndex(2);
  }
  else if (view()->mode() == CQChartsView::Mode::PROBE) {
    modeCombo_    ->setCurrentIndex(3);
    controlsStack_->setCurrentIndex(3);
  }
  else if (view()->mode() == CQChartsView::Mode::EDIT) {
    modeCombo_    ->setCurrentIndex(4);
    controlsStack_->setCurrentIndex(4);
  }
}

void
CQChartsViewToolBar::
viewSettingsSlot(bool b)
{
  if (window())
    window()->setViewSettings(b);
}

void
CQChartsViewToolBar::
viewTableSlot(bool b)
{
  if (window())
    window()->setDataTable(b);
}

void
CQChartsViewToolBar::
manageModelsSlot()
{
  CQCharts *charts = view()->charts();

  if (manageModelsDlg_)
    delete manageModelsDlg_;

  manageModelsDlg_ = new CQChartsManageModelsDlg(charts);

  manageModelsDlg_->show();
}

void
CQChartsViewToolBar::
addPlotSlot()
{
  CQCharts *charts = view()->charts();

  CQChartsModelData *modelData = charts->currentModelData();

  if (! modelData)
    return;

  CQChartsCreatePlotDlg *createPlotDlg = charts->createPlotDlg(modelData);

  createPlotDlg->setViewName(view()->id());
}

void
CQChartsViewToolBar::
autoFitSlot()
{
  view()->fitSlot();
}

void
CQChartsViewToolBar::
leftSlot()
{
  view()->scrollLeft();
}

void
CQChartsViewToolBar::
rightSlot()
{
  view()->scrollRight();
}

void
CQChartsViewToolBar::
helpSlot()
{
  CQCharts *charts = view()->charts();

  CQChartsHelpDlgMgrInst->showDialog(charts);
}

QSize
CQChartsViewToolBar::
sizeHint() const
{
  int w = autoFitButton_->sizeHint().width ();
  int h = autoFitButton_->sizeHint().height();

  return QSize(4*w + 6, h);
}
