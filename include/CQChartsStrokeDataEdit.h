#ifndef CQChartsStrokeDataEdit_H
#define CQChartsStrokeDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsStrokeDataEdit;
class CQChartsPlot;
class CQChartsView;

class CQChartsStrokeDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsStrokeData strokeData READ strokeData WRITE setStrokeData)

 public:
  CQChartsStrokeDataLineEdit(QWidget *parent=nullptr);

  CQChartsPlot *plot() const;
  void setPlot(CQChartsPlot *plot);

  CQChartsView *view() const;
  void setView(CQChartsView *view);

  const CQChartsStrokeData &strokeData() const;
  void setStrokeData(const CQChartsStrokeData &c);

 signals:
  void strokeDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateStrokeData(const CQChartsStrokeData &c, bool updateText);

  void strokeDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsStrokeDataEdit* menuEdit_ { nullptr };
};

//---

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsLengthEdit;
class CQChartsLineDashEdit;
class CQChartsStrokeDataEditPreview;
class QGroupBox;

class CQChartsStrokeDataEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsStrokeDataEdit(QWidget *parent=nullptr);

  CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *plot) { plot_ = plot; }

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view) { view_ = view; }

  const CQChartsStrokeData &data() const { return data_; }
  void setData(const CQChartsStrokeData &v) { data_ = v; }

  void setTitle(const QString &title);

  void setPreview(bool b);

 signals:
  void strokeDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                  plot_       { nullptr };
  CQChartsView*                  view_       { nullptr };
  CQChartsStrokeData             data_;
  QGroupBox*                     groupBox_   { nullptr };
  CQChartsColorLineEdit*         colorEdit_  { nullptr };
  CQChartsAlphaEdit*             alphaEdit_  { nullptr };
  CQChartsLengthEdit*            widthEdit_  { nullptr };
  CQChartsLineDashEdit*          dashEdit_   { nullptr };
  CQChartsLengthEdit*            cornerEdit_ { nullptr };
  CQChartsStrokeDataEditPreview* preview_    { nullptr };
};

//---

class CQChartsStrokeDataEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsStrokeDataEditPreview(CQChartsStrokeDataEdit *edit);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsStrokeDataEdit *edit_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsStrokeData
class CQChartsStrokeDataPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsStrokeDataPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsStrokeData
class CQChartsStrokeDataPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsStrokeDataPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
