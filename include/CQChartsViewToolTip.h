#ifndef CQChartsViewToolTip_H
#define CQChartsViewToolTip_H

class CQChartsView;

#include <CQToolTip.h>
#include <QLabel>

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

  void setFont(const QFont &font);

  QSize sizeHint() const override;

 private:
  bool showTip(const QPoint &gpos);

 private:
  CQChartsView* view_   { nullptr };
  QLabel*       widget_ { nullptr };
  QPoint        gpos_;
  QFont         font_;
};

//------

#include <CQFloatTip.h>

/*!
 * \brief Charts View Tooltip
 * \ingroup Charts
 */
class CQChartsViewFloatTip : public CQFloatTip {
 public:
  CQChartsViewFloatTip(CQChartsView *view);

  CQChartsView *view() const { return view_; }

  void showTip(const QPoint &gpos) override;
  void hideTip() override;

  void showQuery(const QPoint &gpos) override;

  bool isIgnoreKey(Qt::Key key, Qt::KeyboardModifiers modifiers) const override;

  bool updateTip();

 private:
  CQChartsView *view_ { nullptr };
  QPoint        tipPos_;
};

#endif
