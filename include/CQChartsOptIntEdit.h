#ifndef CQChartsOptIntEdit_H
#define CQChartsOptIntEdit_H

#include <CQChartsOptEdit.h>
#include <CQChartsOptInt.h>

/*!
 * \brief optional int edit
 * \ingroup Charts
 */
class CQChartsOptIntEdit : public CQChartsOptEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptInt value READ value WRITE setValue)

 public:
  CQChartsOptIntEdit(QWidget *parent=nullptr);

  const CQChartsOptInt &value() const { return value_; }
  void setValue(const CQChartsOptInt &pos);

 private:
  void setTextValue(const QString &str) override;
  void resetValue() override;

  bool isValueSet() const override;

  QString valueToText() override;

  QVariant toVariant() const override;
  void fromVariant(const QVariant &var) override;

 private:
  CQChartsOptInt value_; //!< optional int
};

//------

/*!
 * \brief type for CQChartsOptInt
 * \ingroup Charts
 */
class CQChartsOptIntPropertyViewType : public CQChartsOptPropertyViewType {
 public:
  CQChartsOptIntPropertyViewType() = default;

  CQPropertyViewEditorFactory *getEditor() const override;

  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const override;

  QString userName() const override { return "opt_int"; }
};

//---

/*!
 * \brief editor factory for CQChartsOptInt
 * \ingroup Charts
 */
class CQChartsOptIntPropertyViewEditor : public CQChartsOptPropertyViewEditor {
 public:
  CQChartsOptIntPropertyViewEditor() = default;

  QWidget *createEdit(QWidget *parent) override;
};

#endif
