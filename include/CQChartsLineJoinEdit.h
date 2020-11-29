#ifndef CQChartsLineJoinEdit_H
#define CQChartsLineJoinEdit_H

#include <CQChartsLineJoin.h>
#include <CQChartsEnumEdit.h>

class CQRealSpin;

/*!
 * \brief line join edit
 * \ingroup Charts
 */
class CQChartsLineJoinEdit : public CQChartsEnumEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsLineJoin lineJoin READ lineJoin WRITE setLineJoin)

 public:
  CQChartsLineJoinEdit(QWidget *parent=nullptr);

  const CQChartsLineJoin &lineJoin() const { return lineJoin_; }
  void setLineJoin(const CQChartsLineJoin &loc);

  QStringList enumNames() const override { return lineJoin_.enumNames(); }

  void setEnumFromString(const QString &str) override;

  QVariant getVariantFromEnum() const override;

  void setEnumFromVariant(const QVariant &var) override;

  QString variantToString(const QVariant &var) const override;

  void connect(QObject *obj, const char *method) override;

 signals:
  void lineJoinChanged();

 private:
  CQChartsLineJoin lineJoin_;
};

//------

/*!
 * \brief type for CQChartsLineJoin
 * \ingroup Charts
 */
class CQChartsLineJoinPropertyViewType : public CQChartsEnumPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  QString variantToString(const QVariant &var) const override;
};

//---

/*!
 * \brief editor factory for CQChartsLineJoin
 * \ingroup Charts
 */
class CQChartsLineJoinPropertyViewEditor :
  public CQChartsEnumPropertyViewEditorFactory {
 public:
  QWidget *createEdit(QWidget *parent) override;
};

#endif
