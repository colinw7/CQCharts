#ifndef CQChartsAxisValueTypeEdit_H
#define CQChartsAxisValueTypeEdit_H

#include <CQChartsAxisValueType.h>
#include <CQChartsEnumEdit.h>

class CQRealSpin;
class QComboBox;

/*!
 * \brief axis value type edit
 * \ingroup Charts
 */
class CQChartsAxisValueTypeEdit : public CQChartsEnumEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsAxisValueType axisValueType READ axisValueType WRITE setAxisValueType)

 public:
  CQChartsAxisValueTypeEdit(QWidget *parent=nullptr);

  const CQChartsAxisValueType &axisValueType() const { return value_; }
  void setAxisValueType(const CQChartsAxisValueType &loc);

  QStringList enumNames() const override { return value_.enumNames(); }

  void setEnumFromString(const QString &str) override;

  QVariant getVariantFromEnum() const override;

  void setEnumFromVariant(const QVariant &var) override;

  QString variantToString(const QVariant &var) const override;

  void connect(QObject *obj, const char *method) override;

 signals:
  void valueChanged();

 private:
  CQChartsAxisValueType value_;
};

//------

/*!
 * \brief type for CQChartsAxisValueType
 * \ingroup Charts
 */
class CQChartsAxisValueTypePropertyViewType : public CQChartsEnumPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  QString variantToString(const QVariant &var) const override;
};

//---

/*!
 * \brief editor factory for CQChartsAxisValueType
 * \ingroup Charts
 */
class CQChartsAxisValueTypePropertyViewEditor :
  public CQChartsEnumPropertyViewEditorFactory {
 public:
  QWidget *createEdit(QWidget *parent) override;
};

#endif
