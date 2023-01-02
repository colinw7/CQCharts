#ifndef CQChartsOptLengthEdit_H
#define CQChartsOptLengthEdit_H

#include <CQChartsOptEdit.h>
#include <CQChartsOptLength.h>

/*!
 * \brief optional length edit
 * \ingroup Charts
 */
class CQChartsOptLengthEdit : public CQChartsOptEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptLength value READ value WRITE setValue)

 public:
  CQChartsOptLengthEdit(QWidget *parent=nullptr);

  const CQChartsOptLength &value() const { return value_; }
  void setValue(const CQChartsOptLength &pos);

 private:
  void setTextValue(const QString &str) override;
  void resetValue() override;

  bool isValueSet() const override;

  QString valueToText() override;

  QVariant toVariant() const override;
  void fromVariant(const QVariant &var) override;

 private:
  CQChartsOptLength value_; //!< optional length
};

//------

/*!
 * \brief type for CQChartsOptLength
 * \ingroup Charts
 */
class CQChartsOptLengthPropertyViewType : public CQChartsOptPropertyViewType {
 public:
  CQChartsOptLengthPropertyViewType() { }

  CQPropertyViewEditorFactory *getEditor() const override;

  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const override;

  QString userName() const override { return "opt_length"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsOptLength
 * \ingroup Charts
 */
class CQChartsOptLengthPropertyViewEditor : public CQChartsOptPropertyViewEditor {
 public:
  CQChartsOptLengthPropertyViewEditor() { }

  QWidget *createEdit(QWidget *parent) override;
};

#endif
