#ifndef CQChartsPaletteControl_H
#define CQChartsPaletteControl_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>
#include <QPointer>

class CQChartsPaletteCanvas;
class CQChartsView;

class CQColorsEditControl;

class QComboBox;

class CQChartsPaletteControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using View = CQChartsView;

 public:
  CQChartsPaletteControl(QWidget *parent);

  View *view() const;
  void setView(View *view) override;

  void updatePalettes();

  void updatePaletteWidgets();

  void updateView();

 private Q_SLOTS:
  void palettesComboSlot(int);

  void resetSlot();

  void colorsChangedSlot();

 private:
  using ViewP = QPointer<View>;

  ViewP                  view_;
  QComboBox*             palettesCombo_   { nullptr };
  CQChartsPaletteCanvas* palettesPlot_    { nullptr };
  CQColorsEditControl*   palettesControl_ { nullptr };
};

#endif
