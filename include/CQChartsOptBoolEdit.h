#ifndef CQChartsOptBoolEdit_H
#define CQChartsOptBoolEdit_H

#include <CQChartsOptEdit.h>
#include <CQChartsOptBool.h>

/*!
 * \brief optional boolean edit
 * \ingroup Charts
 */
class CQChartsOptBoolEdit : public CQChartsOptEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptBool value READ value WRITE setValue)

 public:
  CQChartsOptBoolEdit(QWidget *parent=nullptr);

  const CQChartsOptBool &value() const { return value_; }
  void setValue(const CQChartsOptBool &pos);

 private:
  void setTextValue(const QString &str) override;
  void resetValue() override;

  bool isValueSet() const override;

  QString valueToText() override;

  QVariant toVariant() const override;
  void fromVariant(const QVariant &var) override;

 private:
  CQChartsOptBool value_; //!< optional boolean
};

//------

/*!
 * \brief type for CQChartsOptBool
 * \ingroup Charts
 */
class CQChartsOptBoolPropertyViewType : public CQChartsOptPropertyViewType {
 public:
  CQChartsOptBoolPropertyViewType() { }

  CQPropertyViewEditorFactory *getEditor() const override;

  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const override;

  QString userName() const override { return "opt_bool"; }
};

//---

/*!
 * \brief editor factory for CQChartsOptBool
 * \ingroup Charts
 */
class CQChartsOptBoolPropertyViewEditor : public CQChartsOptPropertyViewEditor {
 public:
  CQChartsOptBoolPropertyViewEditor() { }

  QWidget *createEdit(QWidget *parent) override;
};

#endif
