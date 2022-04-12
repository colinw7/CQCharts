#include <CQChartsPaletteControl.h>
#include <CQChartsPaletteCanvas.h>
#include <CQChartsView.h>
#include <CQColorsTheme.h>

#include <CQColorsEditControl.h>
#include <CQTabSplit.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

CQChartsPaletteControl::
CQChartsPaletteControl(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("paletteControl");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //--

  // create control frame
  auto *controlFrame  = CQUtil::makeWidget<QFrame>("control");
  auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

  //--

  auto *nameLabel = CQUtil::makeLabelWidget<QLabel>("Name", "nameLabel");

  palettesCombo_ = CQUtil::makeWidget<QComboBox>("palettesCombo");

  QStringList paletteNames;

  CQColorsMgrInst->getPaletteNames(paletteNames);

  palettesCombo_->addItems(paletteNames);

  palettesCombo_->setToolTip("Name of Palette to View/Edit");

  controlLayout->addWidget(nameLabel);
  controlLayout->addWidget(palettesCombo_);

  connect(palettesCombo_, SIGNAL(currentIndexChanged(int)),
          this, SLOT(palettesComboSlot(int)));

  //--

  auto *resetButton = CQUtil::makeLabelWidget<QPushButton>("Reset", "resetButton");

  resetButton->setToolTip("Reset palette to default (initial) values");

  controlLayout->addWidget(resetButton);

  connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSlot()));

  //--

  controlLayout->addStretch(1);

  layout->addWidget(controlFrame);

  //----

  // create palettes splitter and add canvas and control
  auto *splitter = CQUtil::makeWidget<CQTabSplit>("splitter");

  splitter->setOrientation(Qt::Vertical);

  layout->addWidget(splitter);

  palettesPlot_    = new CQChartsPaletteCanvas(this);
  palettesControl_ = new CQColorsEditControl(palettesPlot_);

  splitter->addWidget(palettesPlot_   , "Plot"   );
  splitter->addWidget(palettesControl_, "Control");

  //--

  connect(palettesPlot_, SIGNAL(colorsChanged()), this, SLOT(colorsChangedSlot()));
}

void
CQChartsPaletteControl::
palettesComboSlot(int)
{
  auto name = palettesCombo_->currentText();

  palettesPlot_->setPaletteName(name);

  palettesControl_->updateState();
}

void
CQChartsPaletteControl::
resetSlot()
{
  auto name = palettesCombo_->currentText();

  auto *palette = CQColorsMgrInst->getNamedPalette(name);
  if (! palette) return;

  CQColorsMgrInst->resetPalette(name);

  updatePalettes();
}

void
CQChartsPaletteControl::
setView(CQChartsView *view)
{
  view_ = view;

  auto *theme = view_->theme();

  palettesPlot_->setView(view_);
  palettesPlot_->setPaletteName(theme->palette()->name());

  palettesControl_->updateState();

  connect(palettesControl_, SIGNAL(stateChanged()), view_, SLOT(updatePlots()));
}

void
CQChartsPaletteControl::
colorsChangedSlot()
{
  updateView();
}

void
CQChartsPaletteControl::
updatePalettes()
{
  updatePaletteWidgets();

  palettesComboSlot(0);

  updateView();
}

void
CQChartsPaletteControl::
updatePaletteWidgets()
{
  palettesCombo_->setEnabled(true);
}

void
CQChartsPaletteControl::
updateView()
{
  view_->updatePlots();
}
