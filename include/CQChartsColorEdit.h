#ifndef CQChartsColorEdit_H
#define CQChartsColorEdit_H

#include <CQChartsColor.h>
#include <CQChartsLineEditBase.h>

class CQChartsColorEdit;

/*!
 * \brief Color line edit
 * \ingroup Charts
 */
class CQChartsColorLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsColor color READ color WRITE setColor)

 public:
  CQChartsColorLineEdit(QWidget *parent=nullptr);

  const CQChartsColor &color() const;
  void setColor(const CQChartsColor &c);

  void setNoFocus();

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void colorChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateColor(const CQChartsColor &c, bool updateText);

  void colorToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsColorEdit* dataEdit_ { nullptr }; //!< color data edit
};

//---

#include <CQChartsEditBase.h>

class CQColorsEditModel;
class CQRealSpin;
class CQColorEdit;
class CQCheckBox;

class QComboBox;
class QSpinBox;
class QLabel;
class QCheckBox;
class QStackedWidget;

/*!
 * \brief Color edit
 * \ingroup Charts
 */
class CQChartsColorEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsColor color READ color WRITE setColor)

 public:
  CQChartsColorEdit(QWidget *parent=nullptr);

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c);

  void setNoFocus();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 signals:
  void colorChanged();

 private slots:
  void widgetsToColor();

  void indPalSlot(int ind);
  void updateState();

 private:
  void colorToWidgets();

  void connectSlots(bool b);

 private:
  using WidgetLabels = std::map<QWidget*,QWidget*>;

  CQChartsColor      color_;                     //!< color
  QComboBox*         typeCombo_     { nullptr }; //!< palette index/type combo
  QComboBox*         indPalCombo_   { nullptr }; //!< type combo
  QStackedWidget*    indPalStack_   { nullptr }; //!< palette index/type edit stack
  QSpinBox*          indEdit_       { nullptr }; //!< index edit
  QComboBox*         paletteEdit_   { nullptr }; //!< palette edit
  QFrame*            rFrame_        { nullptr }; //!< r edit frame
  CQColorsEditModel* rEdit_         { nullptr }; //!< r edit
  QCheckBox*         rNeg_          { nullptr }; //!< r negate
  QFrame*            gFrame_        { nullptr }; //!< g edit frame
  CQColorsEditModel* gEdit_         { nullptr }; //!< g edit
  QCheckBox*         gNeg_          { nullptr }; //!< g negate
  QFrame*            bFrame_        { nullptr }; //!< b edit frame
  CQColorsEditModel* bEdit_         { nullptr }; //!< b edit
  QCheckBox*         bNeg_          { nullptr }; //!< b negate
  CQRealSpin*        valueEdit_     { nullptr }; //!< value edit
  CQColorEdit*       colorEdit_     { nullptr }; //!< color edit
  CQCheckBox*        scaleCheck_    { nullptr }; //!< scale check
  WidgetLabels       widgetLabels_;              //!< widget labels
  bool               connected_     { false };   //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsColor
 * \ingroup Charts
 */
class CQChartsColorPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsColorPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "color"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsColor
 * \ingroup Charts
 */
class CQChartsColorPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsColorPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
