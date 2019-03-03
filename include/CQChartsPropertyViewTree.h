#ifndef CQChartsPropertyViewTree_H
#define CQChartsPropertyViewTree_H

#include <CQPropertyViewTree.h>

class CQChartsViewSettings;
class CQPropertyViewModel;
class CQChartsEditTitleDlg;
class CQChartsEditKeyDlg;
class CQChartsEditAxisDlg;

class CQChartsPropertyViewTree : public CQPropertyViewTree {
  Q_OBJECT

 public:
  CQChartsPropertyViewTree(CQChartsViewSettings *settings, CQPropertyViewModel *model);

  void addMenuItems(QMenu *menu) override;

  void printItem(CQPropertyViewItem *item) const;

 private slots:
  void editSlot();

 private:
  CQChartsViewSettings *settings_ { nullptr };
  CQChartsEditTitleDlg *titleDlg_ { nullptr };
  CQChartsEditKeyDlg*   keyDlg_   { nullptr };
  CQChartsEditAxisDlg*  axisDlg_  { nullptr };
};

#endif
