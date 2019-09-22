#ifndef CQChartsTitleLocationEdit_H
#define CQChartsTitleLocationEdit_H

#include <CQChartsTitleLocation.h>
#include <CQChartsEnumEdit.h>

/*!
 * \brief editor for CQChartsTitleLocation
 * \ingroup Charts
 */
class CQChartsTitleLocationEdit : public CQChartsEnumEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsTitleLocation titleLocation READ titleLocation WRITE setTitleLocation)

 public:
  CQChartsTitleLocationEdit(QWidget *parent=nullptr);

  const CQChartsTitleLocation &titleLocation() const { return titleLocation_; }
  void setTitleLocation(const CQChartsTitleLocation &loc);

  QStringList enumNames() const override { return titleLocation_.enumNames(); }

  void setEnumFromString(const QString &str) override;

  QVariant getVariantFromEnum() const override;

  void setEnumFromVariant(const QVariant &var) override;

  QString variantToString(const QVariant &var) const override;

  void connect(QObject *obj, const char *method) override;

 signals:
  void titleLocationChanged();

 private:
  CQChartsTitleLocation titleLocation_;
};

//------

/*!
 * \brief type for CQChartsTitleLocation
 * \ingroup Charts
 */
class CQChartsTitleLocationPropertyViewType : public CQChartsEnumPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  QString variantToString(const QVariant &var) const override;
};

//---

/*!
 * \brief editor factory for CQChartsTitleLocation
 * \ingroup Charts
 */
class CQChartsTitleLocationPropertyViewEditorFactory :
  public CQChartsEnumPropertyViewEditorFactory {
 public:
  QWidget *createEdit(QWidget *parent) override;
};

#endif
