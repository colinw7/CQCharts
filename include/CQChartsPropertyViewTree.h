#ifndef CQChartsPropertyViewTree_H
#define CQChartsPropertyViewTree_H

#include <CQPropertyViewTree.h>

class CQChartsViewSettings;
class CQPropertyViewModel;
class CQChartsEditTitleDlg;
class CQChartsEditKeyDlg;
class CQChartsEditAxisDlg;

/*!
 * \brief Charts Property View Tree
 * \ingroup Charts
 */
class CQChartsPropertyViewTree : public CQPropertyViewTree {
  Q_OBJECT

  Q_PROPERTY(bool filterDisplayed READ isFilterDisplayed WRITE setFilterDisplayed)
  Q_PROPERTY(bool showStyleItems  READ isShowStyleItems  WRITE setShowStyleItems )

 public:
  CQChartsPropertyViewTree(CQChartsViewSettings *settings, CQPropertyViewModel *model);

  void addMenuItems(QMenu *menu) override;

  void printItem(CQPropertyViewItem *item) const;

  bool isFilterDisplayed() const { return filterDisplayed_; }
  void setFilterDisplayed(bool visible, bool focus=false);

  bool isShowStyleItems() const { return showStyleItems_; }
  void setShowStyleItems(bool b);

  void keyPressEvent(QKeyEvent *e) override;

 signals:
  void filterStateChanged(bool show, bool focus);

 private slots:
  void editSlot();

  void showHideFilterSlot(bool b);
  void showHideStyleItemsSlot(bool b);

 private:
  void showStyleItems(CQPropertyViewItem *item, bool show);

 private:
  CQChartsViewSettings *settings_        { nullptr };
  CQChartsEditTitleDlg *titleDlg_        { nullptr };
  CQChartsEditKeyDlg*   keyDlg_          { nullptr };
  CQChartsEditAxisDlg*  axisDlg_         { nullptr };
  bool                  filterDisplayed_ { false };
  bool                  showStyleItems_  { true };
};

#endif
