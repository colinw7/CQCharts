#ifndef CQChartsPaletteNameEdit_H
#define CQChartsPaletteNameEdit_H

#include <CQChartsPaletteName.h>
#include <QFrame>

class CQCharts;
class QComboBox;

/*!
 * \brief palette name edit
 * \ingroup Charts
 */
class CQChartsPaletteNameEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsPaletteName paletteName READ paletteName WRITE setPaletteName)

 public:
  CQChartsPaletteNameEdit(QWidget *parent=nullptr);

  const CQChartsPaletteName &paletteName() const;
  void setPaletteName(const CQChartsPaletteName &name);

  void setCharts(const CQCharts *charts);

 private:
  void connectSlots(bool b);

 signals:
  void nameChanged();

 private slots:
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

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
