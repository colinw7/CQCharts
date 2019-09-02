#ifndef CQChartsWindow_H
#define CQChartsWindow_H

#include <CQRangeScroll.h>
#include <QFrame>

class CQChartsWindow;
class CQChartsView;
class CQChartsPlot;
class CQChartsViewExpander;
class CQChartsViewSettings;
class CQChartsFilterEdit;
class CQChartsModelViewHolder;
class CQChartsViewStatus;
class CQChartsViewToolBar;
class CQChartsWindowRangeScroll;
class QStackedWidget;

#define CQChartsWindowMgrInst CQChartsWindowMgr::instance()

/*!
 * \brief Class to manage created windows
 * \ingroup Charts
 */
class CQChartsWindowMgr {
 public:
  static CQChartsWindowMgr *instance();

  CQChartsWindow *createWindow(CQChartsView *view);
  void removeWindow(CQChartsWindow *window);

 ~CQChartsWindowMgr();

  CQChartsWindow *getWindowForView(CQChartsView *view) const;

 private:
  CQChartsWindowMgr();

 private:
  using Windows = std::vector<CQChartsWindow *>;

  Windows windows_;
};

//---

/*!
 * \brief Widget to hold view and associated controls
 * \ingroup Charts
 */
class CQChartsWindow : public QFrame {
  Q_OBJECT

 public:
  CQChartsWindow(CQChartsView *view);
 ~CQChartsWindow();

  CQChartsView *view() const { return view_; }

  //---

  bool isXRangeMap() const { return xRangeMap_; }
  void setXRangeMap(bool b);

  bool isYRangeMap() const { return yRangeMap_; }
  void setYRangeMap(bool b);

  void updateRangeMap();

  //---

  bool isDataTable() const { return dataTable_; }
  void setDataTable(bool b);

  //---

  bool isViewSettings() const { return viewSettings_; }
  void setViewSettings(bool b);

  //---

  void updateInterfacePalette();
  void updateThemePalettes();

  void selectPropertyObjects();

  //---

  void resizeEvent(QResizeEvent *) override;

  QSize sizeHint() const override;

 private:
  CQChartsPlot *objectPlot(QObject *obj) const;

 signals:
  void interfacePaletteChanged();
  void themePalettesChanged();

 private slots:
  void rangeScrollSlot();

  void filterAndSlot(bool b);

  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

  void filterChangedSlot();

  void removeViewSlot(CQChartsView *view);

  void plotSlot();
  void modeSlot();
  void selectModeSlot();

  void setStatusText(const QString &text);
  void setPosText   (const QString &text);
  void setSelText   (const QString &text);

  void propertyItemSelected(QObject *obj, const QString &path);

 private:
  CQChartsView*              view_         { nullptr }; //!< parent view
  bool                       xRangeMap_    { false };   //!< xrange map
  bool                       yRangeMap_    { false };   //!< xrange map
  bool                       dataTable_    { false };   //!< data table
  bool                       viewSettings_ { true };    //!< view settings
  CQChartsWindowRangeScroll* xrangeScroll_ { nullptr }; //!< xrange scroll
  CQChartsWindowRangeScroll* yrangeScroll_ { nullptr }; //!< yrange scroll
  CQChartsViewSettings*      settings_     { nullptr }; //!< settings widget
  QFrame*                    tableFrame_   { nullptr }; //!< table frame
  QStackedWidget*            viewStack_    { nullptr }; //!< view stack
  CQChartsFilterEdit*        filterEdit_   { nullptr }; //!< filter edit
  CQChartsModelViewHolder*   modelView_    { nullptr }; //!< model view
  CQChartsViewToolBar*       toolbar_      { nullptr }; //!< toolbar
  CQChartsViewStatus*        status_       { nullptr }; //!< status
};

//-----

/*!
 * \brief Range scroll control for window
 * \ingroup Charts
 */
class CQChartsWindowRangeScroll : public CQRangeScroll {
  Q_OBJECT

 public:
  CQChartsWindowRangeScroll(CQChartsWindow *window, Qt::Orientation orientation);

  void drawBackground(QPainter *) override;

 private:
  CQChartsWindow* window_ { nullptr };
};

#endif
