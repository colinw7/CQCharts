#ifndef CQExcelDelegate_H
#define CQExcelDelegate_H

#include <QItemDelegate>
#include <QPointer>

class QAbstractItemView;

/*!
 * Delegate Abstract Item View with support for:
 */
class CQExcelDelegate : public QItemDelegate {
  Q_OBJECT

 public:
  CQExcelDelegate(QAbstractItemView *view);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &ind) const;

 private:
  bool drawType(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;

  void drawColor(QPainter *painter, const QStyleOptionViewItem &option,
                 const QColor &color, const QModelIndex &index) const;

  bool typeSizeHint(const QStyleOptionViewItem &option, const QModelIndex &ind, QSize &size) const;

 private:
  QAbstractItemView* view_        { nullptr }; //!< parent view
  mutable bool       isEditable_  { false };   //!< is editable
  mutable bool       isMouseOver_ { false };   //!< is mouse over
};

#endif
