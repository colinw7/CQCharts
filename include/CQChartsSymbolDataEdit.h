#ifndef CQChartsSymbolDataEdit_H
#define CQChartsSymbolDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsSymbolDataEdit;
class CQChartsPlot;
class CQChartsView;

/*!
 * \brief Symbol Data line edit
 * \ingroup Charts
 */
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
class CQGroupBox;

/*!
 * \brief Symbol data edit
 * \ingroup Charts
 */
class CQChartsSymbolDataEdit : public CQChartsEditBase {
  Q_OBJECT

 public:
  CQChartsSymbolDataEdit(QWidget *parent=nullptr, bool optional=true);

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

  void connectSlots(bool b);

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                  plot_       { nullptr }; //!< parent plot
  CQChartsView*                  view_       { nullptr }; //!< parent view
  CQChartsSymbolData             data_;                   //!< symbol data
  CQGroupBox*                    groupBox_   { nullptr }; //!< group box
  CQChartsSymbolEdit*            symbolEdit_ { nullptr }; //!< symbol edit
  CQChartsLengthEdit*            sizeEdit_   { nullptr }; //!< length edit
  CQChartsStrokeDataEdit*        strokeEdit_ { nullptr }; //!< stroke edit
  CQChartsFillDataEdit*          fillEdit_   { nullptr }; //!< fill edit
  CQChartsSymbolDataEditPreview* preview_    { nullptr }; //!< preview
  bool                           connected_  { false };   //!< is connected
};

//---

/*!
 * \brief Symbol data edit preview
 * \ingroup Charts
 */
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

/*!
 * \brief type for CQChartsSymbolData
 * \ingroup Charts
 */
class CQChartsSymbolDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "symbol_data"; }
};

//---

/*!
 * \brief editor factory for CQChartsSymbolData
 * \ingroup Charts
 */
class CQChartsSymbolDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

#endif
