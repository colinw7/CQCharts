#ifndef CQPropertyViewIntegerType_H
#define CQPropertyViewIntegerType_H

#include <CQPropertyViewType.h>

// type for int
class CQPropertyViewIntegerType : public CQPropertyViewType {
 public:
  CQPropertyViewIntegerType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;
};

//------

#include <CQPropertyViewEditor.h>

// editor factory for int
class CQPropertyViewIntegerEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewIntegerEditor(int min=INT_MIN, int max=INT_MAX, int step=1);

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  int min_  { INT_MIN };
  int max_  { INT_MAX };
  int step_ { 1 };
};

#endif
