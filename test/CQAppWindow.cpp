#include <CQAppWindow.h>

#include <QMenuBar>
#include <QMenu>
#include <QToolButton>
#include <QHBoxLayout>

CQAppWindow::
CQAppWindow()
{
  setObjectName("appWindow");

  centralWidget_ = new QFrame(this);

  centralWidget_->setObjectName("center");

  updatePlacement();
}

QMenuBar *
CQAppWindow::
addMenuBar()
{
  menuBar_ = new QMenuBar(this);

  menuBar_->setObjectName("menuBar");

  return menuBar_;
}

void
CQAppWindow::
setCentralWidget(QWidget *widget)
{
  delete centralWidget_;

  centralWidget_ = widget;

  if (widget)
    widget->setParent(this);

  updatePlacement();
}

void
CQAppWindow::
resizeEvent(QResizeEvent *)
{
  updatePlacement();
}

void
CQAppWindow::
updatePlacement()
{
  QSize menuBarSize(0, 0);

  if (menuBar_) {
    menuBar_->setVisible(showMenuBar_);

    if (showMenuBar_) {
      QFontMetrics fm(menuBar_->font());

      menuBarSize = QSize(width(), fm.height() + 8);

      menuBar_->move(0, 0);

      menuBar_->resize(menuBarSize);
    }
  }

  QSize centralWidgetSize(0, 0);

  if (centralWidget_) {
    centralWidgetSize = QSize(width(), height() - menuBarSize.height());

    centralWidget_->move(0, menuBarSize.height());

    centralWidget_->resize(centralWidgetSize);
  }
}
