#ifndef CQChartsAnnotationDlg_H
#define CQChartsAnnotationDlg_H

#include <QDialog>

class CQChartsView;
class CQChartsPlot;
class CQChartsPositionEdit;
class CQChartsLengthEdit;
class CQChartsRectEdit;
class CQChartsAlphaEdit;

class CQColorChooser;
class CQFontChooser;
class CQAngleSpinBox;
class CQPoint2DEdit;
class CQAlignEdit;
class CQLineDash;
class CQRealSpin;
class CQCheckBox;

class QLineEdit;
class QStackedWidget;
class QFrame;
class QBoxLayout;
class QHBoxLayout;
class QGridLayout;

class CQChartsAnnotationDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsAnnotationDlg(CQChartsView *view);
  CQChartsAnnotationDlg(CQChartsPlot *plot);

 private:
  struct Widgets {
    QFrame*             frame           { nullptr };
    CQCheckBox*         backgroundCheck { nullptr };
    CQColorChooser*     backgroundColor { nullptr };
    CQChartsAlphaEdit*  backgroundAlpha { nullptr };
    CQCheckBox*         borderCheck     { nullptr };
    CQColorChooser*     borderColor     { nullptr };
    CQChartsAlphaEdit*  borderAlpha     { nullptr };
    CQChartsLengthEdit* borderWidth     { nullptr };
    CQLineDash*         borderDash      { nullptr };
  };

  struct RectWidgets : public Widgets {
    CQChartsRectEdit*   rectEdit        { nullptr };
    CQRealSpin*         marginEdit      { nullptr };
    CQRealSpin*         paddingEdit     { nullptr };
    CQChartsLengthEdit* cornerSizeEdit  { nullptr };
    QLineEdit*          borderSidesEdit { nullptr };
  };

  struct EllipseWidgets : public Widgets {
    CQChartsPositionEdit* centerEdit      { nullptr };
    CQChartsLengthEdit*   rxEdit          { nullptr };
    CQChartsLengthEdit*   ryEdit          { nullptr };
    QLineEdit*            paddingEdit     { nullptr };
    CQChartsLengthEdit*   cornerSizeEdit  { nullptr };
    QLineEdit*            borderSidesEdit { nullptr };
  };

  struct PolygonWidgets : public Widgets {
    QLineEdit* pointsEdit { nullptr };
  };

  struct PolylineWidgets : public Widgets {
    QLineEdit* pointsEdit { nullptr };
  };

  struct TextWidgets : public Widgets {
    CQChartsPositionEdit* positionEdit    { nullptr };
    QLineEdit*            textEdit        { nullptr };
    CQFontChooser*        fontEdit        { nullptr };
    CQColorChooser*       colorEdit       { nullptr };
    CQChartsAlphaEdit*    alphaEdit       { nullptr };
    CQAngleSpinBox*       angleEdit       { nullptr };
    CQCheckBox*           contrastCheck   { nullptr };
    CQAlignEdit*          alignEdit       { nullptr };
    CQCheckBox*           htmlCheck       { nullptr };
    CQChartsLengthEdit*   cornerSizeEdit  { nullptr };
    QLineEdit*            borderSidesEdit { nullptr };
  };

  struct ArrowWidgets : public Widgets {
    CQChartsPositionEdit* startEdit       { nullptr };
    CQChartsPositionEdit* endEdit         { nullptr };
    CQChartsLengthEdit*   lengthEdit      { nullptr };
    CQAngleSpinBox*       angleEdit       { nullptr };
    CQAngleSpinBox*       backAngleEdit   { nullptr };
    CQCheckBox*           fheadCheck      { nullptr };
    CQCheckBox*           theadCheck      { nullptr };
//  CQCheckBox*           emptyCheck      { nullptr };
    CQCheckBox*           lineEndsCheck   { nullptr };
    CQChartsLengthEdit*   lineWidthEdit   { nullptr };
    CQChartsLengthEdit*   borderWidthEdit { nullptr };
    CQColorChooser*       borderColorEdit { nullptr };
    CQCheckBox*           filledCheck     { nullptr };
    CQColorChooser*       fillColorEdit   { nullptr };
  };

  struct PointWidgets : public Widgets {
    CQChartsPositionEdit* positionEdit  { nullptr };
    CQChartsLengthEdit*   sizeEdit      { nullptr };
    QLineEdit*            typeEdit      { nullptr };
    CQCheckBox*           strokedCheck  { nullptr };
    CQCheckBox*           filledCheck   { nullptr };
    CQChartsLengthEdit*   lineWidthEdit { nullptr };
    CQColorChooser*       lineColorEdit { nullptr };
    CQChartsAlphaEdit*    lineAlphaEdit { nullptr };
    CQColorChooser*       fillColorEdit { nullptr };
    CQChartsAlphaEdit*    fillAlphaEdit { nullptr };
  };

 private:
  void initWidgets();

  void createRectFrame    ();
  void createEllipseFrame ();
  void createPolygonFrame ();
  void createPolyLineFrame();
  void createTextFrame    ();
  void createArrowFrame   ();
  void createPointFrame   ();

  void addFillWidgets  (Widgets &widgets, QBoxLayout *playout);
  void addStrokeWidgets(Widgets &widgets, QBoxLayout *playout);

  void addGridLabelWidget(QGridLayout *playout, const QString &label, QWidget *widget, int &row);

  QHBoxLayout *addLabelWidget(QBoxLayout *playout, const QString &label, QWidget *widget);

  bool createRectAnnotation    ();
  bool createEllipseAnnotation ();
  bool createPolygonAnnotation ();
  bool createPolylineAnnotation();
  bool createTextAnnotation    ();
  bool createArrowAnnotation   ();
  bool createPointAnnotation   ();

 private slots:
  void typeSlot(int ind);

  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  CQChartsView*   view_      { nullptr };
  CQChartsPlot*   plot_      { nullptr };
  QLineEdit*      idEdit_    { nullptr };
  QLineEdit*      tipEdit_   { nullptr };
  QStackedWidget* typeStack_ { nullptr };
  RectWidgets     rectWidgets_;
  EllipseWidgets  ellipseWidgets_;
  PolygonWidgets  polygonWidgets_;
  PolylineWidgets polylineWidgets_;
  TextWidgets     textWidgets_;
  ArrowWidgets    arrowWidgets_;
  PointWidgets    pointWidgets_;
};

#endif
