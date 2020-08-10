#ifndef CQChartsAngleEdit_H
#define CQChartsAngleEdit_H

#include <CQChartsAngle.h>
#include <CQAngleSpinBox.h>

/*!
 * \brief angle value edit
 * \ingroup Charts
 */
class CQChartsAngleEdit : public CQAngleSpinBox {
  Q_OBJECT

  Q_PROPERTY(CQChartsAngle angle READ angle WRITE setAngle)

 public:
  CQChartsAngleEdit(QWidget *parent=nullptr);

  const CQChartsAngle &angle() const { return angle_; }
  void setAngle(const CQChartsAngle &angle);

 signals:
  void angleChanged();

 private slots:
  void editChanged();

 private:
  void widgetsToAngle();
  void angleToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsAngle angle_;               //!< angle value
  bool          connected_ { false }; //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsAngle
 * \ingroup Charts
 */
class CQChartsAnglePropertyViewType : public CQPropertyViewType {
 public:
  CQChartsAnglePropertyViewType() { }

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "angle"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsAngle
 * \ingroup Charts
 */
class CQChartsAnglePropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsAnglePropertyViewEditor() { }

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
