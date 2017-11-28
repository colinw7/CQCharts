#ifndef CQChartsWindow_H
#define CQChartsWindow_H

#include <QFrame>

class CQChartsWindow;
class CQChartsView;
class CQChartsViewExpander;
class CQChartsViewSettings;
class CQChartsViewStatus;
class CQChartsViewToolBar;

#define CQChartsWindowMgrInst CQChartsWindowMgr::instance()

class CQChartsWindowMgr {
 public:
  static CQChartsWindowMgr *instance();

  CQChartsWindow *createWindow(CQChartsView *view);

 ~CQChartsWindowMgr();

  CQChartsWindow *getWindowForView(CQChartsView *view) const;

 private:
  CQChartsWindowMgr();

 private:
  using Windows = std::vector<CQChartsWindow *>;

  Windows windows_;
};

//---

class CQChartsWindow : public QFrame {
  Q_OBJECT

 public:
  CQChartsWindow(CQChartsView *view);
 ~CQChartsWindow();

  CQChartsView *view() const { return view_; }

  void updatePalette();

  //---

  void resizeEvent(QResizeEvent *);

  void updateMargins();

  void updateGeometry();

  void moveExpander(int dx);

  QSize sizeHint() const;

 private slots:
  void modeSlot();

  void setStatusText(const QString &text);

 private:
  CQChartsView*         view_          { nullptr };
  CQChartsViewExpander* expander_      { nullptr };
  CQChartsViewSettings* settings_      { nullptr };
  CQChartsViewStatus*   status_        { nullptr };
  CQChartsViewToolBar*  toolbar_       { nullptr };
  int                   toolBarHeight_ { 8 };
  int                   statusHeight_  { 8 };
};

#endif
