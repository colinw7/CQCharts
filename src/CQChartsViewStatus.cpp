#include <CQChartsViewStatus.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsWidgetUtil.h>

#include <CQUtil.h>
#include <CQPixmapCache.h>

#include <QHBoxLayout>
#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include <QLabel>
#include <QContextMenuEvent>

#include <svg/error_svg.h>

CQChartsViewStatus::
CQChartsViewStatus(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("status");

  setAutoFillBackground(true);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  statusLabel_ = new CQChartsViewStatusStatus(this);
  posLabel_    = new CQChartsViewStatusPos   (this);
  selLabel_    = new CQChartsViewStatusSel   (this);
  errorButton_ = new CQChartsViewStatusError (this);

  QFontMetricsF fm(font());

  auto *selSpacer = CQChartsWidgetUtil::createHSpacer(1, "selSpacer");

  layout->addWidget (statusLabel_);
  layout->addStretch(1);
  layout->addWidget (posLabel_);
  layout->addWidget (selSpacer);
  layout->addWidget (selLabel_);
  layout->addWidget (errorButton_);

  //---

  setStatusText("");
  setPosText   ("");
  setSelText   ("None");
  setHasErrors (false);
}

QString
CQChartsViewStatus::
statusText() const
{
  return statusLabel_->text();
}

void
CQChartsViewStatus::
setStatusText(const QString &s)
{
  statusLabel_->setText(s);

  update();
}

QString
CQChartsViewStatus::
posText() const
{
  return posLabel_->text();
}

void
CQChartsViewStatus::
setPosText(const QString &s)
{
  posLabel_->setText(s);

  update();
}

QString
CQChartsViewStatus::
selText() const
{
  return selLabel_->text();
}

void
CQChartsViewStatus::
setSelText(const QString &s)
{
  selLabel_->setText(s);

  update();
}

void
CQChartsViewStatus::
setHasErrors(bool b)
{
  errorButton_->setEnabled(b);
}

void
CQChartsViewStatus::
toggleViewErrors()
{
  window()->showErrorsTab();
}

QSize
CQChartsViewStatus::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XX"), fm.height() + 4);
}

//------

CQChartsViewStatusStatus::
CQChartsViewStatusStatus(CQChartsViewStatus *status) :
 status_(status)
{
  setObjectName("status");

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQChartsViewStatusStatus::
setText(const QString &text)
{
  if (active_)
    text_ = text;
  else
    text_ = "";

  QLabel::setText("<b>Status:</b> " + text_);
}

void
CQChartsViewStatusStatus::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = new QMenu;

  //---

  QAction *activeAction = menu->addAction("Active");

  activeAction->setCheckable(true);
  activeAction->setChecked  (active_);

  connect(activeAction, SIGNAL(triggered(bool)), this, SLOT(activateSlot(bool)));

  //---

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQChartsViewStatusStatus::
activateSlot(bool b)
{
  active_ = b;

  setText("");
}

//------

CQChartsViewStatusPos::
CQChartsViewStatusPos(CQChartsViewStatus *status) :
 status_(status)
{
  setObjectName("pos");

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQChartsViewStatusPos::
setText(const QString &text)
{
  if (active_)
    text_ = text;
  else
    text_ = "";

  QLabel::setText("<b>Pos:</b> " + text_);
}

void
CQChartsViewStatusPos::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = new QMenu;

  //---

  QAction *activeAction = menu->addAction("Active");

  activeAction->setCheckable(true);
  activeAction->setChecked  (active_);

  connect(activeAction, SIGNAL(triggered(bool)), this, SLOT(activateSlot(bool)));

  //---

  menu->addSeparator();

  //---

  auto *actionGroup = new QActionGroup(menu);

  QAction *plotAction  = menu->addAction("Plot" );
  QAction *viewAction  = menu->addAction("View" );
  QAction *pixelAction = menu->addAction("Pixel");

  plotAction ->setCheckable(true);
  viewAction ->setCheckable(true);
  pixelAction->setCheckable(true);

  actionGroup->addAction(plotAction);
  actionGroup->addAction(viewAction);
  actionGroup->addAction(pixelAction);

  actionGroup->setExclusive(true);

  CQChartsView::PosTextType posTextType = status_->window()->view()->posTextType();

  if      (posTextType == CQChartsView::PosTextType::PLOT)
    plotAction->setChecked(true);
  else if (posTextType == CQChartsView::PosTextType::VIEW)
    viewAction->setChecked(true);
  else if (posTextType == CQChartsView::PosTextType::PIXEL)
    pixelAction->setChecked(true);

  connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(posTextTypeAction(QAction *)));

  menu->addActions(actionGroup->actions());

  //---

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQChartsViewStatusPos::
activateSlot(bool b)
{
  active_ = b;

  setText("");
}

void
CQChartsViewStatusPos::
posTextTypeAction(QAction *action)
{
  QString str = action->text();

  if      (str == "Plot")
    status_->window()->view()->setPosTextType(CQChartsView::PosTextType::PLOT);
  else if (str == "View")
    status_->window()->view()->setPosTextType(CQChartsView::PosTextType::VIEW);
  else if (str == "Pixel")
    status_->window()->view()->setPosTextType(CQChartsView::PosTextType::PIXEL);
}

//------

CQChartsViewStatusSel::
CQChartsViewStatusSel(CQChartsViewStatus *status) :
 status_(status)
{
  setObjectName("sel");

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQChartsViewStatusSel::
setText(const QString &text)
{
  if (active_)
    text_ = text;
  else
    text_ = "";

  QLabel::setText("<b>Sel:</b> " + text_);
}

void
CQChartsViewStatusSel::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = new QMenu;

  //---

  QAction *activeAction = menu->addAction("Active");

  activeAction->setCheckable(true);
  activeAction->setChecked  (active_);

  connect(activeAction, SIGNAL(triggered(bool)), this, SLOT(activateSlot(bool)));

  //---

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQChartsViewStatusSel::
activateSlot(bool b)
{
  active_ = b;

  setText("");
}

//------

CQChartsViewStatusError::
CQChartsViewStatusError(CQChartsViewStatus *status) :
 status_(status)
{
  setObjectName("error");

  setIcon(CQPixmapCacheInst->getIcon("ERROR"));

  setToolTip("Show Plot Errors");

  connect(this, SIGNAL(clicked()), status, SLOT(toggleViewErrors()));
}
