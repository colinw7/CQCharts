#ifndef CQChartsTableDelegate_H
#define CQChartsTableDelegate_H

#include <CQChartsUtil.h>
#include <CQBaseModelTypes.h>
#include <QItemDelegate>
#include <future>

class CQChartsTable;
class CQChartsColor;
class CQChartsSymbol;
class QPainter;

class CQChartsTableDelegate : public QItemDelegate {
 public:
  struct ColumnData {
    CQBaseModelType    type;
    CQBaseModelType    baseType;
    CQChartsNameValues nameValues;
  };

 public:
  CQChartsTableDelegate(CQChartsTable *table);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const;

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &item,
                        const QModelIndex &index) const;

  void click(const QModelIndex &index) const;

  void getColumnData(const QModelIndex &index, ColumnData &data) const;

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
  using ColumnDataMap = std::map<int,ColumnData>;

  CQChartsTable*      table_ { nullptr };
  ColumnDataMap       columnDataMap_;
  mutable QModelIndex currentIndex_;
  mutable std::mutex  mutex_;
};

#endif
