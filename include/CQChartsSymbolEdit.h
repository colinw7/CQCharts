#ifndef CQChartsSymbolEdit_H
#define CQChartsSymbolEdit_H

#include <CQChartsSymbol.h>
#include <QFrame>

class CQRealSpin;
class QComboBox;

class CQChartsSymbolEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsSymbolEdit(QWidget *parent=nullptr);

  const CQChartsSymbol &symbol() const;
  void setSymbol(const CQChartsSymbol &pos);

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

// type for CQChartsSymbol
class CQChartsSymbolPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsSymbolPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsSymbol
class CQChartsSymbolPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsSymbolPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
