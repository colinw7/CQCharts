#ifndef CQChartsTableDelegate_H
#define CQChartsTableDelegate_H

#include <CQChartsUtil.h>
#include <CQChartsModelTypes.h>
#include <QItemDelegate>
#include <future>

class CQCharts;
class CQChartsTable;
class CQChartsTree;
class CQChartsModelView;
class CQChartsColor;
class CQChartsSymbol;
class CQChartsImage;
class CQChartsModelDetails;
class CQChartsModelColumnDetails;
class QPainter;

/*!
 * \brief Custom delegate for table view
 * \ingroup Charts
 */
class CQChartsTableDelegate : public QItemDelegate {
 public:
  //! column details data
  struct ColumnData {
    CQChartsModelColumnDetails* details { nullptr };
  };

  using ModelP = QSharedPointer<QAbstractItemModel>;

  using Table     = CQChartsTable;
  using Tree      = CQChartsTree;
  using ModelView = CQChartsModelView;
  using Color     = CQChartsColor;
  using Symbol    = CQChartsSymbol;
  using Image     = CQChartsImage;

 public:
  CQChartsTableDelegate(Table *table);
  CQChartsTableDelegate(Tree *tree);
  CQChartsTableDelegate(ModelView *view);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &item,
                        const QModelIndex &index) const override;

  void click(const QModelIndex &index) const;

  void getColumnData(const QModelIndex &index, ColumnData &data) const;

  void resetColumnData();

  void drawCheckInside(QPainter *painter, const QStyleOptionViewItem &option,
                       bool checked, const QModelIndex &index) const;

  void drawColor(QPainter *painter, const QStyleOptionViewItem &option,
                 const Color &c, const QModelIndex &index) const;

  void drawSymbol(QPainter *painter, const QStyleOptionViewItem &option,
                  const Symbol &symbol, const QModelIndex &index) const;

  void drawImage(QPainter *painter, const QStyleOptionViewItem &option,
                 const Image &image, const QModelIndex &index) const;

  bool drawNullValue(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const;

  void drawString(QPainter *painter, const QStyleOptionViewItem &option, const QString &str,
                  const QModelIndex &index) const;

  void clearColumnTypes();

 private Q_SLOTS:
  void updateBoolean();

 private:
  using ModelDetails = CQChartsModelDetails;

 private:
  void init();

  bool drawType(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;

  bool typeSizeHint(const QStyleOptionViewItem &option,
                    const QModelIndex &index, QSize &size) const;

  CQCharts *charts() const;

  ModelP modelP() const;

  ModelDetails *getDetails() const;

 private:
  using ColumnDataMap = std::map<int, ColumnData>;

  Table*              table_ { nullptr };
  Tree*               tree_  { nullptr };
  ModelView*          view_  { nullptr };
  ColumnDataMap       columnDataMap_;
  mutable QModelIndex currentIndex_;
  mutable std::mutex  mutex_;
};

#endif
