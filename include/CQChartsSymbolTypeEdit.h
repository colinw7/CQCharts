#ifndef CQChartsSymbolTypeEdit_H
#define CQChartsSymbolTypeEdit_H

#include <CQChartsSymbolType.h>
#include <QFrame>

class CQRealSpin;
class QComboBox;

/*!
 * \brief symbol edit
 * \ingroup Charts
 */
class CQChartsSymbolTypeEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsSymbolType symbol READ symbolType WRITE setSymbolType)

 public:
  CQChartsSymbolTypeEdit(QWidget *parent=nullptr);

  const CQChartsSymbolType &symbolType() const;
  void setSymbolType(const CQChartsSymbolType &pos);

 private:
  void connectSlots(bool b);

 Q_SIGNALS:
  void symbolChanged();

 private Q_SLOTS:
  void comboChanged();

 private:
  CQChartsSymbolType symbolType_;
  QComboBox*         combo_ { nullptr };
};

//------

#if 0
#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsSymbolType
 * \ingroup Charts
 */
class CQChartsSymbolTypePropertyViewType : public CQPropertyViewType {
 public:
  CQChartsSymbolTypePropertyViewType();

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
 * \brief editor factory for CQChartsSymbolType
 * \ingroup Charts
 */
class CQChartsSymbolTypePropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsSymbolTypePropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};
#endif

#endif
