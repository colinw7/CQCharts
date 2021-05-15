#ifndef CQChartsWindow_H
#define CQChartsWindow_H

#include <CQRangeScroll.h>
#include <QFrame>
#include <QModelIndex>

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
  using Window = CQChartsWindow;
  using View   = CQChartsView;

 public:
  static CQChartsWindowMgr *instance();

  Window *createWindow(View *view);
  void removeWindow(Window *window);

 ~CQChartsWindowMgr();

  Window *getWindowForView(View *view) const;

 private:
  CQChartsWindowMgr();

 private:
  using Windows = std::vector<Window *>;

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
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQChartsWindow(View *view);
 ~CQChartsWindow();

  View *view() const { return view_; }

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

  void updateThemePalettes();

  void selectPropertyObjects();

  //---

  void connectModelViewExpand(bool connect);

  bool isExpandModelIndex(const QModelIndex &ind) const;
  void expandModelIndex(const QModelIndex &ind, bool b);

  void expandedModelIndices(QModelIndexList &inds);

  //---

  void setViewModel();

  void showErrorsTab();

  void setHasErrors(bool b);

  //---

  void resizeEvent(QResizeEvent *) override;

  QSize sizeHint() const override;

 private:
  Plot *objectPlot(QObject *obj) const;

 signals:
  void interfacePaletteChanged();
  void themePalettesChanged();

  void expansionChanged();

 private slots:
  void rangeScrollSlot();

  void filterAndSlot(bool b);

  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

  void filterChangedSlot();

  void expansionChangeSlot();

  void removeViewSlot(CQChartsView *view);

  void plotSlot();
  void modeSlot();
  void selectModeSlot();

  void setStatusText(const QString &text);
  void setPosText   (const QString &text);
  void setSelText   (const QString &text);

  void propertyItemSelected(QObject *obj, const QString &path);

 private:
  using RangeScroll     = CQChartsWindowRangeScroll;
  using ViewSettings    = CQChartsViewSettings;
  using FilterEdit      = CQChartsFilterEdit;
  using ModelViewHolder = CQChartsModelViewHolder;
  using ToolBar         = CQChartsViewToolBar;
  using Status          = CQChartsViewStatus;

  View*            view_         { nullptr }; //!< parent view
  bool             xRangeMap_    { false };   //!< xrange map
  bool             yRangeMap_    { false };   //!< xrange map
  bool             dataTable_    { false };   //!< data table
  bool             viewSettings_ { true };    //!< view settings
  RangeScroll*     xrangeScroll_ { nullptr }; //!< xrange scroll
  RangeScroll*     yrangeScroll_ { nullptr }; //!< yrange scroll
  ViewSettings*    settings_     { nullptr }; //!< settings widget
  QFrame*          tableFrame_   { nullptr }; //!< table frame
  QStackedWidget*  viewStack_    { nullptr }; //!< view stack
  FilterEdit*      filterEdit_   { nullptr }; //!< filter edit
  ModelViewHolder* modelView_    { nullptr }; //!< model view
  ToolBar*         toolbar_      { nullptr }; //!< toolbar
  Status*          status_       { nullptr }; //!< status
};

//-----

/*!
 * \brief Range scroll control for window
 * \ingroup Charts
 */
class CQChartsWindowRangeScroll : public CQRangeScroll {
  Q_OBJECT

 public:
  using Window = CQChartsWindow;

 public:
  CQChartsWindowRangeScroll(Window *window, Qt::Orientation orientation);

  void drawBackground(QPainter *) override;

 private:
  Window* window_ { nullptr };
};

#endif
