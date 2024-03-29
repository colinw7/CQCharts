#ifndef CQChartsAlphaEdit_H
#define CQChartsAlphaEdit_H

#include <CQChartsAlpha.h>
#include <CQComboSlider.h>

/*!
 * \brief alpha value edit
 * \ingroup Charts
 */
class CQChartsAlphaEdit : public CQComboSlider {
  Q_OBJECT

  Q_PROPERTY(CQChartsAlpha alpha READ alpha WRITE setAlpha)

 public:
  using Alpha = CQChartsAlpha;

 public:
  CQChartsAlphaEdit(QWidget *parent=nullptr);

  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &alpha);

 Q_SIGNALS:
  void alphaChanged();

 private Q_SLOTS:
  void editChanged();

 private:
  void widgetsToAlpha();
  void alphaToWidgets();

  void connectSlots(bool b);

 private:
  Alpha alpha_;               //!< alpha value
  bool  connected_ { false }; //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsAlpha
 * \ingroup Charts
 */
class CQChartsAlphaPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsAlphaPropertyViewType() = default;

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "alpha"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsAlpha
 * \ingroup Charts
 */
class CQChartsAlphaPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsAlphaPropertyViewEditor() = default;

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
