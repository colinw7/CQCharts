#ifndef CQChartsPlotPropertiesWidget_H
#define CQChartsPlotPropertiesWidget_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>
#include <QPointer>

#include <set>

class CQChartsPlot;
class CQChartsPropertyViewTree;
class CQChartsPropertyViewTreeFilterEdit;
class CQChartsEditTitleDlg;
class CQChartsEditKeyDlg;
class CQChartsEditAxisDlg;

class CQTabWidget;

/*!
 * \brief Plot properties widget
 * \ingroup Charts
 */
class CQChartsPlotPropertiesWidget : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsPlotPropertiesWidget(Plot *plot=nullptr);

  Plot *plot() const;
  void setPlot(Plot *plot) override;

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 Q_SIGNALS:
  void propertyItemSelected(QObject *obj, const QString &path);

 private Q_SLOTS:
  void filterStateSlot(bool show, bool focus);

 private:
  using PlotP = QPointer<Plot>;

  PlotP                               plot_;
  CQChartsPropertyViewTree*           propertyTree_ { nullptr };
  CQChartsPropertyViewTreeFilterEdit* filterEdit_   { nullptr };
};

//---

class CQChartsPlotPropertiesControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using View                 = CQChartsView;
  using Plot                 = CQChartsPlot;
  using PlotSet              = std::set<Plot *>;
  using PlotPropertiesWidget = CQChartsPlotPropertiesWidget;

 public:
  CQChartsPlotPropertiesControl(View *view=nullptr);

  View *view() const;
  void setView(View *view) override;

  void updatePlots();

  CQChartsPropertyViewTree *getPlotPropertyViewTree(Plot *plot, int &ind) const;

  int addPlotPropertyViewTree(Plot *plot);

  void updatePlotPropertyViewTrees(const PlotSet &plotSet);

  Plot *getCurrentPlot() const;

 Q_SIGNALS:
  void propertyItemSelected(QObject *obj, const QString &path);

 private Q_SLOTS:
  void tabChangedSlot();

  void editTitleSlot();
  void editKeySlot();
  void editXAxisSlot();
  void editYAxisSlot();
  void writeSlot();

 private:
  using ViewP        = QPointer<View>;
  using EditTitleDlg = CQChartsEditTitleDlg;
  using EditKeyDlg   = CQChartsEditKeyDlg;
  using EditAxisDlg  = CQChartsEditAxisDlg;

  ViewP view_;

  CQTabWidget*  plotsTab_     { nullptr }; //!< plot properties tab widget
  EditTitleDlg* editTitleDlg_ { nullptr }; //!< edit plot title dialog
  EditKeyDlg*   editKeyDlg_   { nullptr }; //!< edit plot key dialog
  EditAxisDlg*  editXAxisDlg_ { nullptr }; //!< edit x axis dialog
  EditAxisDlg*  editYAxisDlg_ { nullptr }; //!< edit y axis dialog
};

#endif
