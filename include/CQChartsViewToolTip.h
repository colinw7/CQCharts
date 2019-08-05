#ifndef CQChartsViewToolTip_H
#define CQChartsViewToolTip_H

#include <CQToolTip.h>
#include <QLabel>

class CQChartsView;

/*!
 * \brief Charts View Tooltip
 * \ingroup Charts
 */
class CQChartsViewToolTip : public CQToolTipIFace {
 public:
  CQChartsViewToolTip(CQChartsView *view);

 ~CQChartsViewToolTip();

  CQChartsView *view() const { return view_; }

  QWidget *widget() const { return widget_; }

  QWidget *showWidget(const QPoint &gpos) override;

  void hideWidget() override;

  bool trackMouse() const override { return true; }

  bool updateWidget(const QPoint &gpos) override;

  bool isHideKey(int key, Qt::KeyboardModifiers mod) const override;

  bool grabKey() const override { return true; }

  QSize sizeHint() const override;

 private:
  bool showTip(const QPoint &gpos);

 private:
  CQChartsView* view_   { nullptr };
  QLabel*       widget_ { nullptr };
  QPoint        gpos_;
};

#endif
