#ifndef CQChartsKeyPressBehaviorEdit_H
#define CQChartsKeyPressBehaviorEdit_H

#include <CQChartsKeyPressBehavior.h>
#include <CQChartsEnumEdit.h>

/*!
 * \brief editor for CQChartsKeyPressBehavior
 * \ingroup Charts
 */
class CQChartsKeyPressBehaviorEdit : public CQChartsEnumEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsKeyPressBehavior keyPressBehavior
               READ keyPressBehavior WRITE setKeyPressBehavior)

 public:
  CQChartsKeyPressBehaviorEdit(QWidget *parent=nullptr);

  const CQChartsKeyPressBehavior &keyPressBehavior() const { return keyPressBehavior_; }
  void setKeyPressBehavior(const CQChartsKeyPressBehavior &bheavior);

  QStringList enumNames() const override { return keyPressBehavior_.enumNames(); }

  void setEnumFromString(const QString &str) override;

  QVariant getVariantFromEnum() const override;

  void setEnumFromVariant(const QVariant &var) override;

  QString variantToString(const QVariant &var) const override;

  void connect(QObject *obj, const char *method) override;

 signals:
  void keyPressBehaviorChanged();

 private:
  CQChartsKeyPressBehavior keyPressBehavior_;
};

//------

/*!
 * \brief type for CQChartsKeyPressBehavior
 * \ingroup Charts
 */
class CQChartsKeyPressBehaviorPropertyViewType : public CQChartsEnumPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  QString variantToString(const QVariant &var) const override;
};

//---

/*!
 * \brief editor factory for CQChartsKeyPressBehavior
 * \ingroup Charts
 */
class CQChartsKeyPressBehaviorPropertyViewEditorFactory :
  public CQChartsEnumPropertyViewEditorFactory {
 public:
  QWidget *createEdit(QWidget *parent) override;
};

#endif
