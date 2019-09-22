#ifndef CQChartsSymbolEdit_H
#define CQChartsSymbolEdit_H

#include <CQChartsSymbol.h>
#include <QFrame>

class CQRealSpin;
class QComboBox;

/*!
 * \brief symbol edit
 * \ingroup Charts
 */
class CQChartsSymbolEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsSymbol symbol READ symbol WRITE setSymbol)

 public:
  CQChartsSymbolEdit(QWidget *parent=nullptr);

  const CQChartsSymbol &symbol() const;
  void setSymbol(const CQChartsSymbol &pos);

 private:
  void connectSlots(bool b);

 signals:
  void symbolChanged();

 private slots:
  void comboChanged();

 private:
  CQChartsSymbol symbol_;
  QComboBox*     combo_ { nullptr };
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
            const QVariant &value, bool inside) override;

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
