#ifndef CQPropertyViewColorType_H
#define CQPropertyViewColorType_H

#include <CQPropertyViewType.h>

// type for QColor
class CQPropertyViewColorType : public CQPropertyViewType {
 public:
  CQPropertyViewColorType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;
};

//------

#include <CQPropertyViewEditor.h>

// editor factory for QColor
class CQPropertyViewColorEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewColorEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
