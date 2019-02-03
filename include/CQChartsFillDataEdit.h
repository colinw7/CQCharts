#ifndef CQChartsFillDataEdit_H
#define CQChartsFillDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsFillDataEdit;
class CQChartsPlot;
class CQChartsView;

class CQChartsFillDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillData fillData READ fillData WRITE setFillData)

 public:
  CQChartsFillDataLineEdit(QWidget *parent=nullptr);

  CQChartsPlot *plot() const;
  void setPlot(CQChartsPlot *plot);

  CQChartsView *view() const;
  void setView(CQChartsView *view);

  const CQChartsFillData &fillData() const;
  void setFillData(const CQChartsFillData &c);

 signals:
  void fillDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateFillData(const CQChartsFillData &c, bool updateText);

  void fillDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsFillDataEdit* menuEdit_ { nullptr };
};

//---

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsFillPatternEdit;
class CQChartsFillDataEditPreview;
class QGroupBox;

class CQChartsFillDataEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsFillDataEdit(QWidget *parent=nullptr);

  CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *plot) { plot_ = plot; }

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view) { view_ = view; }

  const CQChartsFillData &data() const { return data_; }
  void setData(const CQChartsFillData &v) { data_ = v; }

  void setTitle(const QString &title);

  void setPreview(bool b);

 signals:
  void fillDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                plot_        { nullptr };
  CQChartsView*                view_        { nullptr };
  CQChartsFillData             data_;
  QGroupBox*                   groupBox_    { nullptr };
  CQChartsColorLineEdit*       colorEdit_   { nullptr };
  CQChartsAlphaEdit*           alphaEdit_   { nullptr };
  CQChartsFillPatternEdit*     patternEdit_ { nullptr };
  CQChartsFillDataEditPreview* preview_     { nullptr };
};

//---

class CQChartsFillDataEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsFillDataEditPreview(CQChartsFillDataEdit *edit);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsFillDataEdit *edit_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsFillData
class CQChartsFillDataPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsFillDataPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsFillData
class CQChartsFillDataPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsFillDataPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
