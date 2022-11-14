#ifndef CQChartsInterfaceControl_H
#define CQChartsInterfaceControl_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>
#include <QPointer>

class CQChartsPaletteCanvas;
class CQChartsView;

class CQColorsEditControl;

class CQChartsInterfaceControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using View = CQChartsView;

 public:
  CQChartsInterfaceControl(QWidget *parent);

  View *view() const;
  void setView(View *view);

  void updateState();

 private:
  void updateView();

 private Q_SLOTS:
  void colorsChangedSlot();

 private:
  using ViewP = QPointer<View>;

  ViewP                  view_;
  CQChartsPaletteCanvas* interfacePlot_    { nullptr };
  CQColorsEditControl*   interfaceControl_ { nullptr };
};

#endif
