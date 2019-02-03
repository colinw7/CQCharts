#ifndef CQChartsColumnsEdit_H
#define CQChartsColumnsEdit_H

#include <CQChartsColumn.h>
#include <CQChartsLineEditBase.h>

class CQChartsColumnsEdit;

class CQChartsColumnsLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumns columns READ columns WRITE setColumns)

 public:
  CQChartsColumnsLineEdit(QWidget *parent=nullptr);

  QAbstractItemModel *model() const;
  void setModel(QAbstractItemModel *model);

  const CQChartsColumns &columns() const;
  void setColumns(const CQChartsColumns &c);

 signals:
  void columnsChanged();

 private slots:
  void menuEditChanged();

 private:
  void updateColumns(const CQChartsColumns &columns, bool updateText);

  void textChanged() override;

  void updateMenu() override;

  void columnsToWidgets();

  void connectSlots(bool b);

  bool textToColumns(const QString &str, CQChartsColumns &columns) const;

 private:
  CQChartsColumnsEdit *menuEdit_ { nullptr };
};

//------

class CQChartsColumnLineEdit;

class CQChartsColumnsEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumns columns READ columns WRITE setColumns)

 public:
  CQChartsColumnsEdit(QWidget *parent=nullptr);

  QAbstractItemModel *model() const { return model_; }
  void setModel(QAbstractItemModel *model);

  const CQChartsColumns &columns() const;
  void setColumns(const CQChartsColumns &c);

  QSize sizeHint() const;

 signals:
  void columnsChanged();

 private slots:
  void addSlot();
  void removeSlot();

  void widgetsToColumn();

 private:
  void columnsToWidgets();

  void updateEdits();

  void connectSlots(bool b);

 private:
  using ColumnEdits = std::vector<CQChartsColumnLineEdit *>;

  CQChartsColumns     columns_;
  QAbstractItemModel* model_           { nullptr };
  QFrame*             controlFrame_    { nullptr };
  QFrame*             columnsFrame_    { nullptr };
  ColumnEdits         columnEdits_;
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsColumn
class CQChartsColumnsPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsColumnsPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsColumn
class CQChartsColumnsPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsColumnsPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
