#ifndef CQChartsInterfaceControl_H
#define CQChartsInterfaceControl_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>

class CQChartsPaletteCanvas;
class CQChartsView;

class CQColorsEditControl;

class CQChartsInterfaceControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsInterfaceControl(QWidget *parent);

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view);

  void updateState();

 private:
  void updateView();

 private slots:
  void colorsChangedSlot();

 private:
  CQChartsView*          view_             { nullptr };
  CQChartsPaletteCanvas* interfacePlot_    { nullptr };
  CQColorsEditControl*   interfaceControl_ { nullptr };
};

#endif
