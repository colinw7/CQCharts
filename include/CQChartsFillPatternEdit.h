#ifndef CQChartsFillPatternEdit_H
#define CQChartsFillPatternEdit_H

#include <CQChartsFillPattern.h>
#include <CQChartsEnumEdit.h>

/*!
 * \brief editor for CQChartsFillPattern
 * \ingroup Charts
 */
class CQChartsFillPatternEdit : public CQChartsEnumEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillPattern fillPattern READ fillPattern WRITE setFillPattern)

 public:
  CQChartsFillPatternEdit(QWidget *parent=nullptr);

  const CQChartsFillPattern &fillPattern() const { return fillPattern_; }
  void setFillPattern(const CQChartsFillPattern &pos);

  QStringList enumNames() const override { return fillPattern_.enumNames(); }

  void setEnumFromString(const QString &str) override;

  QVariant getVariantFromEnum() const override;

  void setEnumFromVariant(const QVariant &var) override;

  QString variantToString(const QVariant &var) const override;

  void connect(QObject *obj, const char *method) override;

 signals:
  void fillPatternChanged();

 private:
  CQChartsFillPattern fillPattern_;
  QComboBox*          combo_ { nullptr };
};

//------

/*!
 * \brief type for CQChartsFillPattern
 * \ingroup Charts
 */
class CQChartsFillPatternPropertyViewType : public CQChartsEnumPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  QString variantToString(const QVariant &var) const override;
};

//---

/*!
 * \brief editor factory for CQChartsFillPattern
 * \ingroup Charts
 */
class CQChartsFillPatternPropertyViewEditorFactory :
  public CQChartsEnumPropertyViewEditorFactory {
 public:
  QWidget *createEdit(QWidget *parent) override;
};

#endif
