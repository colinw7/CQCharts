#ifndef CQChartsFillPatternEdit_H
#define CQChartsFillPatternEdit_H

#include <CQChartsFillPattern.h>
#include <QFrame>

class CQRealSpin;
class QComboBox;

class CQChartsFillPatternEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsFillPatternEdit(QWidget *parent=nullptr);

  const CQChartsFillPattern &fillPattern() const;
  void setFillPattern(const CQChartsFillPattern &pos);

 signals:
  void fillPatternChanged();

 private slots:
  void comboChanged();

 private:
  CQChartsFillPattern fillPattern_;
  QComboBox*          combo_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsFillPattern
class CQChartsFillPatternPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsFillPatternPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsFillPattern
class CQChartsFillPatternPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsFillPatternPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
