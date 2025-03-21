#ifndef CQChartsWindow_H
#define CQChartsWindow_H

#include <CQRangeScroll.h>

#include <QFrame>
#include <QModelIndex>
#include <QPointer>

class CQChartsTabWindow;
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

class CQTabSplit;
class QStackedWidget;
class QSplitter;
class QComboBox;

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

  CQChartsTabWindow *getTabWindow();

  Window *getWindowForView(View *view) const;

 private:
  CQChartsWindowMgr();

 private:
  using Windows = std::vector<Window *>;

  CQChartsTabWindow *tabWindow_ { nullptr };
  Windows            windows_;
};

//---

class CQChartsTabWindow : public QFrame {
  Q_OBJECT

 public:
  using Window = CQChartsWindow;

 public:
  CQChartsTabWindow();

  void addWindow(CQChartsWindow *window);

  QSize sizeHint() const override;

 private:
  void setCurrentWindow(Window *window);

 private Q_SLOTS:
  void currentIndexSlot(int);

 private:
  QSplitter*  split_         { nullptr };
  CQTabSplit* windowsSplit_  { nullptr };
  QFrame*     settingsArea_  { nullptr };
  Window*     currentWindow_ { nullptr };
};

//---

/*!
 * \brief Widget to hold view and associated controls
 * \ingroup Charts
 */
class CQChartsWindow : public QFrame {
  Q_OBJECT

 public:
  using View         = CQChartsView;
  using Plot         = CQChartsPlot;
  using ViewSettings = CQChartsViewSettings;

 public:
  CQChartsWindow(View *view);
 ~CQChartsWindow();

  View *view() const;

  ViewSettings *viewSettings() { return settings_; }

  //---

  bool isXRangeMap() const { return xRangeMap_; }
  void setXRangeMap(bool b);

  bool isYRangeMap() const { return yRangeMap_; }
  void setYRangeMap(bool b);

  void updateRangeMap();

  //---

  bool isDataTable() const { return showDataTable_; }
  void setDataTable(bool b, bool force=false);

  //---

  bool isViewSettings() const { return viewSettingsData_.visible; }
  void setViewSettings(bool b);

  bool isViewSettingsMajorObjects() const { return viewSettingsData_.majorObjects; }
  void setViewSettingsMajorObjects(bool b);

  bool isViewSettingsMinorObjects() const { return viewSettingsData_.minorObjects; }
  void setViewSettingsMinorObjects(bool b);

  int viewSettingsMaxObjects() const { return viewSettingsData_.maxObjects; }
  void setViewSettingsMaxObjects(int n);

  //---

  void updateThemePalettes();

  void selectPropertyObjects();

  //---

  void connectModelViewExpand(bool connect);

  bool isExpandModelIndex(const QModelIndex &ind) const;
  void expandModelIndex(const QModelIndex &ind, bool b);

  void expandedModelIndices(QModelIndexList &inds);

  //---

  void updateOverview();

  //---

  void setViewModel();

  void showErrorsTab();

  void setHasErrors(bool b);

  //---

  void resizeEvent(QResizeEvent *) override;

  //---

  QSize sizeHint() const override;

 private:
  Plot *objectPlot(QObject *obj) const;

  void addModelView(bool current);

 Q_SIGNALS:
  void interfacePaletteChanged();
  void themePalettesChanged();

  void expansionChanged();

 private Q_SLOTS:
  void rangeScrollSlot();

  void stackSlot(int ind);

  void filterAndSlot(bool b);

  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

  void filterChangedSlot();

  void expansionChangeSlot();

  void removeViewSlot(CQChartsView *view);

  void plotSlot();
  void plotModelSlot();
  void modeSlot();
  void selectModeSlot();

  void setStatusText(const QString &text);
  void setPosText   (const QString &text);
  void setSelText   (const QString &text);

  void propertyItemSelected(QObject *obj, const QString &path);

 private:
  using ViewP           = QPointer<View>;
  using PlotP           = QPointer<Plot>;
  using RangeScroll     = CQChartsWindowRangeScroll;
  using FilterEdit      = CQChartsFilterEdit;
  using ModelViewHolder = CQChartsModelViewHolder;
  using ToolBar         = CQChartsViewToolBar;
  using Status          = CQChartsViewStatus;

  ViewP view_;                    //!< parent view
  PlotP plot_;                    //!< current plot
  bool  xRangeMap_     { false }; //!< xrange map
  bool  yRangeMap_     { false }; //!< xrange map
  bool  showDataTable_ { false }; //!< data table

  struct ViewSettingsData {
    bool visible      { true };  //!< view settings visible
    bool majorObjects { true };  //!< view settings major objects visible
    bool minorObjects { false }; //!< view settings minor objects visible
    int  maxObjects   { 512 };   //!< max number of objects to add
  };

  ViewSettingsData viewSettingsData_;
  ViewSettings*    settings_ { nullptr }; //!< settings widget

  RangeScroll* xrangeScroll_ { nullptr }; //!< xrange scroll
  RangeScroll* yrangeScroll_ { nullptr }; //!< yrange scroll

  CQTabSplit* viewSplitter_ { nullptr }; //!< view splitter

  ToolBar* toolbar_ { nullptr }; //!< toolbar
  Status*  status_  { nullptr }; //!< status

  struct TableData {
    bool             isCurrent  { false };   //!< is current plot model view
    QFrame*          tableFrame { nullptr }; //!< table frame
    FilterEdit*      filterEdit { nullptr }; //!< filter edit
    ModelViewHolder* modelView  { nullptr }; //!< model view
  };

  using TableDatas = std::vector<TableData>;

  QFrame*         tableFrame_    { nullptr };
  QFrame*         modelSelFrame_ { nullptr };
  QComboBox*      stackCombo_    { nullptr };
  QStackedWidget* tableStack_    { nullptr };
  TableDatas      tableDatas_;
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
