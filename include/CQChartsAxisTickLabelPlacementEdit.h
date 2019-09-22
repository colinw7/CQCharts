#ifndef CQChartsAxisTickLabelPlacementEdit_H
#define CQChartsAxisTickLabelPlacementEdit_H

#include <CQChartsAxisTickLabelPlacement.h>
#include <CQChartsEnumEdit.h>

/*!
 * \brief editor for CQChartsAxisTickLabelPlacement
 * \ingroup Charts
 */
class CQChartsAxisTickLabelPlacementEdit : public CQChartsEnumEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsAxisTickLabelPlacement axisTickLabelPlacement
               READ axisTickLabelPlacement WRITE setAxisTickLabelPlacement)

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

/*!
 * \brief type for CQChartsAxisTickLabelPlacement
 * \ingroup Charts
 */
class CQChartsAxisTickLabelPlacementPropertyViewType : public CQChartsEnumPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  QString variantToString(const QVariant &var) const override;
};

//---

/*!
 * \brief editor factory for CQChartsAxisTickLabelPlacement
 * \ingroup Charts
 */
class CQChartsAxisTickLabelPlacementPropertyViewEditorFactory :
  public CQChartsEnumPropertyViewEditorFactory {
 public:
  QWidget *createEdit(QWidget *parent) override;
};

#endif
