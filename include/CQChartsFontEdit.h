#ifndef CQChartsFontEdit_H
#define CQChartsFontEdit_H

#include <CQChartsFont.h>
#include <CQChartsLineEditBase.h>

class CQChartsFontEdit;

/*!
 * \brief Font line edit
 * \ingroup Charts
 */
class CQChartsFontLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsFont font READ font WRITE setFont)

 public:
  CQChartsFontLineEdit(QWidget *parent=nullptr);

  const CQChartsFont &font() const;
  void setFont(const CQChartsFont &c);

  void setNoFocus();

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void fontChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateFont(const CQChartsFont &c, bool updateText);

  void fontToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsFontEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQFontEdit;
class CQCheckBox;
class CQRealSpin;
class QComboBox;

/*!
 * \brief Font edit
 * \ingroup Charts
 */
class CQChartsFontEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsFont font READ font WRITE setFont)

 public:
  CQChartsFontEdit(QWidget *parent=nullptr);

  const CQChartsFont &font() const;
  void setFont(const CQChartsFont &c);

  void setNoFocus();

 signals:
  void fontChanged();

 private slots:
  void widgetsToFont();

  void updateState();

 private:
  void fontToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsFont font_;
  QComboBox*   typeCombo_     { nullptr };
  CQFontEdit*  fontEdit_      { nullptr };
  CQCheckBox*  normalCheck_   { nullptr };
  CQCheckBox*  boldCheck_     { nullptr };
  CQCheckBox*  italicCheck_   { nullptr };
  QComboBox*   sizeTypeCombo_ { nullptr };
  CQRealSpin*  sizeEdit_      { nullptr };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsFont
 * \ingroup Charts
 */
class CQChartsFontPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsFontPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "font"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsFont
 * \ingroup Charts
 */
class CQChartsFontPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsFontPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
