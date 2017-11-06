#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsViewExpander.h>
#include <CQChartsViewSettings.h>
#include <CQChartsViewStatus.h>
#include <CQChartsViewToolBar.h>

CQChartsWindow::
CQChartsWindow(CQChartsView *view) :
 QFrame(0), view_(view)
{
  setObjectName("window");

  setAttribute(Qt::WA_DeleteOnClose);

  //---

  view->setParent(this);

  expander_ = new CQChartsViewExpander(this);
  settings_ = new CQChartsViewSettings(this);
  status_   = new CQChartsViewStatus(this);
  toolbar_  = new CQChartsViewToolBar(this);

  //----

  connect(view_, SIGNAL(modeChanged()), this, SLOT(modeSlot()));

  connect(view_, SIGNAL(statusTextChanged(const QString &)),
          this, SLOT(setStatusText(const QString &)));

  //---

  updateMargins();
}

CQChartsWindow::
~CQChartsWindow()
{
  delete expander_;
  delete settings_;
  delete status_;
  delete toolbar_;
}

void
CQChartsWindow::
resizeEvent(QResizeEvent *)
{
  updateGeometry();
}

void
CQChartsWindow::
updateMargins()
{
  statusHeight_  = status_ ->sizeHint().height();
  toolBarHeight_ = toolbar_->sizeHint().height();
}

void
CQChartsWindow::
updateGeometry()
{
  setMinimumSize(16 + expander_->width(), 16 + statusHeight_ + toolBarHeight_);

  //---

  view_->resize(width() - expander_->width(), height() - statusHeight_ - toolBarHeight_);

  view_->move(0, toolBarHeight_);

  //---

  settings_->setVisible(expander_->isExpanded());

  if (expander_->isExpanded()) {
    settings_->move  (width() - settings_->width(), 0);
    settings_->resize(settings_->width(), height());
  }

  //---

  expander_->setVisible(true);

  if (expander_->isExpanded())
    expander_->move(width() - settings_->width() - expander_->width(), 0);
  else
    expander_->move(width() - expander_->width(), 0);

  expander_->resize(expander_->width(), height());

  //---

  toolbar_->move(0, 0);
  toolbar_->resize(width(), toolBarHeight_);

  //---

  status_->move(0, height() - statusHeight_);
  status_->resize(width(), statusHeight_);

  //---

  toolbar_ ->raise();
  status_  ->raise();
  settings_->raise();
}

void
CQChartsWindow::
moveExpander(int dx)
{
  expander_->move(expander_->x() - dx, expander_->y());

  settings_->resize(settings_->width() + dx, settings_->height());

  settings_->move(settings_->x() - dx, settings_->y());

  settings_->raise();
}

void
CQChartsWindow::
setStatusText(const QString &text)
{
  status_->setText(text);
}

void
CQChartsWindow::
modeSlot()
{
  toolbar_->updateMode();
}

QSize
CQChartsWindow::
sizeHint() const
{
  return QSize(1600, 1200);
}
