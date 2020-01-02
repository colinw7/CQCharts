#ifndef CQChartsOptRealEdit_H
#define CQChartsOptRealEdit_H

#include <CQChartsOptEdit.h>
#include <CQChartsOptReal.h>

/*!
 * \brief optional real edit
 * \ingroup Charts
 */
class CQChartsOptRealEdit : public CQChartsOptEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptReal value READ value WRITE setValue)

 public:
  CQChartsOptRealEdit(QWidget *parent=nullptr);

  const CQChartsOptReal &value() const { return value_; }
  void setValue(const CQChartsOptReal &pos);

 private:
  void setTextValue(const QString &str) override;
  void resetValue() override;

  bool isValueSet() const override;

  QString valueToText() override;

  QVariant toVariant() const override;
  void fromVariant(const QVariant &var) override;

 private:
  CQChartsOptReal value_; //!< optional real
};

//------

/*!
 * \brief type for CQChartsOptReal
 * \ingroup Charts
 */
class CQChartsOptRealPropertyViewType : public CQChartsOptPropertyViewType {
 public:
  CQChartsOptRealPropertyViewType() { }

  CQPropertyViewEditorFactory *getEditor() const override;

  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const override;

  QString userName() const override { return "opt_real"; }
};

//---

/*!
 * \brief editor factory for CQChartsOptReal
 * \ingroup Charts
 */
class CQChartsOptRealPropertyViewEditor : public CQChartsOptPropertyViewEditor {
 public:
  CQChartsOptRealPropertyViewEditor() { }

  QWidget *createEdit(QWidget *parent) override;
};

#endif
