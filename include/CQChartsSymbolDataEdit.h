#ifndef CQChartsSymbolDataEdit_H
#define CQChartsSymbolDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsSymbolDataEdit;
class CQChartsPlot;
class CQChartsView;

class CQChartsSymbolDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsSymbolData symbolData READ symbolData WRITE setSymbolData)

 public:
  CQChartsSymbolDataLineEdit(QWidget *parent=nullptr);

  const CQChartsSymbolData &symbolData() const;
  void setSymbolData(const CQChartsSymbolData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void symbolDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateSymbolData(const CQChartsSymbolData &c, bool updateText);

  void symbolDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsSymbolDataEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsSymbolEdit;
class CQChartsLengthEdit;
class CQChartsStrokeDataEdit;
class CQChartsFillDataEdit;
class CQChartsSymbolDataEditPreview;
class QGroupBox;

class CQChartsSymbolDataEdit : public CQChartsEditBase {
  Q_OBJECT

 public:
  CQChartsSymbolDataEdit(QWidget *parent=nullptr);

  const CQChartsSymbolData &data() const { return data_; }
  void setData(const CQChartsSymbolData &d);

  void setPlot(CQChartsPlot *plot) override;
  void setView(CQChartsView *view) override;

  void setTitle(const QString &title);

  void setPreview(bool b);

 signals:
  void symbolDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                  plot_        { nullptr };
  CQChartsView*                  view_        { nullptr };
  CQChartsSymbolData             data_;
  QGroupBox*                     groupBox_    { nullptr };
  CQChartsSymbolEdit*            symbolEdit_  { nullptr };
  CQChartsLengthEdit*            sizeEdit_    { nullptr };
  CQChartsStrokeDataEdit*        strokeEdit_  { nullptr };
  CQChartsFillDataEdit*          fillEdit_    { nullptr };
  CQChartsSymbolDataEditPreview* preview_     { nullptr };
};

//---

class CQChartsSymbolDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  CQChartsSymbolDataEditPreview(CQChartsSymbolDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const CQChartsSymbolData &data, const QRect &rect,
                   CQChartsPlot *plot, CQChartsView *view);

 private:
  CQChartsSymbolDataEdit *edit_ { nullptr };
};

//------

#include <CQChartsPropertyViewEditor.h>

// type for CQChartsSymbolData
class CQChartsSymbolDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;
};

//---

// editor factory for CQChartsSymbolData
class CQChartsSymbolDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

#endif
