#ifndef CQChartsColumnsEdit_H
#define CQChartsColumnsEdit_H

#include <CQChartsColumn.h>
#include <QFrame>
#include <QStyleOptionComboBox>

class CQChartsColumnEdit;
class CQWidgetMenu;
class QLineEdit;
class QPushButton;

class CQChartsColumnsEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumns columns         READ columns         WRITE setColumns        )
  Q_PROPERTY(QString         placeholderText READ placeholderText WRITE setPlaceholderText)

 public:
  CQChartsColumnsEdit(QWidget *parent=nullptr);

  QAbstractItemModel *model() const { return model_; }
  void setModel(QAbstractItemModel *model);

  const CQChartsColumns &columns() const;
  void setColumns(const CQChartsColumns &c);

  QString text() const;
  void setText(const QString &s);

  QString placeholderText() const;
  void setPlaceholderText(const QString &s);

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

 signals:
  void columnsChanged();

 private slots:
  void showMenu();

  void updateMenu();

  void textChanged(const QString &str);

  void addSlot();
  void removeSlot();

  void updateState();

  void columnSlot();

 private:
  void initStyle(QStyleOptionComboBox &opt);

  void textToColumns();
  void columnsToText();

  void connectSlots(bool b);

 private:
  using ColumnEdits = std::vector<CQChartsColumnEdit *>;

  CQChartsColumns     columns_;
  QLineEdit*          edit_            { nullptr };
  QPushButton*        button_          { nullptr };
  CQWidgetMenu*       menu_            { nullptr };
  QAbstractItemModel* model_           { nullptr };
  QFrame*             menuFrame_       { nullptr };
  QFrame*             controlFrame_    { nullptr };
  QFrame*             columnsFrame_    { nullptr };
  ColumnEdits         columnEdits_;
};

//---

#include <QPushButton>

class CQChartsColumnsEditMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQChartsColumnsEditMenuButton(QWidget *parent=nullptr);

  void paintEvent(QPaintEvent *) override;
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
