#ifndef CQChartsShapeTypeEdit_H
#define CQChartsShapeTypeEdit_H

#include <CQSwitchLineEdit.h>
#include <CQChartsShapeType.h>

class CQRealSpin;
class QComboBox;

/*!
 * \brief shape edit
 * \ingroup Charts
 */
class CQChartsShapeTypeEdit : public CQSwitchLineEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsShapeType shape READ shapeType WRITE setShapeType)

 public:
  CQChartsShapeTypeEdit(QWidget *parent=nullptr);

  const CQChartsShapeType &shapeType() const;
  void setShapeType(const CQChartsShapeType &pos);

 private:
  void connectSlots(bool b);

  void updateWidgets();

 Q_SIGNALS:
  void shapeChanged();

 private Q_SLOTS:
  void editSwitched(bool);

  void comboChanged();
  void textChangedSlot();

 private:
  static bool s_isAlt;

  CQChartsShapeType shapeType_;
  QComboBox*        combo_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsShapeType
 * \ingroup Charts
 */
class CQChartsShapeTypePropertyViewType : public CQPropertyViewType {
 public:
  CQChartsShapeTypePropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "shape"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsShapeType
 * \ingroup Charts
 */
class CQChartsShapeTypePropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsShapeTypePropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
