#ifndef CQChartsOptStringEdit_H
#define CQChartsOptStringEdit_H

#include <CQChartsOptEdit.h>
#include <CQChartsOptString.h>

/*!
 * \brief optional int edit
 * \ingroup Charts
 */
class CQChartsOptStringEdit : public CQChartsOptEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptString value READ value WRITE setValue)

 public:
  CQChartsOptStringEdit(QWidget *parent=nullptr);

  const CQChartsOptString &value() const { return value_; }
  void setValue(const CQChartsOptString &pos);

 private:
  void setTextValue(const QString &str) override;
  void resetValue() override;

  bool isValueSet() const override;

  QString valueToText() override;

  QVariant toVariant() const override;
  void fromVariant(const QVariant &var) override;

 private:
  CQChartsOptString value_; //!< optional int
};

//------

/*!
 * \brief type for CQChartsOptString
 * \ingroup Charts
 */
class CQChartsOptStringPropertyViewType : public CQChartsOptPropertyViewType {
 public:
  CQChartsOptStringPropertyViewType() = default;

  CQPropertyViewEditorFactory *getEditor() const override;

  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const override;

  QString userName() const override { return "opt_int"; }
};

//---

/*!
 * \brief editor factory for CQChartsOptString
 * \ingroup Charts
 */
class CQChartsOptStringPropertyViewEditor : public CQChartsOptPropertyViewEditor {
 public:
  CQChartsOptStringPropertyViewEditor() = default;

  QWidget *createEdit(QWidget *parent) override;
};

#endif
