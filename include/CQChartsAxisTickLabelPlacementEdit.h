#ifndef CQChartsAxisTickLabelPlacementEdit_H
#define CQChartsAxisTickLabelPlacementEdit_H

#include <CQChartsAxisTickLabelPlacement.h>
#include <CQChartsEnumEdit.h>

// editor for CQChartsAxisTickLabelPlacement
class CQChartsAxisTickLabelPlacementEdit : public CQChartsEnumEdit {
  Q_OBJECT

 public:
  CQChartsAxisTickLabelPlacementEdit(QWidget *parent=nullptr);

  const CQChartsAxisTickLabelPlacement &axisTickLabelPlacement() const {
    return axisTickLabelPlacement_; }
  void setAxisTickLabelPlacement(const CQChartsAxisTickLabelPlacement &pos);

  QStringList enumNames() const override { return axisTickLabelPlacement_.enumNames(); }

  void setEnumFromString(const QString &str) override;

  QVariant getVariantFromEnum() const override;

  void setEnumFromVariant(const QVariant &var) override;

  QString variantToString(const QVariant &var) const override;

  void connect(QObject *obj, const char *method) override;

 signals:
  void axisTickLabelPlacementChanged();

 private:
  CQChartsAxisTickLabelPlacement axisTickLabelPlacement_;
  QComboBox*                     combo_ { nullptr };
};

//------

// type for CQChartsAxisTickLabelPlacement
class CQChartsAxisTickLabelPlacementPropertyViewType : public CQChartsEnumPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  QString variantToString(const QVariant &var) const override;
};

//---

// editor factory for CQChartsAxisTickLabelPlacement
class CQChartsAxisTickLabelPlacementPropertyViewEditorFactory :
  public CQChartsEnumPropertyViewEditorFactory {
 public:
  QWidget *createEdit(QWidget *parent) override;
};

#endif
