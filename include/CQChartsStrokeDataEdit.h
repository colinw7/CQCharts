#ifndef CQChartsStrokeDataEdit_H
#define CQChartsStrokeDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

struct CQChartsStrokeDataEditConfig {
  bool cornerSize { true };

  CQChartsStrokeDataEditConfig &setCornerSize(bool b) { cornerSize = b; return *this; }
};

//---

class CQChartsStrokeDataEdit;
class CQChartsPlot;
class CQChartsView;

class CQChartsStrokeDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsStrokeData strokeData READ strokeData WRITE setStrokeData)

 public:
  CQChartsStrokeDataLineEdit(QWidget *parent=nullptr);

  const CQChartsStrokeData &strokeData() const;
  void setStrokeData(const CQChartsStrokeData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

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
  CQChartsStrokeDataEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsLengthEdit;
class CQChartsLineDashEdit;
class CQChartsStrokeDataEditPreview;
class CQGroupBox;

class CQChartsStrokeDataEdit : public CQChartsEditBase {
  Q_OBJECT

 public:
  CQChartsStrokeDataEdit(QWidget *parent=nullptr,
   const CQChartsStrokeDataEditConfig &config=CQChartsStrokeDataEditConfig());

  const CQChartsStrokeData &data() const { return data_; }
  void setData(const CQChartsStrokeData &d);

  void setTitle(const QString &title);

  void setPreview(bool b);

 signals:
  void strokeDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsStrokeDataEditConfig   config_;
  CQChartsPlot*                  plot_       { nullptr };
  CQChartsView*                  view_       { nullptr };
  CQChartsStrokeData             data_;
  CQGroupBox*                    groupBox_   { nullptr };
  CQChartsColorLineEdit*         colorEdit_  { nullptr };
  CQChartsAlphaEdit*             alphaEdit_  { nullptr };
  CQChartsLengthEdit*            widthEdit_  { nullptr };
  CQChartsLineDashEdit*          dashEdit_   { nullptr };
  CQChartsLengthEdit*            cornerEdit_ { nullptr };
  CQChartsStrokeDataEditPreview* preview_    { nullptr };
};

//---

class CQChartsStrokeDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  CQChartsStrokeDataEditPreview(CQChartsStrokeDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const CQChartsStrokeData &data, const QRect &rect,
                   CQChartsPlot *plot, CQChartsView *view);

 private:
  CQChartsStrokeDataEdit *edit_ { nullptr };
};

//------

#include <CQChartsPropertyViewEditor.h>

// type for CQChartsStrokeData
class CQChartsStrokeDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;
};

//---

// editor factory for CQChartsStrokeData
class CQChartsStrokeDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
