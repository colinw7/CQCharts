#ifndef CQChartsShapeDataEdit_H
#define CQChartsShapeDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsShapeDataEdit;
class CQChartsPlot;
class CQChartsView;

class CQChartsShapeDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsShapeData shapeData READ shapeData WRITE setShapeData)

 public:
  CQChartsShapeDataLineEdit(QWidget *parent=nullptr);

  CQChartsPlot *plot() const;
  void setPlot(CQChartsPlot *plot);

  CQChartsView *view() const;
  void setView(CQChartsView *view);

  const CQChartsShapeData &shapeData() const;
  void setShapeData(const CQChartsShapeData &c);

 signals:
  void shapeDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateShapeData(const CQChartsShapeData &c, bool updateText);

  void shapeDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsShapeDataEdit* menuEdit_ { nullptr };
};

//---

class CQChartsFillDataEdit;
class CQChartsStrokeDataEdit;
class CQChartsShapeDataEditPreview;
class QGroupBox;

class CQChartsShapeDataEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsShapeDataEdit(QWidget *parent=nullptr);

  CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *plot);

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view);

  const CQChartsShapeData &data() const { return data_; }
  void setData(const CQChartsShapeData &v) { data_ = v; }

 signals:
  void shapeDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                 plot_       { nullptr };
  CQChartsView*                 view_       { nullptr };
  CQChartsShapeData             data_;
  CQChartsFillDataEdit*         fillEdit_   { nullptr };
  CQChartsStrokeDataEdit*       strokeEdit_ { nullptr };
  CQChartsShapeDataEditPreview* preview_    { nullptr };
};

//---

class CQChartsShapeDataEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsShapeDataEditPreview(CQChartsShapeDataEdit *edit);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsShapeDataEdit *edit_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsShapeData
class CQChartsShapeDataPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsShapeDataPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsShapeData
class CQChartsShapeDataPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsShapeDataPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
