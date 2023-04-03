#ifndef CQChartsValueListEdit_H
#define CQChartsValueListEdit_H

#include <CQChartsValueList.h>
#include <QFrame>

class QComboBox;

/*!
 * \brief value list editor
 * \ingroup Charts
 */
class CQChartsValueListEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsValueListEdit(QWidget *parent=nullptr);

  const CQChartsValueList &valueList() const { return valueList_; }
  void setValueList(const CQChartsValueList &v);

 private:
  void connectSlots(bool b);

 Q_SIGNALS:
  void currentIndChanged();

 protected Q_SLOTS:
  void comboChanged(int);

 protected:
  CQChartsValueList valueList_;
  QComboBox*        combo_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsValueList
 * \ingroup Charts
 */
class CQChartsValueListPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsValueListPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "value_list"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsValueList
 * \ingroup Charts
 */
class CQChartsValueListPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsValueListPropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
