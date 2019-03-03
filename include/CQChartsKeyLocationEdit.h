#ifndef CQChartsKeyLocationEdit_H
#define CQChartsKeyLocationEdit_H

#include <CQChartsKeyLocation.h>
#include <CQChartsEnumEdit.h>

class CQRealSpin;
class QComboBox;

class CQChartsKeyLocationEdit : public CQChartsEnumEdit {
  Q_OBJECT

 public:
  CQChartsKeyLocationEdit(QWidget *parent=nullptr);

  const CQChartsKeyLocation &keyLocation() const { return keyLocation_; }
  void setKeyLocation(const CQChartsKeyLocation &loc);

  QStringList enumNames() const override { return keyLocation_.enumNames(); }

  void setEnumFromString(const QString &str) override;

  QVariant getVariantFromEnum() const override;

  void setEnumFromVariant(const QVariant &var) override;

  QString variantToString(const QVariant &var) const override;

  void connect(QObject *obj, const char *method) override;

 signals:
  void keyLocationChanged();

 private:
  CQChartsKeyLocation keyLocation_;
};

//------

// type for CQChartsKeyLocation
class CQChartsKeyLocationPropertyViewType : public CQChartsEnumPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  QString variantToString(const QVariant &var) const override;
};

//---

// editor factory for CQChartsKeyLocation
class CQChartsKeyLocationPropertyViewEditor :
  public CQChartsEnumPropertyViewEditorFactory {
 public:
  QWidget *createEdit(QWidget *parent) override;
};

#endif
