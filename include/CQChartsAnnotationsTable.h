#ifndef CQChartsAnnotationsTable_H
#define CQChartsAnnotationsTable_H

#include <CQChartsAnnotationsTable.h>
#include <CQChartsPenBrush.h>
#include <CQChartsWidgetIFace.h>

#include <CQTableWidget.h>

#include <QItemDelegate>
#include <QPointer>

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
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQChartsAnnotationsControl(QWidget *parent=nullptr);

  View *view() const;
  void setView(View *view);

  Plot *plot() const;
  void setPlot(Plot *plot) override;

 private Q_SLOTS:
  void updateAnnotations();

  void writeAnnotationSlot();

 private:
  using PlotP = QPointer<Plot>;
  using ViewP = QPointer<View>;

  CQTabSplit *split_ { nullptr };

  ViewP view_;
  PlotP plot_;

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
  using View = CQChartsView;

 public:
  CQChartsViewAnnotationsControl(QWidget *parent=nullptr);

  View *view() const;
  void setView(View *view);

 private:
  CQChartsAnnotation *getSelectedViewAnnotation() const;

 private Q_SLOTS:
  void updateViewAnnotations();

  void viewAnnotationSelectionChangeSlot();
  void raiseViewAnnotationSlot();
  void lowerViewAnnotationSlot();
  void createViewAnnotationSlot();
  void editViewAnnotationSlot();
  void removeViewAnnotationsSlot();

 private:
  using ViewP                = QPointer<View>;
  using ViewAnnotationsTable = CQChartsViewAnnotationsTable;
  using CreateAnnotationDlg  = CQChartsCreateAnnotationDlg;
  using EditAnnotationDlg    = CQChartsEditAnnotationDlg;

  ViewP view_;

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
  using Plot = CQChartsPlot;

 public:
  CQChartsPlotAnnotationsControl(QWidget *parent=nullptr);

  Plot *plot() const;
  void setPlot(Plot *plot) override;

 private:
  CQChartsAnnotation *getSelectedPlotAnnotation() const;

 private Q_SLOTS:
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

  using PlotP = QPointer<Plot>;

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
  using View        = CQChartsView;
  using Plot        = CQChartsPlot;
  using Annotations = std::vector<CQChartsAnnotation *>;

 public:
  CQChartsAnnotationsTable(View *view=nullptr, Plot *plot=nullptr);
 ~CQChartsAnnotationsTable();

  void setView(View *view);
  void setPlot(Plot *plot);

  void addHeaderItems();

  QTableWidgetItem *createItem(const QString &name, int r, int c,
                               CQChartsAnnotation *annotation);

  void getSelectedAnnotations(Annotations &annotations);

  CQChartsAnnotation *itemAnnotation(QTableWidgetItem *item) const;

 protected:
  using ViewP = QPointer<View>;
  using PlotP = QPointer<Plot>;

  ViewP                       view_;
  PlotP                       plot_;
  CQChartsAnnotationDelegate* delegate_ { nullptr };
};

//---

/*!
 * \brief view annotations table widget
 */
class CQChartsViewAnnotationsTable : public CQChartsAnnotationsTable, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using View = CQChartsView;

 public:
  CQChartsViewAnnotationsTable();

  View *view() const;
  void setView(View *view) override;

 public Q_SLOTS:
  void updateAnnotations();

 private:
  using ViewP = QPointer<View>;

  ViewP view_;
};

//--

/*!
 * \brief plot annotations table widget
 */
class CQChartsPlotAnnotationsTable : public CQChartsAnnotationsTable, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsPlotAnnotationsTable();

  Plot *plot() const;
  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateAnnotations();

 private:
  using PlotP = QPointer<Plot>;

  PlotP plot_;
};

#endif
