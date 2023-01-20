#include <CQChartsViewStatus.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsWidgetUtil.h>

#include <CQUtil.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QContextMenuEvent>

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

  return QSize(fm.horizontalAdvance("XX"), fm.height() + 4);
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
    text_.clear();

  QLabel::setText("<b>Status:</b> " + text_);
}

void
CQChartsViewStatusStatus::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = new QMenu;

  //---

  auto *activeAction = menu->addAction("Active");

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
    text_.clear();

  QLabel::setText("<b>Pos:</b> " + text_);
}

void
CQChartsViewStatusPos::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = new QMenu;

  //---

  auto *activeAction = menu->addAction("Active");

  activeAction->setCheckable(true);
  activeAction->setChecked  (active_);

  connect(activeAction, SIGNAL(triggered(bool)), this, SLOT(activateSlot(bool)));

  //---

  menu->addSeparator();

  //---

  auto posTextType = status_->window()->view()->posTextType();

  auto *actionGroup = CQUtil::createActionGroup(menu);

  actionGroup->setExclusive(true);

  connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(posTextTypeAction(QAction *)));

  CQUtil::addGroupCheckAction(actionGroup, "Plot",
                              posTextType == CQChartsView::PosTextType::PLOT);
  CQUtil::addGroupCheckAction(actionGroup, "View",
                              posTextType == CQChartsView::PosTextType::VIEW);
  CQUtil::addGroupCheckAction(actionGroup, "Pixel",
                              posTextType == CQChartsView::PosTextType::PIXEL);

  CQUtil::addActionGroupToMenu(actionGroup);

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
  auto str = action->text();

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
    text_.clear();

  setToolTip(text_);

  auto labelText = text_;

  if (labelText.length() > 64) {
    QFontMetrics fm(font());

    labelText = fm.elidedText(labelText, Qt::ElideRight, fm.horizontalAdvance("X")*64);
  }

  QLabel::setText("<b>Sel:</b> " + labelText);
}

void
CQChartsViewStatusSel::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = new QMenu;

  //---

  auto *activeAction = menu->addAction("Active");

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

  setIcon("ERROR");

  setToolTip("Show Plot Errors");

  connect(this, SIGNAL(clicked()), status, SLOT(toggleViewErrors()));
}
