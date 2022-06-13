#ifndef CQChartsAnnotationsTable_H
#define CQChartsAnnotationsTable_H

#include <CQChartsAnnotationsTable.h>
#include <CQChartsPenBrush.h>
#include <CQChartsWidgetIFace.h>

#include <CQTableWidget.h>

#include <QItemDelegate>

class CQChartsViewAnnotationsControl;
class CQChartsPlotAnnotationsControl;
class CQChartsViewAnnotationsTable;
class CQChartsPlotAnnotationsTable;
class CQChartsAnnotationsTable;
class CQChartsView;
class CQChartsPlot;
class CQChartsAnnotation;
class CQChartsCreateAnnotationDlg;
class CQChartsEditAnnotationDlg;

class CQTabSplit;
class QPushButton;

//---

/*!
 * \brief annotation table delegate
 */
class CQChartsAnnotationDelegate : public QItemDelegate {
 public:
  CQChartsAnnotationDelegate(CQChartsAnnotationsTable *table);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  CQChartsAnnotationsTable *table() const { return table_; }

 private:
  CQChartsAnnotationsTable *table_ { nullptr };
};

//---

/*!
 * \brief annotation control widget
 */
class CQChartsAnnotationsControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsAnnotationsControl(QWidget *parent=nullptr);

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view) override;

  CQChartsPlot *plot() const;
  void setPlot(CQChartsPlot *plot) override;

 private slots:
  void updateAnnotations();

  void writeAnnotationSlot();

 private:
  using PlotP = QPointer<CQChartsPlot>;

  CQTabSplit *split_ { nullptr };

  CQChartsView *view_ { nullptr };
  PlotP         plot_;

  CQChartsViewAnnotationsControl* viewTable_ { nullptr }; //!< view annotations table
  CQChartsPlotAnnotationsControl* plotTable_ { nullptr }; //!< plot annotations table
};

//---

/*!
 * \brief view annotation control widget
 */
class CQChartsViewAnnotationsControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsViewAnnotationsControl(QWidget *parent=nullptr);

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view) override;

 private:
  CQChartsAnnotation *getSelectedViewAnnotation() const;

 private slots:
  void updateViewAnnotations();

  void viewAnnotationSelectionChangeSlot();
  void raiseViewAnnotationSlot();
  void lowerViewAnnotationSlot();
  void createViewAnnotationSlot();
  void editViewAnnotationSlot();
  void removeViewAnnotationsSlot();

 private:
  using ViewAnnotationsTable = CQChartsViewAnnotationsTable;
  using CreateAnnotationDlg  = CQChartsCreateAnnotationDlg;
  using EditAnnotationDlg    = CQChartsEditAnnotationDlg;

  CQChartsView *view_ { nullptr };

  ViewAnnotationsTable* viewTable_        { nullptr }; //!< view annotations table
  QPushButton*          viewRaiseButton_  { nullptr }; //!< view annotation raise button
  QPushButton*          viewLowerButton_  { nullptr }; //!< view annotation lower button
  QPushButton*          viewCreateButton_ { nullptr }; //!< view annotation create button
  QPushButton*          viewEditButton_   { nullptr }; //!< view annotation edit button
  QPushButton*          viewRemoveButton_ { nullptr }; //!< view annotation remove button

  CreateAnnotationDlg* createAnnotationDlg_ { nullptr }; //!< create annotation dialog
  EditAnnotationDlg*   editAnnotationDlg_   { nullptr }; //!< edit annotation dialog
};

//---

/*!
 * \brief plot annotation control widget
 */
class CQChartsPlotAnnotationsControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsPlotAnnotationsControl(QWidget *parent=nullptr);

  CQChartsPlot *plot() const;
  void setPlot(CQChartsPlot *plot) override;

 private:
  CQChartsAnnotation *getSelectedPlotAnnotation() const;

 private slots:
  void updatePlotAnnotations();

  void plotAnnotationSelectionChangeSlot();
  void raisePlotAnnotationSlot();
  void lowerPlotAnnotationSlot();
  void createPlotAnnotationSlot();
  void editPlotAnnotationSlot();
  void removePlotAnnotationsSlot();

 private:
  using PlotAnnotationsTable = CQChartsPlotAnnotationsTable;
  using CreateAnnotationDlg  = CQChartsCreateAnnotationDlg;
  using EditAnnotationDlg    = CQChartsEditAnnotationDlg;

  using PlotP = QPointer<CQChartsPlot>;

  PlotP plot_;

  PlotAnnotationsTable* plotTable_        { nullptr }; //!< plot annotations table
  QPushButton*          plotRaiseButton_  { nullptr }; //!< plot annotation raise button
  QPushButton*          plotLowerButton_  { nullptr }; //!< plot annotation lower button
  QPushButton*          plotCreateButton_ { nullptr }; //!< plot annotation create button
  QPushButton*          plotEditButton_   { nullptr }; //!< plot annotation edit button
  QPushButton*          plotRemoveButton_ { nullptr }; //!< plot annotation remove button
  QPushButton*          writeButton_      { nullptr }; //!< view and plot annotations write button

  CreateAnnotationDlg* createAnnotationDlg_ { nullptr }; //!< create annotation dialog
  EditAnnotationDlg*   editAnnotationDlg_   { nullptr }; //!< edit annotation dialog
};

//---

/*!
 * \brief annotations table widget
 */
class CQChartsAnnotationsTable : public CQTableWidget {
 public:
  using Annotations = std::vector<CQChartsAnnotation *>;

 public:
  CQChartsAnnotationsTable(CQChartsView *view=nullptr, CQChartsPlot *plot=nullptr);
 ~CQChartsAnnotationsTable();

  void setView(CQChartsView *view) { view_ = view; plot_ = nullptr; }
  void setPlot(CQChartsPlot *plot) { plot_ = plot; view_ = nullptr; }

  void addHeaderItems();

  QTableWidgetItem *createItem(const QString &name, int r, int c,
                               CQChartsAnnotation *annotation);

  void getSelectedAnnotations(Annotations &annotations);

  CQChartsAnnotation *itemAnnotation(QTableWidgetItem *item) const;

 protected:
  CQChartsView*               view_     { nullptr };
  CQChartsPlot*               plot_     { nullptr };
  CQChartsAnnotationDelegate* delegate_ { nullptr };
};

//---

/*!
 * \brief view annotations table widget
 */
class CQChartsViewAnnotationsTable : public CQChartsAnnotationsTable, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsViewAnnotationsTable();

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view) override;

 public slots:
  void updateAnnotations();

 private:
  CQChartsView *view_ { nullptr };
};

//--

/*!
 * \brief plot annotations table widget
 */
class CQChartsPlotAnnotationsTable : public CQChartsAnnotationsTable, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsPlotAnnotationsTable();

  CQChartsPlot *plot() const;
  void setPlot(CQChartsPlot *plot) override;

 public slots:
  void updateAnnotations();

 private:
  using PlotP = QPointer<CQChartsPlot>;

  PlotP plot_;
};

#endif
