#ifndef CQChartsColumnNumEdit_H
#define CQChartsColumnNumEdit_H

#include <CQChartsColumnNum.h>
#include <CQChartsLineEditBase.h>
#include <QFrame>

class CQChartsColumnCombo;
class CQChartsModelData;

/*!
 * \brief Column Number Line Edit
 * \ingroup Charts
 */
class CQChartsColumnNumLineEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumnNum columnNum READ columnNum WRITE setColumnNum)
  Q_PROPERTY(bool              proxy     READ isProxy   WRITE setProxy    )

 public:
  CQChartsColumnNumLineEdit(QWidget *parent=nullptr);

  void setPlot(CQChartsPlot *plot);

  void setModelData(CQChartsModelData *modelData);

  const CQChartsColumnNum &columnNum() const;
  void setColumnNum(const CQChartsColumnNum &c);

  bool isProxy() const { return proxy_; }
  void setProxy(bool b);

 Q_SIGNALS:
  void columnChanged();

 private Q_SLOTS:
  void resetModelData();

  void columnComboChanged();

 private:
  void columnToWidgets();
  void widgetsToColumn();

  void connectSlots(bool b);

 private:
  CQChartsColumnNum    columnNum_;             //!< column number
  CQChartsModelData*   modelData_ { nullptr }; //!< model data
  CQChartsColumnCombo* combo_     { nullptr }; //!< column combo
  bool                 connected_ { false };   //!< is connected
  bool                 proxy_     { true };    //!< model allows proxy
};

//---

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsColumnNum
 * \ingroup Charts
 */
class CQChartsColumnNumPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsColumnNumPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "column"; }

 private:
  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsColumnNum
 * \ingroup Charts
 */
class CQChartsColumnNumPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsColumnNumPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
