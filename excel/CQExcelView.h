#ifndef CQExcelView_H
#define CQExcelView_H

#include <CQModelView.h>

class QAbstractProxyModel;

namespace CQExcel {

class Frame;
class Model;

/*!
 * \brief Model View for Excel Grid
 */
class View : public CQModelView {
  Q_OBJECT

 public:
  View(Frame *frame, int nr=100, int nc=100);

  Frame *frame() const { return frame_; }

  Model *excelModel() const { return model_; }
  void setExcelModel(QAbstractItemModel *model);

  const QModelIndex &currentIndex() const { return currentIndex_; }

  QModelIndexList getSelectedIndexes() const;

  void applyFunction(const QString &text);

  void setCurrentIndex(const QModelIndex &ind);

  void addMenuActions(QMenu *menu) override;

  void getSelectionDetails(QString &cellName);

  void getIndexDetails(const QModelIndex &ind, QString &cellName, QString &functionText);

 Q_SIGNALS:
  void currentIndexChanged();

  void currentSelectionChanged();

 private Q_SLOTS:
  void updateCurrentIndex();

  void selectionSlot();

  void viewClickSlot(const QModelIndex &ind);

  void addRowSlot();
  void addColumnSlot();

  void setCurrentlot();

  void copySlot();
  void pasteSlot();

  void sumSlot();
  void averageSlot();
  void minSlot();
  void maxSlot();

  void badStyleSlot();
  void errorStyleSlot();

 private:
  void addRangeFunction(const QString &fnName);

 private:
  using Proxy = QAbstractProxyModel;

  Frame*          frame_ { nullptr };
  Model*          model_ { nullptr };
  Proxy*          proxy_ { nullptr };
  int             nr_    { 100 };
  int             nc_    { 100 };
  QModelIndex     currentIndex_;
  QModelIndexList copyIndices_;
};

}

#endif
