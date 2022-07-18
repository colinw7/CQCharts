#ifndef CQChartsSymbolEdit_H
#define CQChartsSymbolEdit_H

#include <CQChartsSymbol.h>
#include <CQChartsLineEditBase.h>

class CQChartsSymbolEdit;

/*!
 * \brief Symbol line edit
 * \ingroup Charts
 */
class CQChartsSymbolLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsSymbol symbol READ symbol  WRITE setSymbol)
  Q_PROPERTY(bool           basic  READ isBasic WRITE setBasic )

 public:
  CQChartsSymbolLineEdit(QWidget *parent=nullptr);

  //! get/set symbol
  const CQChartsSymbol &symbol() const;
  void setSymbol(const CQChartsSymbol &c);

  //! get/set basic edit state
  bool isBasic() const { return basic_; }
  void setBasic(bool b);

  void setNoFocus();

  void drawPreview(QPainter *painter, const QRect &rect) override;

 Q_SIGNALS:
  void symbolChanged();

 private Q_SLOTS:
  void menuEditChanged();

 private:
  void updateMenu() override;

  void textChanged() override;

  void updateSymbol(const CQChartsSymbol &c, bool updateText);

  void symbolToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsSymbolEdit* dataEdit_ { nullptr }; //!< symbol data edit
  bool                basic_    { false };   //!< is basic
};

//---

#include <CQChartsEditBase.h>

class CQChartsSymbolTypeEdit;
class CQCheckBox;

class QStackedWidget;
class QComboBox;
class QLineEdit;
class QLabel;

/*!
 * \brief Symbol edit
 * \ingroup Charts
 */
class CQChartsSymbolEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsSymbol symbol READ symbol  WRITE setSymbol)
  Q_PROPERTY(bool           basic  READ isBasic WRITE setBasic )

 public:
  CQChartsSymbolEdit(QWidget *parent);

  //! get/set symbol
  const CQChartsSymbol &symbol() const { return symbol_; }
  void setSymbol(const CQChartsSymbol &c);

  //! get/set basic edit state
  bool isBasic() const { return basic_; }
  void setBasic(bool b);

  void updateMenu();

  void setNoFocus();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 Q_SIGNALS:
  void symbolChanged();

 private Q_SLOTS:
  void typeSlot();

  void widgetsToSymbol();

  void updateState();

 private:
  void symbolToWidgets();

  void updateWidgets();

  void connectSlots(bool b);

 private:
  using StackFrames = std::vector<QFrame *>;

  CQChartsSymbolLineEdit *lineEdit_ { nullptr };

  CQChartsSymbol symbol_;          //!< symbol
  bool           basic_ { false }; //!< is basic

  QFrame*                 basicFrame_        { nullptr }; //!< basic frame
  CQChartsSymbolTypeEdit* basicTypeEdit_     { nullptr }; //!< basic type edit
  CQCheckBox*             basicFilledCheck_  { nullptr }; //!< basic filled check
  CQCheckBox*             basicStrokedCheck_ { nullptr }; //!< basic stroke check

  QFrame*                 normalFrame_   { nullptr }; //!< normal frame
  QComboBox*              typeCombo_     { nullptr }; //!< type combo
  StackFrames             stackFrames_;               //!< per type widget stack
  CQChartsSymbolTypeEdit* typeEdit_      { nullptr }; //!< type edit
  QLineEdit*              charEdit_      { nullptr }; //!< char edit
  QLineEdit*              charNameEdit_  { nullptr }; //!< char name edit
  QComboBox*              pathCombo_     { nullptr }; //!< path name combo
  QLineEdit*              pathSrcEdit_   { nullptr }; //!< path src edit
  QComboBox*              svgCombo_      { nullptr }; //!< svg name combo
  QLineEdit*              svgSrcEdit_    { nullptr }; //!< svg src edit
  CQCheckBox*             filledCheck_   { nullptr }; //!< filled check
  CQCheckBox*             strokedCheck_  { nullptr }; //!< stroke check
  bool                    connected_     { false };   //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsSymbol
 * \ingroup Charts
 */
class CQChartsSymbolPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsSymbolPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "symbol"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsSymbol
 * \ingroup Charts
 */
class CQChartsSymbolPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsSymbolPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
