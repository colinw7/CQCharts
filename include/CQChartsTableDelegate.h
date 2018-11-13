#ifndef CQChartsTableDelegate_H
#define CQChartsTableDelegate_H

#include <CQBaseModel.h>
#include <QItemDelegate>

class CQChartsTable;
class CQChartsColor;
class CQChartsSymbol;
class QPainter;

class CQChartsTableDelegate : public QItemDelegate {
 public:
  CQChartsTableDelegate(CQChartsTable *table);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const;

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &item,
                        const QModelIndex &index) const;

  void click(const QModelIndex &index) const;

  CQBaseModel::Type getColumnType(const QModelIndex &index) const;

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
  using ColumnTypeMap = std::map<int,CQBaseModel::Type>;

  CQChartsTable*      table_ { nullptr };
  ColumnTypeMap       columnTypeMap_;
  mutable QModelIndex currentIndex_;
};

#endif
