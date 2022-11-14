#include <CQChartsInterfaceControl.h>
#include <CQChartsPaletteCanvas.h>
#include <CQChartsView.h>

#include <CQColorsEditControl.h>
#include <CQTabSplit.h>
#include <CQUtil.h>

#include <QHBoxLayout>

CQChartsInterfaceControl::
CQChartsInterfaceControl(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("interfaceControl");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //--

  // create interface splitter and add canvas and control
  auto *splitter = CQUtil::makeWidget<CQTabSplit>("splitter");

  splitter->setOrientation(Qt::Vertical);

  layout->addWidget(splitter);

  interfacePlot_    = new CQChartsPaletteCanvas(this);
  interfaceControl_ = new CQColorsEditControl(interfacePlot_);

  splitter->addWidget(interfacePlot_   , "Plot"   );
  splitter->addWidget(interfaceControl_, "Control");

  //--

  connect(interfacePlot_, SIGNAL(colorsChanged()), this, SLOT(colorsChangedSlot()));
}

CQChartsView *
CQChartsInterfaceControl::
view() const
{
  return view_.data();
}

void
CQChartsInterfaceControl::
setView(CQChartsView *view)
{
  view_ = view;

  interfacePlot_->setView(view_);

  updateState();

  connect(interfaceControl_, SIGNAL(stateChanged()), view_, SLOT(updatePlots()));
}

void
CQChartsInterfaceControl::
updateState()
{
  interfacePlot_->setInterface(true);

  interfaceControl_->updateState();
}

void
CQChartsInterfaceControl::
colorsChangedSlot()
{
  updateView();
}

void
CQChartsInterfaceControl::
updateView()
{
  view_->updatePlots();
}
