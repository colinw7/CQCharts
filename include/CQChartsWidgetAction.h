#ifndef CQChartsWidgetAction_H
#define CQChartsWidgetAction_H

#include <QWidgetAction>

/*!
 * \brief Widget Action wrapped for Menus
 * \ingroup Charts
 */
class CQChartsWidgetAction : public QWidgetAction {
 public:
  CQChartsWidgetAction(QWidget *w) :
   QWidgetAction(nullptr), w_(w) {
  }

 ~CQChartsWidgetAction() {
    delete w_;
  }

  QWidget *widget() const { return w_; }

  QWidget *createWidget(QWidget *parent) override {
    w_->setParent(parent);

    return w_;
  }

 private:
  QWidget *w_ { nullptr };
};

#endif
