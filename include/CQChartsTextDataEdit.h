#ifndef CQChartsTextDataEdit_H
#define CQChartsTextDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsTextDataEdit;
class CQChartsPlot;
class CQChartsView;

class CQChartsTextDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsTextData textData READ textData WRITE setTextData)

 public:
  CQChartsTextDataLineEdit(QWidget *parent=nullptr);

  CQChartsPlot *plot() const;
  void setPlot(CQChartsPlot *plot);

  CQChartsView *view() const;
  void setView(CQChartsView *view);

  const CQChartsTextData &textData() const;
  void setTextData(const CQChartsTextData &c);

 signals:
  void textDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateTextData(const CQChartsTextData &c, bool updateText);

  void textDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsTextDataEdit* menuEdit_ { nullptr };
};

//---

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsTextDataEditPreview;
class CQFontChooser;
class CQAngleSpinBox;
class CQAlignEdit;
class QGroupBox;
class QCheckBox;

class CQChartsTextDataEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsTextDataEdit(QWidget *parent=nullptr);

  CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *plot) { plot_ = plot; }

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view) { view_ = view; }

  const CQChartsTextData &data() const { return data_; }
  void setData(const CQChartsTextData &v) { data_ = v; }

 signals:
  void textDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                plot_          { nullptr };
  CQChartsView*                view_          { nullptr };
  CQChartsTextData             data_;
  QGroupBox*                   groupBox_      { nullptr };
  CQFontChooser*               fontEdit_      { nullptr };
  CQChartsColorLineEdit*       colorEdit_     { nullptr };
  CQChartsAlphaEdit*           alphaEdit_     { nullptr };
  CQAngleSpinBox*              angleEdit_     { nullptr };
  QCheckBox*                   contrastEdit_  { nullptr };
  CQAlignEdit*                 alignEdit_     { nullptr };
  QCheckBox*                   formattedEdit_ { nullptr };
  QCheckBox*                   scaledEdit_    { nullptr };
  QCheckBox*                   htmlEdit_      { nullptr };
  CQChartsTextDataEditPreview* preview_       { nullptr };
};

//---

class CQChartsTextDataEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsTextDataEditPreview(CQChartsTextDataEdit *edit);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsTextDataEdit *edit_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsTextData
class CQChartsTextDataPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsTextDataPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsTextData
class CQChartsTextDataPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsTextDataPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
