#ifndef CQAppWindow_H
#define CQAppWindow_H

#include <QFrame>

class QMenuBar;
class QMenu;
class QToolButton;

// toplevel window with support for menbar, toolbars, side panels and status bar
class CQAppWindow : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool showMenuBar READ isShowMenuBar WRITE setShowMenuBar)

 public:
  CQAppWindow();

  QMenuBar *menuBar() const { return menuBar_; }

  QWidget *centralWidget() const { return centralWidget_; }

  QWidget *statusBar() const { return statusBar_; }

  bool isShowMenuBar() const { return showMenuBar_; }
  void setShowMenuBar(bool b) { showMenuBar_ = b; }

  QMenuBar *addMenuBar();

  void setCentralWidget(QWidget *widget);

 private:
  void resizeEvent(QResizeEvent *);

  void updatePlacement();

 public:
  QMenuBar* menuBar_       { nullptr };
  QWidget*  centralWidget_ { nullptr };
  QWidget*  statusBar_     { nullptr };
  bool      showMenuBar_   { true };
};

#endif
