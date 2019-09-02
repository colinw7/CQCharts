#ifndef CQChartsTableDelegate_H
#define CQChartsTableDelegate_H

#include <CQChartsUtil.h>
#include <CQBaseModelTypes.h>
#include <QItemDelegate>
#include <future>

class CQCharts;
class CQChartsTable;
class CQChartsTree;
class CQChartsModelView;
class CQChartsColor;
class CQChartsSymbol;
class CQChartsModelDetails;
class CQChartsModelColumnDetails;
class QPainter;

/*!
 * \brief Custom delegate for table view
 * \ingroup Charts
 */
class CQChartsTableDelegate : public QItemDelegate {
 public:
  struct ColumnData {
    CQChartsModelColumnDetails* details { nullptr };
  };

  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsTableDelegate(CQChartsTable *table);
  CQChartsTableDelegate(CQChartsTree *tree);
  CQChartsTableDelegate(CQChartsModelView *tree);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &item,
                        const QModelIndex &index) const override;

  void click(const QModelIndex &index) const;

  void getColumnData(const QModelIndex &index, ColumnData &data) const;

  void resetColumnData();

  void drawCheckInside(QPainter *painter, const QStyleOptionViewItem &option,
                       bool checked, const QModelIndex &index) const;

  void drawColor(QPainter *painter, const QStyleOptionViewItem &option,
                 const CQChartsColor &c, const QModelIndex &index) const;

  void drawSymbol(QPainter *painter, const QStyleOptionViewItem &option,
                  const CQChartsSymbol &symbol, const QModelIndex &index) const;

  void drawString(QPainter *painter, const QStyleOptionViewItem &option, const QString &str,
                  const QModelIndex &index) const;

  void clearColumnTypes();

 private slots:
  void updateBoolean();

 private:
  bool drawType(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;

  CQCharts *charts() const;

  ModelP modelP() const;

  CQChartsModelDetails *getDetails() const;

 private:
  using ColumnDataMap = std::map<int,ColumnData>;

  CQChartsTable*      table_ { nullptr };
  CQChartsTree*       tree_  { nullptr };
  CQChartsModelView*  view_  { nullptr };
  ColumnDataMap       columnDataMap_;
  mutable QModelIndex currentIndex_;
  mutable std::mutex  mutex_;
};

#endif
