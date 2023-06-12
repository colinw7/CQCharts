#ifndef CQChartsSymbolSetEdit_H
#define CQChartsSymbolSetEdit_H

#include <CQChartsSymbolSet.h>
#include <CQChartsEditBase.h>
#include <QFrame>

class CQCharts;
class QComboBox;

/*!
 * \brief symbol set name edit
 * \ingroup Charts
 */
class CQChartsSymbolSetEdit : public CQChartsFrame {
  Q_OBJECT

  Q_PROPERTY(QString symbolSetName READ symbolSetName WRITE setSymbolSetName)

 public:
  CQChartsSymbolSetEdit(QWidget *parent=nullptr);

  //! get/set charts
  void setCharts(const CQCharts *charts) override;

  //! get/set symbol set name
  const QString &symbolSetName() const;
  void setSymbolSetName(const QString &name);

  //! convenience function to set charts and symbol set
  void setChartsSymbolSetName(CQCharts *charts, const QString &name);

 private:
  void connectSlots(bool b);

 Q_SIGNALS:
  //! emitted when symbol set changed
  void setChanged(const QString &);

 private Q_SLOTS:
  void comboChanged();

 private:
  QString    symbolSetName_;
  QComboBox* combo_ { nullptr };
};

//------

#if 0
#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsSymbolSet
 * \ingroup Charts
 */
class CQChartsSymbolSetPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsSymbolSetPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "symbolSet"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsSymbolSet
 * \ingroup Charts
 */
class CQChartsSymbolSetPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsSymbolSetPropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};
#endif

#endif
