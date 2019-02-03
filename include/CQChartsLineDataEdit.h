#ifndef CQChartsLineDataEdit_H
#define CQChartsLineDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsLineDataEdit;
class CQChartsPlot;
class CQChartsView;

class CQChartsLineDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsLineData lineData READ lineData WRITE setLineData)

 public:
  CQChartsLineDataLineEdit(QWidget *parent=nullptr);

  CQChartsPlot *plot() const;
  void setPlot(CQChartsPlot *plot);

  CQChartsView *view() const;
  void setView(CQChartsView *view);

  const CQChartsLineData &lineData() const;
  void setLineData(const CQChartsLineData &c);

 signals:
  void lineDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateLineData(const CQChartsLineData &c, bool updateText);

  void lineDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsLineDataEdit* menuEdit_ { nullptr };
};

//---

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsLengthEdit;
class CQChartsLineDashEdit;
class CQChartsLineDataEditPreview;
class QGroupBox;

class CQChartsLineDataEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsLineDataEdit(QWidget *parent=nullptr);

  CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *plot) { plot_ = plot; }

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view) { view_ = view; }

  const CQChartsLineData &data() const { return data_; }
  void setData(const CQChartsLineData &v) { data_ = v; }

 signals:
  void lineDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                plot_      { nullptr };
  CQChartsView*                view_      { nullptr };
  CQChartsLineData             data_;
  QGroupBox*                   groupBox_  { nullptr };
  CQChartsColorLineEdit*       colorEdit_ { nullptr };
  CQChartsAlphaEdit*           alphaEdit_ { nullptr };
  CQChartsLengthEdit*          widthEdit_ { nullptr };
  CQChartsLineDashEdit*        dashEdit_  { nullptr };
  CQChartsLineDataEditPreview* preview_   { nullptr };
};

//---

class CQChartsLineDataEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsLineDataEditPreview(CQChartsLineDataEdit *edit);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsLineDataEdit *edit_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsLineData
class CQChartsLineDataPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsLineDataPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsLineData
class CQChartsLineDataPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsLineDataPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
