#ifndef CQChartsUnitsEdit_H
#define CQChartsUnitsEdit_H

#include <CQChartsUnits.h>
#include <QComboBox>

/*!
 * \brief units edit
 * \ingroup Charts
 */
class CQChartsUnitsEdit : public QComboBox {
  Q_OBJECT

 public:
  using Units = CQChartsUnits::Type;

 public:
  CQChartsUnitsEdit(QWidget *parent=nullptr);

  const Units &units() const;
  void setUnits(const Units &units);

  QSize sizeHint() const override;

 Q_SIGNALS:
  void unitsChanged();

 private:
  void updateTip();

 private Q_SLOTS:
  void indexChanged();

 private:
  Units units_ { Units::NONE };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsUnits
 * \ingroup Charts
 */
class CQChartsUnitsPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsUnitsPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "length"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsUnits
 * \ingroup Charts
 */
class CQChartsUnitsPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsUnitsPropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
