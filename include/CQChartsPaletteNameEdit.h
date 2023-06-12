#ifndef CQChartsPaletteNameEdit_H
#define CQChartsPaletteNameEdit_H

#include <CQChartsPaletteName.h>
#include <CQChartsEditBase.h>
#include <QFrame>

class CQCharts;
class QComboBox;

/*!
 * \brief palette name edit
 * \ingroup Charts
 */
class CQChartsPaletteNameEdit : public CQChartsFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsPaletteName paletteName READ paletteName WRITE setPaletteName)

 public:
  CQChartsPaletteNameEdit(QWidget *parent=nullptr);

  //! set charts
  void setCharts(const CQCharts *charts) override;

  //! get/set palette name
  const CQChartsPaletteName &paletteName() const;
  void setPaletteName(const CQChartsPaletteName &name);

  //! convenience function to set charts and palette name
  void setChartsPaletteName(CQCharts *charts, const CQChartsPaletteName &name);

 private:
  void connectSlots(bool b);

 Q_SIGNALS:
  //! emitted when palette name changed
  void nameChanged();

 private Q_SLOTS:
  void comboChanged();

 private:
  CQChartsPaletteName name_;
  QComboBox*          combo_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsPaletteName
 * \ingroup Charts
 */
class CQChartsPaletteNamePropertyViewType : public CQPropertyViewType {
 public:
  CQChartsPaletteNamePropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "paletteName"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsPaletteName
 * \ingroup Charts
 */
class CQChartsPaletteNamePropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsPaletteNamePropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
