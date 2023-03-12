#ifndef CQChartsPropertyViewTree_H
#define CQChartsPropertyViewTree_H

#include <CQPropertyViewTree.h>

class CQChartsEditTitleDlg;
class CQChartsEditKeyDlg;
class CQChartsEditAxisDlg;
class CQChartsView;
class CQChartsPlot;

class CQPropertyViewModel;

/*!
 * \brief Charts Property View Tree
 * \ingroup Charts
 */
class CQChartsPropertyViewTree : public CQPropertyViewTree {
  Q_OBJECT

  Q_PROPERTY(bool filterDisplayed READ isFilterDisplayed WRITE setFilterDisplayed)
  Q_PROPERTY(bool showStyleItems  READ isShowStyleItems  WRITE setShowStyleItems )

 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQChartsPropertyViewTree(QWidget *parent, CQPropertyViewModel *model);

  void setView(View *view);
  void setPlot(Plot *plot);

  void addMenuItems(QMenu *menu) override;

  void printItem(CQPropertyViewItem *item) const;

  bool isFilterDisplayed() const { return filterDisplayed_; }
  void setFilterDisplayed(bool visible, bool focus=false);

  bool isShowStyleItems() const { return showStyleItems_; }
  void setShowStyleItems(bool b);

  void initEditor(QWidget *w) override;

  void keyPressEvent(QKeyEvent *e) override;

 Q_SIGNALS:
  void filterStateChanged(bool show, bool focus);

 private Q_SLOTS:
  void editSlot();

  void showHideFilterSlot(bool b);
  void showHideStyleItemsSlot(bool b);

 private:
  void showStyleItems(CQPropertyViewItem *item, bool show);

 private:
  CQChartsEditTitleDlg *titleDlg_        { nullptr };
  CQChartsEditKeyDlg*   keyDlg_          { nullptr };
  CQChartsEditAxisDlg*  axisDlg_         { nullptr };
  bool                  filterDisplayed_ { false };
  bool                  showStyleItems_  { true };
  View*                 view_            { nullptr };
  Plot*                 plot_            { nullptr };
};

#endif
