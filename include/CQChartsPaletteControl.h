#ifndef CQChartsPaletteControl_H
#define CQChartsPaletteControl_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>

class CQChartsPaletteCanvas;
class CQChartsView;

class CQColorsEditControl;

class QComboBox;

class CQChartsPaletteControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsPaletteControl(QWidget *parent);

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view);

  void updatePalettes();

  void updatePaletteWidgets();

  void updateView();

 private slots:
  void palettesComboSlot(int);

  void resetSlot();

  void colorsChangedSlot();

 private:
  CQChartsView*          view_            { nullptr };
  QComboBox*             palettesCombo_   { nullptr };
  CQChartsPaletteCanvas* palettesPlot_    { nullptr };
  CQColorsEditControl*   palettesControl_ { nullptr };
};

#endif
