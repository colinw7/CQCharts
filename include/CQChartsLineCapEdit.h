#ifndef CQChartsLineCapEdit_H
#define CQChartsLineCapEdit_H

#include <CQChartsLineCap.h>
#include <CQChartsEnumEdit.h>

class CQRealSpin;

/*!
 * \brief line cap edit
 * \ingroup Charts
 */
class CQChartsLineCapEdit : public CQChartsEnumEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsLineCap lineCap READ lineCap WRITE setLineCap)

 public:
  CQChartsLineCapEdit(QWidget *parent=nullptr);

  const CQChartsLineCap &lineCap() const { return lineCap_; }
  void setLineCap(const CQChartsLineCap &loc);

  QStringList enumNames() const override { return lineCap_.enumNames(); }

  void setEnumFromString(const QString &str) override;

  QVariant getVariantFromEnum() const override;

  void setEnumFromVariant(const QVariant &var) override;

  QString variantToString(const QVariant &var) const override;

  void connect(QObject *obj, const char *method) override;

 Q_SIGNALS:
  void lineCapChanged();

 private:
  CQChartsLineCap lineCap_;
};

//------

/*!
 * \brief type for CQChartsLineCap
 * \ingroup Charts
 */
class CQChartsLineCapPropertyViewType : public CQChartsEnumPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  QString variantToString(const QVariant &var) const override;
};

//---

/*!
 * \brief editor factory for CQChartsLineCap
 * \ingroup Charts
 */
class CQChartsLineCapPropertyViewEditor :
  public CQChartsEnumPropertyViewEditorFactory {
 public:
  QWidget *createEdit(QWidget *parent) override;
};

#endif
