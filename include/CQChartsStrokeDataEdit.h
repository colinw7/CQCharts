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

/*!
 * \brief Stroke Data line edit
 * \ingroup Charts
 */
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

/*!
 * \brief Stroke data edit
 * \ingroup Charts
 */
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

  void connectSlots(bool b);

 private slots:
  void widgetsToData();

 private:
  CQChartsStrokeDataEditConfig   config_;                 //!< edit config
  CQChartsPlot*                  plot_       { nullptr }; //!< parent plot
  CQChartsView*                  view_       { nullptr }; //!< parent view
  CQChartsStrokeData             data_;                   //!< stroke data
  CQGroupBox*                    groupBox_   { nullptr }; //!< group box
  CQChartsColorLineEdit*         colorEdit_  { nullptr }; //!< color edit
  CQChartsAlphaEdit*             alphaEdit_  { nullptr }; //!< alpha edit
  CQChartsLengthEdit*            widthEdit_  { nullptr }; //!< width edit
  CQChartsLineDashEdit*          dashEdit_   { nullptr }; //!< dash edit
  CQChartsLengthEdit*            cornerEdit_ { nullptr }; //!< corner edit
  CQChartsStrokeDataEditPreview* preview_    { nullptr }; //!< preview
  bool                           connected_  { false };   //!< is connected
};

//---

/*!
 * \brief Stroke data edit preview
 * \ingroup Charts
 */
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

/*!
 * \brief type for CQChartsStrokeData
 * \ingroup Charts
 */
class CQChartsStrokeDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "stroke_data"; }
};

//---

/*!
 * \brief editor factory for CQChartsStrokeData
 * \ingroup Charts
 */
class CQChartsStrokeDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
