#ifndef CQPropertyViewPointFType_H
#define CQPropertyViewPointFType_H

#include <CQPropertyViewType.h>

// type for QPointF
class CQPropertyViewPointFType : public CQPropertyViewType {
 public:
  CQPropertyViewPointFType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const;
};

//------

#include <CQPropertyViewEditor.h>

// editor factory for QPointF
class CQPropertyViewPointFEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewPointFEditor(double min=-1E50, double max=1E50, double step=1.0);

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  double min_, max_;
  double step_;
};

#endif
