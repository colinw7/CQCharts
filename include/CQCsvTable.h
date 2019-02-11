#ifndef CQCsvTable_H
#define CQCsvTable_H

#include <CQTableView.h>

class CQCsvModel;
class QSortFilterProxyModel;

class CQCsvTable : public CQTableView {
  Q_OBJECT

 public:
  CQCsvTable(QWidget *parent=nullptr);
 ~CQCsvTable();

  void setModel(CQCsvModel *model);

 private:
  CQCsvModel*            model_ { nullptr };
  QSortFilterProxyModel* proxy_ { nullptr };
};

#endif
