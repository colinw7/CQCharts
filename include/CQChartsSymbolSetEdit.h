#ifndef CQChartsSymbolSetEdit_H
#define CQChartsSymbolSetEdit_H

#include <CQChartsSymbolSet.h>
#include <QFrame>

class CQCharts;
class QComboBox;

/*!
 * \brief symbol set name edit
 * \ingroup Charts
 */
class CQChartsSymbolSetEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString symbolSetName READ symbolSetName WRITE setSymbolSetName)

 public:
  CQChartsSymbolSetEdit(QWidget *parent=nullptr);

  //! get/set charts
  const CQCharts *charts() const { return charts_; }
  void setCharts(const CQCharts *charts);

  //! get/set symbol set name
  const QString &symbolSetName() const;
  void setSymbolSetName(const QString &name);

  //! convenience function to set charts and symbol set
  void setChartsSymbolSetName(CQCharts *charts, const QString &name);

 private:
  void connectSlots(bool b);

 signals:
  //! emitted when symbol set changed
  void setChanged(const QString &);

 private slots:
  void comboChanged();

 private:
  const CQCharts* charts_ { nullptr };
  QString         symbolSetName_;
  QComboBox*      combo_ { nullptr };
};

//------

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

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
