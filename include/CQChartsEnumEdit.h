#ifndef CQChartsEnumEdit_H
#define CQChartsEnumEdit_H

#include <QFrame>

class QComboBox;

class CQChartsEnumEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsEnumEdit(QWidget *parent=nullptr);

  virtual ~CQChartsEnumEdit() { }

  void init();

  void setEnumString(const QString &str);

  virtual QStringList enumNames() const = 0;

  virtual void setEnumFromString(const QString &str) = 0;

  virtual QVariant getVariantFromEnum() const = 0;

  virtual void setEnumFromVariant(const QVariant &str) = 0;

  virtual QString variantToString(const QVariant &var) const = 0;

  virtual void connect(QObject *obj, const char *method) = 0;

 signals:
  void enumChanged();

 protected slots:
  void comboChanged();

 protected:
  QComboBox* combo_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsEnum
class CQChartsEnumPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsEnumPropertyViewType();

  virtual ~CQChartsEnumPropertyViewType() { }

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;

  virtual QString variantToString(const QVariant &var) const = 0;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsEnum
class CQChartsEnumPropertyViewEditorFactory : public CQPropertyViewEditorFactory {
 public:
  CQChartsEnumPropertyViewEditorFactory();

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
