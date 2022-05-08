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

  double hideSecs() const override { return hideSecs_; }
  void setHideSecs(double d) { hideSecs_ = d; }

  bool updateWidget(const QPoint &gpos) override;

  bool isHideKey(int key, Qt::KeyboardModifiers mod) const override;

  bool grabKey() const override { return true; }

  void setFont(const QFont &font);

  bool showTip(const QPoint &gpos);

  bool isVisible() const { return widget_ && widget_->isVisible(); }

  QSize sizeHint() const override;

 private:
  CQChartsView* view_     { nullptr }; //!< parent view
  QLabel*       widget_   { nullptr }; //!< tip widget
  QPoint        gpos_;                 //!< global position
  QFont         font_;                 //!< font
  double        hideSecs_ { 3.0 };     //!< hide seconds
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
