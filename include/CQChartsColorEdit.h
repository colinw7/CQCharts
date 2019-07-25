#ifndef CQChartsColorEdit_H
#define CQChartsColorEdit_H

#include <CQChartsColor.h>
#include <CQChartsLineEditBase.h>

class CQChartsColorEdit;

/*!
 * \brief Color line edit
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

class CQRealSpin;
class CQColorEdit;
class CQCheckBox;

class QComboBox;
class QSpinBox;

/*!
 * \brief Color edit
 */
class CQChartsColorEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsColor color READ color WRITE setColor)

 public:
  CQChartsColorEdit(QWidget *parent=nullptr);

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c);

  void setNoFocus();

 signals:
  void colorChanged();

 private slots:
  void widgetsToColor();

  void updateState();

 private:
  void colorToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsColor color_;                  //!< color
  QComboBox*    typeCombo_  { nullptr }; //!< type combo
  QSpinBox*     indEdit_    { nullptr }; //!< index edit
  CQRealSpin*   valueEdit_  { nullptr }; //!< value edit
  CQColorEdit*  colorEdit_  { nullptr }; //!< color edit
  CQCheckBox*   scaleCheck_ { nullptr }; //!< scale check
  bool          connected_  { false };   //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsColor
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
