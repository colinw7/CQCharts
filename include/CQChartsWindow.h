#ifndef CQChartsWindow_H
#define CQChartsWindow_H

#include <QFrame>

class CQChartsWindow;
class CQChartsView;
class CQChartsViewExpander;
class CQChartsViewSettings;
class CQChartsTable;
class CQChartsTree;
class CQChartsViewStatus;
class CQChartsViewToolBar;
class QStackedWidget;

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

  QSize sizeHint() const;

 private slots:
  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

  void filterChangedSlot();

  void plotSlot();
  void modeSlot();
  void selectModeSlot();

  void setStatusText(const QString &text);
  void setPosText   (const QString &text);
  void setSelText   (const QString &text);

 private:
  CQChartsView*         view_             { nullptr };
  CQChartsViewSettings* settings_         { nullptr };
  CQChartsViewExpander* settingsExpander_ { nullptr };
  QStackedWidget*       viewStack_        { nullptr };
  CQChartsTable*        table_            { nullptr };
  CQChartsTree*         tree_             { nullptr };
  CQChartsViewExpander* tableExpander_    { nullptr };
  CQChartsViewStatus*   status_           { nullptr };
  CQChartsViewToolBar*  toolbar_          { nullptr };
  int                   toolBarHeight_    { 8 };
  int                   statusHeight_     { 8 };
};

#endif
