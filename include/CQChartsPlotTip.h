#ifndef CQChartsPlotTip_H
#define CQChartsPlotTip_H

#include <CQChartsPlotTip.h>

#if 0
class CQChartsPlotTipLabel : public QLabel {
 public:
  CQChartsPlotTipLabel();

  void setText(const QString &text);

  void updateSize();

  QSize sizeHint() const { return size_; }

 public:
  QSize size_;
};
#endif

#if 0
class CQChartsPlotTip : public CQToolTipIFace {
 public:
  CQChartsPlotTip();
 ~CQChartsPlotTip();

  void setPlot(CQChartsPlot *plot);

  QWidget *showWidget(const QPoint &p) override;

  void hideWidget() override;

  //bool trackMouse() const override { return true; }

  bool updateWidget(const QPoint &) override;

  bool isHideKey(int key, Qt::KeyboardModifiers mod) const override;

  bool keyPress(int key, Qt::KeyboardModifiers mod) override;

 private:
  using PlotP = QPointer<CQChartsPlot>;

  PlotP                 plot_;
  CQChartsPlotTipLabel* widget_   { nullptr };
  bool                  expanded_ { false };
};
#endif

#endif
