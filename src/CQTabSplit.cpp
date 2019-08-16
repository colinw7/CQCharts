#include <CQTabSplit.h>
#include <CQGroupBox.h>

#include <QTabBar>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPainter>

#include <cassert>

CQTabSplit::
CQTabSplit(Qt::Orientation orient, QWidget *parent) :
 QFrame(parent), orient_(orient)
{
  init();
}

CQTabSplit::
CQTabSplit(QWidget *parent) :
 QFrame(parent)
{
  init();
}

void
CQTabSplit::
init()
{
  setObjectName("tabSplit");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  splitter_  = new CQTabSplitSplitter (this);
  tabWidget_ = new CQTabSplitTabWidget(this);

  layout->addWidget(splitter_);
  layout->addWidget(tabWidget_);

  tabWidget_->setVisible(false);

  splitter_->setOrientation(orientation());

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void
CQTabSplit::
setOrientation(Qt::Orientation orient)
{
  if (state_ != State::TAB)
    setState(orient == Qt::Horizontal ? State::HSPLIT : State::VSPLIT);
  else
    orient_ = orient;
}

void
CQTabSplit::
setSizes(const QList<int> &sizes)
{
  if (orientation() == Qt::Horizontal)
    hsizes_ = sizes;
  else
    vsizes_ = sizes;

  splitter_->setSizes(sizes);
}

void
CQTabSplit::
addWidget(QWidget *w, const QString &name)
{
  assert(w);

  WidgetData data(w, name);

  if (isGrouped()) {
    data.group = new CQGroupBox(name);
    data.group->setObjectName("group");

    data.layout = new QVBoxLayout(data.group);
    data.layout->setMargin(2); data.layout->setSpacing(2);

    data.layout->addWidget(w);
  }

  widgets_.push_back(data);

  if (state_ == State::HSPLIT || state_ == State::VSPLIT) {
    assert(splitter_);

    if (isGrouped())
      splitter_->addWidget(data.group);
    else
      splitter_->addWidget(data.w);
  }
  else {
    tabWidget_->addTab(data.w, name);
  }
}

QWidget *
CQTabSplit::
widget(int i) const
{
  if (i < 0 || i >= int(widgets_.size()))
    return nullptr;

  return widgets_[i].w;
}

int
CQTabSplit::
count() const
{
  return widgets_.size();
}

void
CQTabSplit::
setState(State state)
{
  if (state_ == state)
    return;

  std::swap(state_, state);

  splitter_ ->setVisible(state_ == State::HSPLIT || state_ == State::VSPLIT);
  tabWidget_->setVisible(state_ == State::TAB);

  // from tab -> splitter
  if      (state == State::TAB) {
    // remove widgets from tab
    int n = tabWidget_->count();

    assert(n == int(widgets_.size()));

    for (int i = 0; i < n; ++i)
      tabWidget_->removeTab(0);

    //---

    //add widgets to splitter
    for (auto &data : widgets_) {
      if (! data.w)
        continue;

      if (isGrouped())
        splitter_->addWidget(data.group);
      else
        splitter_->addWidget(data.w);

      if (isGrouped()) {
        data.layout->addWidget(data.w);

        data.group->setVisible(true);
      }

      data.w->setVisible(true);
    }

    //---

    // restore saved sizes
    if (orientation() == Qt::Horizontal) {
      if (hsizes_.length())
        splitter_->setSizes(hsizes_);
    }
    else {
      if (vsizes_.length())
        splitter_->setSizes(vsizes_);
    }
  }
  // from splitter -> tab
  else if (state_ == State::TAB) {
    // save sizes
    if (orientation() == Qt::Horizontal)
      hsizes_ = splitter_->sizes();
    else
      vsizes_ = splitter_->sizes();

    //---

    // remove widgets from splitter
    int n = splitter_->count();

    assert(n == int(widgets_.size()));

    for (auto &data : widgets_) {
      if (! data.w)
        continue;

      if (isGrouped())
        data.group->setParent(this);
      else
        data.w->setParent(this);
    }

    //---

    // add widgets to tab
    for (auto &data : widgets_) {
      if (! data.w)
        continue;

      tabWidget_->addTab(data.w, data.name);
    }
  }
  // new splitter direction
  else {
    orient_ = (state_ == State::HSPLIT ? Qt::Horizontal : Qt::Vertical);

    splitter_->setOrientation(orientation());
  }
}

QSize
CQTabSplit::
sizeHint() const
{
  if (state_ == State::TAB)
    return tabWidget_->sizeHint();
  else
    return splitter_->sizeHint();
}

//------

CQTabSplitSplitter::
CQTabSplitSplitter(CQTabSplit *split) :
 QSplitter(split), split_(split)
{
  setObjectName("splitter");
}

QSplitterHandle *
CQTabSplitSplitter::
createHandle()
{
  return new CQTabSplitSplitterHandle(orientation(), this);
}

//------

CQTabSplitSplitterHandle::
CQTabSplitSplitterHandle(Qt::Orientation orient, CQTabSplitSplitter *splitter) :
 QSplitterHandle(orient, splitter), splitter_(splitter)
{
  setObjectName("splitterHandle");

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQTabSplitSplitterHandle::
contextMenuEvent(QContextMenuEvent *e)
{
  QMenu *menu = new QMenu;

  menu->setObjectName("menu");

  //---

  QAction *tabAction = menu->addAction("Tabbed");

  connect(tabAction, SIGNAL(triggered()), this, SLOT(tabSlot()));

  if (splitter()->orientation() == Qt::Horizontal) {
    QAction *splitAction = menu->addAction("Horizontal Split");

    connect(splitAction, SIGNAL(triggered()), this, SLOT(splitSlot()));
  }
  else {
    QAction *splitAction = menu->addAction("Vertical Split");

    connect(splitAction, SIGNAL(triggered()), this, SLOT(splitSlot()));
  }

  //---

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQTabSplitSplitterHandle::
paintEvent(QPaintEvent *)
{
  auto blendColors = [](const QColor &c1, const QColor &c2, double f) {
    double f1 = 1.0 - f;

    double r = c1.redF  ()*f + c2.redF  ()*f1;
    double g = c1.greenF()*f + c2.greenF()*f1;
    double b = c1.blueF ()*f + c2.blueF ()*f1;

    return QColor(std::min(std::max(int(255*r), 0), 255),
                  std::min(std::max(int(255*g), 0), 255),
                  std::min(std::max(int(255*b), 0), 255));
  };

  //QSplitterHandle::paintEvent(e);

  QPainter painter(this);

  QColor wc = palette().color(QPalette::Window);

  QColor fc = (hover_ ? blendColors(palette().color(QPalette::Highlight), wc, 0.3) :
                        blendColors(palette().color(QPalette::Highlight), wc, 0.1));

  painter.fillRect(rect(), fc);

  //---

  QColor fg = palette().color(QPalette::Text);
  QColor bg = palette().color(QPalette::Base);

  int ss = 2*barSize();

  if (orientation() == Qt::Horizontal) {
    int ym = rect().center().y();
    int x1 = rect().left  () + 1;
    int x2 = rect().right () - 1;

    painter.fillRect(QRect(x1, ym - ss/2, x2 - x1 + 1, ss), bg);

    //---

    painter.setPen(fg);

    painter.drawLine(x1 + 1, ym - 4, x2 - 1, ym - 4);
    painter.drawLine(x1 + 1, ym    , x2 - 1, ym    );
    painter.drawLine(x1 + 1, ym + 4, x2 - 1, ym + 4);

    //---

    if (hover_) {
      painter.setRenderHints(QPainter::Antialiasing, true);

      int th = barSize()/2;

      QPainterPath path1;

      int yt1 = ym + ss;

      path1.moveTo(x1 - 1, yt1     );
      path1.lineTo(x2    , yt1 + th);
      path1.lineTo(x2    , yt1 - th);

      painter.fillPath(path1, bg);

      QPainterPath path2;

      int yt2 = ym - ss;

      path2.moveTo(x2 + 1, yt2     );
      path2.lineTo(x1    , yt2 + th);
      path2.lineTo(x1    , yt2 - th);

      painter.fillPath(path2, bg);
    }
  }
  else {
    int xm = rect().center().x();
    int y1 = rect().top   () + 1;
    int y2 = rect().bottom() - 1;

    painter.fillRect(QRect(xm - ss/2, y1, ss, y2 - y1 + 1), bg);

    //---

    painter.setPen(fg);

    painter.drawLine(xm - 4, y1 + 1, xm - 4, y2 - 1);
    painter.drawLine(xm    , y1 + 1, xm    , y2 - 1);
    painter.drawLine(xm + 4, y1 + 1, xm + 4, y2 - 1);

    //---

    if (hover_) {
      painter.setRenderHints(QPainter::Antialiasing, true);

      int tw = barSize()/2;

      QPainterPath path1;

      int xt1 = xm + ss;

      path1.moveTo(xt1     , y1 - 1);
      path1.lineTo(xt1 + tw, y2    );
      path1.lineTo(xt1 - tw, y2    );

      painter.fillPath(path1, bg);

      QPainterPath path2;

      int xt2 = xm - ss;

      path2.moveTo(xt2     , y2 + 1);
      path2.lineTo(xt2 + tw, y1    );
      path2.lineTo(xt2 - tw, y1    );

      painter.fillPath(path2, bg);
    }
  }
}

bool
CQTabSplitSplitterHandle::
event(QEvent *event)
{
  switch (event->type()) {
    case QEvent::HoverEnter: hover_ = true ; update(); break;
    case QEvent::HoverLeave: hover_ = false; update(); break;
    default:                 break;
  }

  return QSplitterHandle::event(event);
}

void
CQTabSplitSplitterHandle::
tabSlot()
{
  splitter()->split()->setState(CQTabSplit::State::TAB);
}

void
CQTabSplitSplitterHandle::
splitSlot()
{
  if (splitter()->orientation() == Qt::Horizontal)
    splitter()->setOrientation(Qt::Vertical);
  else
    splitter()->setOrientation(Qt::Horizontal);
}

QSize
CQTabSplitSplitterHandle::
sizeHint() const
{
  QSize s = QSplitterHandle::sizeHint();

  if (splitter()->orientation() == Qt::Horizontal)
    return QSize(barSize(), s.height());
  else
    return QSize(s.width(), barSize());
}

//---

CQTabSplitTabWidget::
CQTabSplitTabWidget(CQTabSplit *split) :
 split_(split)
{
  setObjectName("tab");

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQTabSplitTabWidget::
contextMenuEvent(QContextMenuEvent *e)
{
  QPoint p = e->pos();
  QRect  r = tabBar()->rect();

  if (p.y() >= r.height())
    return;

  QMenu *menu = new QMenu;

  menu->setObjectName("menu");

  //---

  QAction *hsplitAction = menu->addAction("Horizontal Split");
  QAction *vsplitAction = menu->addAction("Vertical Split");

  connect(hsplitAction, SIGNAL(triggered()), this, SLOT(hsplitSlot()));
  connect(vsplitAction, SIGNAL(triggered()), this, SLOT(vsplitSlot()));

  //---

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQTabSplitTabWidget::
hsplitSlot()
{
  split()->setState(CQTabSplit::State::HSPLIT);
}

void
CQTabSplitTabWidget::
vsplitSlot()
{
  split()->setState(CQTabSplit::State::VSPLIT);
}
