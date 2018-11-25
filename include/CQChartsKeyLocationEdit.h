#ifndef CQChartsKeyLocationEdit_H
#define CQChartsKeyLocationEdit_H

#include <CQChartsKeyLocation.h>
#include <QFrame>

class CQRealSpin;
class QComboBox;

class CQChartsKeyLocationEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsKeyLocationEdit(QWidget *parent=nullptr);

  const CQChartsKeyLocation &keyLocation() const;
  void setKeyLocation(const CQChartsKeyLocation &pos);

 signals:
  void keyLocationChanged();

 private slots:
  void comboChanged();

 private:
  CQChartsKeyLocation keyLocation_;
  QComboBox*          combo_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsKeyLocation
class CQChartsKeyLocationPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsKeyLocationPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsKeyLocation
class CQChartsKeyLocationPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsKeyLocationPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
