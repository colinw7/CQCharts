#ifndef CQChartsEditAnnotationDlg_H
#define CQChartsEditAnnotationDlg_H

#include <QDialog>

class CQChartsAnnotation;
class CQChartsPositionEdit;
class CQChartsRectEdit;
class CQChartsLengthEdit;
class CQChartsSidesEdit;
class CQChartsPolygonEdit;
class CQChartsSymbolDataEdit;
class CQChartsTextDataEdit;
class CQChartsFillDataEdit;
class CQChartsStrokeDataEdit;
class CQChartsArrowDataEdit;
class CQChartsColorLineEdit;

class CQRealSpin;
class CQCheckBox;
class CQLineEdit;

class QFrame;
class QRadioButton;
class QBoxLayout;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

/*!
 * \brief dialog to create a new annotation
 * \ingroup Charts
 */
class CQChartsEditAnnotationDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsEditAnnotationDlg(QWidget *parent, CQChartsAnnotation *annotation);

 private:
  struct Widgets {
    QFrame*                 frame              { nullptr };
    CQChartsFillDataEdit*   backgroundDataEdit { nullptr };
    CQChartsStrokeDataEdit* strokeDataEdit     { nullptr };
    CQChartsSidesEdit*      borderSidesEdit    { nullptr };
  };

  struct RectWidgets : public Widgets {
    CQChartsRectEdit* rectEdit    { nullptr };
    CQRealSpin*       marginEdit  { nullptr };
    CQRealSpin*       paddingEdit { nullptr };
  };

  struct EllipseWidgets : public Widgets {
    CQChartsPositionEdit* centerEdit  { nullptr };
    CQChartsLengthEdit*   rxEdit      { nullptr };
    CQChartsLengthEdit*   ryEdit      { nullptr };
    CQLineEdit*           paddingEdit { nullptr };
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
    CQLineEdit*           textEdit      { nullptr };
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

  bool updateRectangleAnnotation();
  bool updateEllipseAnnotation  ();
  bool updatePolygonAnnotation  ();
  bool updatePolylineAnnotation ();
  bool updateTextAnnotation     ();
  bool updateArrowAnnotation    ();
  bool updatePointAnnotation    ();

 private slots:
  void textPositionSlot(bool);

  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  CQChartsAnnotation* annotation_       { nullptr }; //!< annotation
  CQLineEdit*         idEdit_           { nullptr }; //!< id edit
  CQLineEdit*         tipEdit_          { nullptr }; //!< tip edit
  QVBoxLayout*        frameLayout_      { nullptr }; //!< frame layout
  RectWidgets         rectWidgets_;                  //!< rect edit widgets
  EllipseWidgets      ellipseWidgets_;               //!< ellipse edit widgets
  PolygonWidgets      polygonWidgets_;               //!< polygon edit widgets
  PolylineWidgets     polylineWidgets_;              //!< polyline edit widgets
  TextWidgets         textWidgets_;                  //!< text edit widgets
  ArrowWidgets        arrowWidgets_;                 //!< arrow edit widgets
  PointWidgets        pointWidgets_;                 //!< point edit widgets
};

#endif
