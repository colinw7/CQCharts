#ifndef CQChartsViewToolTip_H
#define CQChartsViewToolTip_H

#include <CQToolTip.h>

class CQChartsView;
class QLabel;

class CQChartsViewToolTip : public CQToolTipIFace {
 public:
  CQChartsViewToolTip(CQChartsView *view);

 ~CQChartsViewToolTip();

  QWidget *showWidget(const QPoint &gpos) override;

  void hideWidget() override;

  bool trackMouse() const override { return true; }

  bool updateWidget(const QPoint &gpos) override;

  bool isHideKey(int key, Qt::KeyboardModifiers mod) const override;

  QSize sizeHint() const override;

 private:
  CQChartsView* view_   { nullptr };
  QLabel*       widget_ { nullptr };
};

#endif
