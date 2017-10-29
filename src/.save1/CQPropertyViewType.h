#ifndef CQPropertyViewType_H
#define CQPropertyViewType_H

#include <QString>

class CQPropertyViewEditorFactory;
class CQPropertyViewDelegate;
class CQPropertyViewItem;

class QStyleOptionViewItem;
class QPainter;
class QModelIndex;
class QVariant;

// type base class
//
// support:
//  . create editor
//  . delegate draw
class CQPropertyViewType {
 public:
  CQPropertyViewType() { }

  virtual ~CQPropertyViewType() { }

  virtual CQPropertyViewEditorFactory *getEditor() const = 0;

  virtual bool setEditorData(CQPropertyViewItem *item, const QVariant &value);

  virtual void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
                    const QStyleOptionViewItem &option, const QModelIndex &index,
                    const QVariant &value, bool inside);

  virtual QString tip(const QVariant &value) const;
};

#endif
