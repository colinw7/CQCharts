#ifndef CQChartsViewToolTip_H
#define CQChartsViewToolTip_H

class CQChartsView;

#include <CQToolTip.h>

#include <QLabel>
#include <QPointer>

/*!
 * \brief Charts View Tooltip
 * \ingroup Charts
 */
class CQChartsViewToolTip : public CQToolTipIFace {
 public:
  using View = CQChartsView;

 public:
  CQChartsViewToolTip(View *view);

 ~CQChartsViewToolTip();

  View *view() const;

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
  using ViewP = QPointer<View>;

  ViewP   view_;                 //!< parent view
  QLabel* widget_   { nullptr }; //!< tip widget
  QPoint  gpos_;                 //!< global position
  QFont   font_;                 //!< font
  double  hideSecs_ { 3.0 };     //!< hide seconds
};

//------

#include <CQFloatTip.h>

/*!
 * \brief Charts View Tooltip
 * \ingroup Charts
 */
class CQChartsViewFloatTip : public CQFloatTip {
 public:
  using View = CQChartsView;

 public:
  CQChartsViewFloatTip(View *view);

  View *view() const;

  void showTip(const QPoint &gpos) override;
  void hideTip() override;

  void showQuery(const QPoint &gpos) override;

  bool isIgnoreKey(Qt::Key key, Qt::KeyboardModifiers modifiers) const override;

  bool updateTip();

 private:
  using ViewP = QPointer<View>;

  ViewP  view_;
  QPoint tipPos_;
};

#endif
