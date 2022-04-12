#include <CQChartsPaletteCanvas.h>
#include <CQChartsView.h>
#include <CQColors.h>

CQChartsPaletteCanvas::
CQChartsPaletteCanvas(QWidget *parent) :
 CQColorsEditCanvas(parent)
{
}

void
CQChartsPaletteCanvas::
setView(CQChartsView *view)
{
  view_ = view;
}

void
CQChartsPaletteCanvas::
setInterface(bool interface)
{
  interface_ = interface;

  updatePalette();
}
void
CQChartsPaletteCanvas::
setPaletteName(const QString &name)
{
  paletteName_ = name;

  updatePalette();
}

void
CQChartsPaletteCanvas::
updatePalette()
{
  setGray(interface_);

  if (interface_) {
    if (view_)
      setPalette(view_->interfacePalette());
  }
  else {
    auto *palette = CQColorsMgrInst->getNamedPalette(paletteName_);

    setPalette(palette);
  }

  update();
}
