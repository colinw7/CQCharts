#include <CQChartsViewStatus.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <QHBoxLayout>
#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include <QLabel>
#include <QContextMenuEvent>

CQChartsViewStatus::
CQChartsViewStatus(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("status");

  setAutoFillBackground(true);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  statusLabel_ = new QLabel;

  statusLabel_->setObjectName("status");

  posLabel_ = new CQChartsViewStatusPos(this);

  selLabel_ = new QLabel;

  selLabel_->setObjectName("sel");

  QFontMetrics fm(font());

  QFrame *selSpacer = new QFrame;
  selSpacer->setFixedWidth(fm.width("XX"));

  layout->addWidget (statusLabel_);
  layout->addStretch(1);
  layout->addWidget (posLabel_);
  layout->addWidget (selSpacer);
  layout->addWidget (selLabel_);

  //---

  setStatusText("");
  setPosText("");
  setSelText("None");
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
  statusLabel_->setText("<b>Status:</b> " + s);

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
  selLabel_->setText("<b>Sel:</b> " + s);

  update();
}

QSize
CQChartsViewStatus::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("XX"), fm.height() + 4);
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
  text_ = text;

  QLabel::setText("<b>Pos:</b> " + text_);
}

void
CQChartsViewStatusPos::
contextMenuEvent(QContextMenuEvent *e)
{
  QMenu *menu = new QMenu;

  QActionGroup *actionGroup = new QActionGroup(menu);

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

  (void) menu->exec(e->globalPos());

  delete menu;
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
