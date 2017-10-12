#ifndef CQPropertyViewSizeFType_H
#define CQPropertyViewSizeFType_H

#include <CQPropertyViewType.h>

// type for QSizeF
class CQPropertyViewSizeFType : public CQPropertyViewType {
 public:
  CQPropertyViewSizeFType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const;
};

//------

#include <CQPropertyViewEditor.h>

// editor factory for QSizeF
class CQPropertyViewSizeFEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewSizeFEditor(double max=1000, double step=1.0);

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  double max_;
  double step_;
};

#endif
