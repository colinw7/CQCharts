#ifndef CQChartsCreateAnnotationDlg_H
#define CQChartsCreateAnnotationDlg_H

#include <QDialog>

class CQChartsView;
class CQChartsPlot;
class CQChartsPositionEdit;
class CQChartsRectEdit;
class CQChartsLengthEdit;
class CQChartsMarginEdit;
class CQChartsSidesEdit;
class CQChartsPolygonEdit;
class CQChartsSymbolDataEdit;
class CQChartsTextDataEdit;
class CQChartsFillDataEdit;
class CQChartsStrokeDataEdit;
class CQChartsArrowDataEdit;
class CQChartsColorLineEdit;
class CQChartsLineEdit;

class CQRealSpin;
class CQCheckBox;

class QStackedWidget;
class QFrame;
class QComboBox;
class QRadioButton;
class QLabel;
class QBoxLayout;
class QHBoxLayout;
class QGridLayout;

/*!
 * \brief dialog to create a new annotation
 * \ingroup Charts
 */
class CQChartsCreateAnnotationDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsCreateAnnotationDlg(QWidget *parent, CQChartsView *view);
  CQChartsCreateAnnotationDlg(QWidget *parent, CQChartsPlot *plot);

  CQChartsView *view() const { return view_; }
  CQChartsPlot *plot() const { return plot_; }

 private:
  struct Widgets {
    QFrame*                 frame              { nullptr };
    CQChartsFillDataEdit*   backgroundDataEdit { nullptr };
    CQChartsStrokeDataEdit* strokeDataEdit     { nullptr };
    CQChartsSidesEdit*      borderSidesEdit    { nullptr };
  };

  struct RectWidgets : public Widgets {
    CQChartsRectEdit*   rectEdit    { nullptr };
    CQChartsMarginEdit* marginEdit  { nullptr };
    CQChartsMarginEdit* paddingEdit { nullptr };
  };

  struct EllipseWidgets : public Widgets {
    CQChartsPositionEdit* centerEdit  { nullptr };
    CQChartsLengthEdit*   rxEdit      { nullptr };
    CQChartsLengthEdit*   ryEdit      { nullptr };
    CQChartsLineEdit*     paddingEdit { nullptr };
  };

  struct PolygonWidgets : public Widgets {
    CQChartsPolygonEdit* pointsEdit { nullptr };
  };

  struct PolylineWidgets : public Widgets {
    CQChartsPolygonEdit* pointsEdit { nullptr };
  };

  struct TextWidgets : public Widgets {
    QRadioButton*         positionRadio { nullptr };
    QRadioButton*         rectRadio     { nullptr };
    CQChartsPositionEdit* positionEdit  { nullptr };
    CQChartsRectEdit*     rectEdit      { nullptr };
    CQChartsLineEdit*     textEdit      { nullptr };
    CQChartsTextDataEdit* dataEdit      { nullptr };
  };

  struct ArrowWidgets : public Widgets {
    CQChartsPositionEdit*  startEdit       { nullptr };
    CQChartsPositionEdit*  endEdit         { nullptr };
    CQChartsArrowDataEdit* dataEdit        { nullptr };
    CQChartsLengthEdit*    strokeWidthEdit { nullptr };
    CQChartsColorLineEdit* strokeColorEdit { nullptr };
    CQCheckBox*            filledCheck     { nullptr };
    CQChartsColorLineEdit* fillColorEdit   { nullptr };
  };

  struct PointWidgets : public Widgets {
    CQChartsPositionEdit*   positionEdit { nullptr };
    CQChartsSymbolDataEdit* dataEdit     { nullptr };
  };

 private:
  QStringList typeNames() const;

  void initWidgets();

  void createRectFrame    ();
  void createEllipseFrame ();
  void createPolygonFrame ();
  void createPolyLineFrame();
  void createTextFrame    ();
  void createArrowFrame   ();
  void createPointFrame   ();

  void addFillWidgets  (Widgets &widgets, QBoxLayout *playout);
  void addStrokeWidgets(Widgets &widgets, QBoxLayout *playout, bool corner);

  void addSidesWidget(Widgets &widgets, QBoxLayout *playout);

  QHBoxLayout *addLabelWidget(QBoxLayout *playout, const QString &label, QWidget *widget);

  bool createRectangleAnnotation();
  bool createEllipseAnnotation  ();
  bool createPolygonAnnotation  ();
  bool createPolylineAnnotation ();
  bool createTextAnnotation     ();
  bool createArrowAnnotation    ();
  bool createPointAnnotation    ();

  bool setErrorMsg(const QString &msg);
  void clearErrorMsg();

 private slots:
  void typeSlot(int ind);

  void textPositionSlot(bool);

  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  CQChartsView*     view_             { nullptr }; //!< associated view
  CQChartsPlot*     plot_             { nullptr }; //!< associated plot
  QComboBox*        typeCombo_        { nullptr }; //!< type combo
  CQChartsLineEdit* idEdit_           { nullptr }; //!< id edit
  CQChartsLineEdit* tipEdit_          { nullptr }; //!< tip edit
  QStackedWidget*   typeStack_        { nullptr }; //!< type stacked widget
  RectWidgets       rectWidgets_;                  //!< rect edit widgets
  EllipseWidgets    ellipseWidgets_;               //!< ellipse edit widgets
  PolygonWidgets    polygonWidgets_;               //!< polygon edit widgets
  PolylineWidgets   polylineWidgets_;              //!< polyline edit widgets
  TextWidgets       textWidgets_;                  //!< text edit widgets
  ArrowWidgets      arrowWidgets_;                 //!< arrow edit widgets
  PointWidgets      pointWidgets_;                 //!< point edit widgets
  QLabel*           msgLabel_         { nullptr }; //!< error message label
};

#endif
