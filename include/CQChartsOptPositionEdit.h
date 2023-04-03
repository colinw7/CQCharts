#ifndef CQChartsOptPositionEdit_H
#define CQChartsOptPositionEdit_H

#include <CQChartsOptEdit.h>
#include <CQChartsOptPosition.h>

/*!
 * \brief optional position edit
 * \ingroup Charts
 */
class CQChartsOptPositionEdit : public CQChartsOptEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptPosition value READ value WRITE setValue)

 public:
  CQChartsOptPositionEdit(QWidget *parent=nullptr);

  const CQChartsOptPosition &value() const { return value_; }
  void setValue(const CQChartsOptPosition &pos);

 private:
  void setTextValue(const QString &str) override;
  void resetValue() override;

  bool isValueSet() const override;

  QString valueToText() override;

  QVariant toVariant() const override;
  void fromVariant(const QVariant &var) override;

 private:
  CQChartsOptPosition value_; //!< optional position
};

//------

/*!
 * \brief type for CQChartsOptPosition
 * \ingroup Charts
 */
class CQChartsOptPositionPropertyViewType : public CQChartsOptPropertyViewType {
 public:
  CQChartsOptPositionPropertyViewType() = default;

  CQPropertyViewEditorFactory *getEditor() const override;

  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const override;

  QString userName() const override { return "opt_position"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsOptPosition
 * \ingroup Charts
 */
class CQChartsOptPositionPropertyViewEditor : public CQChartsOptPropertyViewEditor {
 public:
  CQChartsOptPositionPropertyViewEditor() = default;

  QWidget *createEdit(QWidget *parent) override;
};

#endif
