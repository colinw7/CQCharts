#ifndef CQChartsOptRectEdit_H
#define CQChartsOptRectEdit_H

#include <CQChartsOptEdit.h>
#include <CQChartsOptRect.h>

/*!
 * \brief optional rect edit
 * \ingroup Charts
 */
class CQChartsOptRectEdit : public CQChartsOptEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptRect value READ value WRITE setValue)

 public:
  CQChartsOptRectEdit(QWidget *parent=nullptr);

  const CQChartsOptRect &value() const { return value_; }
  void setValue(const CQChartsOptRect &pos);

 private:
  void setTextValue(const QString &str) override;
  void resetValue() override;

  bool isValueSet() const override;

  QString valueToText() override;

  QVariant toVariant() const override;
  void fromVariant(const QVariant &var) override;

 private:
  CQChartsOptRect value_; //!< optional rect
};

//------

/*!
 * \brief type for CQChartsOptRect
 * \ingroup Charts
 */
class CQChartsOptRectPropertyViewType : public CQChartsOptPropertyViewType {
 public:
  CQChartsOptRectPropertyViewType() { }

  CQPropertyViewEditorFactory *getEditor() const override;

  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const override;

  QString userName() const override { return "opt_rect"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsOptRect
 * \ingroup Charts
 */
class CQChartsOptRectPropertyViewEditor : public CQChartsOptPropertyViewEditor {
 public:
  CQChartsOptRectPropertyViewEditor() { }

  QWidget *createEdit(QWidget *parent) override;
};

#endif
