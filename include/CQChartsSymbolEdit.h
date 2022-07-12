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

  Q_PROPERTY(CQChartsSymbol symbol READ symbol WRITE setSymbol)

 public:
  CQChartsSymbolLineEdit(QWidget *parent=nullptr);

  const CQChartsSymbol &symbol() const;
  void setSymbol(const CQChartsSymbol &c);

  void setNoFocus();

  void drawPreview(QPainter *painter, const QRect &rect) override;

 Q_SIGNALS:
  void symbolChanged();

 private Q_SLOTS:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateSymbol(const CQChartsSymbol &c, bool updateText);

  void symbolToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsSymbolEdit* dataEdit_ { nullptr }; //!< symbol data edit
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

  Q_PROPERTY(CQChartsSymbol symbol READ symbol WRITE setSymbol)

 public:
  CQChartsSymbolEdit(QWidget *parent=nullptr);

  const CQChartsSymbol &symbol() const { return symbol_; }
  void setSymbol(const CQChartsSymbol &c);

  void setNoFocus();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 Q_SIGNALS:
  void symbolChanged();

 private Q_SLOTS:
  void widgetsToSymbol();

  void updateState();

 private:
  void symbolToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsSymbol          symbol_;                   //!< symbol
  QComboBox*              typeCombo_    { nullptr }; //!< type combo
  QStackedWidget*         stack_        { nullptr }; //!< per type widget stack
  CQChartsSymbolTypeEdit* typeEdit_     { nullptr }; //!< type edit
  QLineEdit*              charEdit_     { nullptr }; //!< char edit
  QLineEdit*              charNameEdit_ { nullptr }; //!< char name edit
  QComboBox*              pathCombo_    { nullptr }; //!< path name combo
  QLineEdit*              pathSrcEdit_  { nullptr }; //!< path src edit
  QComboBox*              svgCombo_     { nullptr }; //!< svg name combo
  QLineEdit*              svgSrcEdit_   { nullptr }; //!< svg src edit
  CQCheckBox*             filledCheck_  { nullptr }; //!< filled check
  CQCheckBox*             strokedCheck_ { nullptr }; //!< stroke check
  bool                    connected_    { false };   //!< is connected
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
