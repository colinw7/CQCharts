#ifndef CQChartsPropertyMeta_H
#define CQChartsPropertyMeta_H

#define CQChartsPropertyMetaProp(T) \
  Q_PROPERTY(CQCharts##T T READ get##T WRITE set##T)
#define CQChartsPropertyMetaGeomProp(T) \
  Q_PROPERTY(CQChartsGeom::T Geom##T READ getGeom##T WRITE setGeom##T)

#define CQChartsPropertyMetaAccessor(T) \
 public: \
  CQCharts##T get##T() const { return _##T; } \
  void set##T(const CQCharts##T &t) { _##T = t; } \
\
  QString get##T##Str() const { return _##T.toString(); } \
  void set##T##Str(const QString &s) { _##T.fromString(s); } \
 private: \
  CQCharts##T _##T;

#define CQChartsPropertyMetaGeomAccessor(T) \
 public: \
  CQChartsGeom::T getGeom##T() const { return _Geom##T; } \
  void setGeom##T(const CQChartsGeom::T &t) { _Geom##T = t; } \
\
  QString getGeom##T##Str() const { return _Geom##T.toString(); } \
  void setGeom##T##Str(const QString &s) { _Geom##T.fromString(s); } \
 private: \
  CQChartsGeom::T _Geom##T;

#define CQChartsPropertyMetaGetTypeStr(T) \
  if (type == #T) { str = get##T##Str(); return true; }
#define CQChartsPropertyMetaSetTypeStr(T) \
  if (type == #T) { set##T##Str(str); return true; }

#define CQChartsPropertyMetaGetGeomTypeStr(T) \
  if (type == "Geom" #T) { str = getGeom##T##Str(); return true; }
#define CQChartsPropertyMetaSetGeomTypeStr(T) \
  if (type == "Geom" #T) { setGeom##T##Str(str); return true; }

#include <CQChartsAlpha.h>
#include <CQChartsAngle.h>
#include <CQChartsArea.h>
#include <CQChartsArrowData.h>
#include <CQChartsAxisSide.h>
#include <CQChartsAxisTickLabelPlacement.h>
#include <CQChartsAxisValueType.h>
#include <CQChartsColor.h>
#include <CQChartsColorMap.h>
#include <CQChartsColorStops.h>
#include <CQChartsColumn.h>
#include <CQChartsColumnNum.h>
#include <CQChartsConnectionList.h>
#include <CQChartsData.h>
#include <CQChartsFillPattern.h>
#include <CQChartsFillUnder.h>
#include <CQChartsFont.h>
#include <CQChartsImage.h>
#include <CQChartsKeyLocation.h>
#include <CQChartsKeyPressBehavior.h>
#include <CQChartsLength.h>
#include <CQChartsLineCap.h>
#include <CQChartsLineDash.h>
#include <CQChartsLineJoin.h>
#include <CQChartsMargin.h>
#include <CQChartsModelColumn.h>
#include <CQChartsModelInd.h>
#include <CQChartsModelIndex.h>
#include <CQChartsNamePair.h>
#include <CQChartsObjRefPos.h>
#include <CQChartsPaletteName.h>
#include <CQChartsPoints.h>
#include <CQChartsPolygon.h>
#include <CQChartsPosition.h>
#include <CQChartsReals.h>
#include <CQChartsRect.h>
#include <CQChartsSides.h>
#include <CQChartsSymbolSizeMap.h>
#include <CQChartsSymbolTypeMap.h>
#include <CQChartsThemeName.h>
#include <CQChartsTitleLocation.h>
#include <CQChartsUnits.h>
#include <CQChartsValueList.h>
#include <CQChartsWidget.h>

#include <CQChartsOptBool.h>
#include <CQChartsOptInt.h>
#include <CQChartsOptString.h>
#include <CQChartsOptPosition.h>
#include <CQChartsOptRect.h>

#include <CQUtil.h>

class CQChartsPropertyMeta : public QObject {
  Q_OBJECT

  CQChartsPropertyMetaProp(Alpha                 )
  CQChartsPropertyMetaProp(Angle                 )
  CQChartsPropertyMetaProp(Area                  )
  CQChartsPropertyMetaProp(ArrowData             )
  CQChartsPropertyMetaProp(AxisSide              )
  CQChartsPropertyMetaProp(AxisTickLabelPlacement)
  CQChartsPropertyMetaProp(AxisValueType         )
  CQChartsPropertyMetaProp(BoxData               )
  CQChartsPropertyMetaProp(Color                 )
  CQChartsPropertyMetaProp(ColorMap              )
  CQChartsPropertyMetaProp(ColorStops            )
  CQChartsPropertyMetaProp(Column                )
  CQChartsPropertyMetaProp(Columns               )
  CQChartsPropertyMetaProp(ColumnNum             )
  CQChartsPropertyMetaProp(ConnectionList        )
  CQChartsPropertyMetaProp(FillData              )
  CQChartsPropertyMetaProp(FillPattern           )
  CQChartsPropertyMetaProp(FillUnderPos          )
  CQChartsPropertyMetaProp(FillUnderSide         )
  CQChartsPropertyMetaProp(Font                  )
  CQChartsPropertyMetaProp(Image                 )
  CQChartsPropertyMetaProp(KeyLocation           )
  CQChartsPropertyMetaProp(KeyPressBehavior      )
  CQChartsPropertyMetaProp(Length                )
  CQChartsPropertyMetaProp(LineCap               )
  CQChartsPropertyMetaProp(LineDash              )
  CQChartsPropertyMetaProp(LineData              )
  CQChartsPropertyMetaProp(LineJoin              )
  CQChartsPropertyMetaProp(Margin                )
  CQChartsPropertyMetaProp(ModelColumn           )
  CQChartsPropertyMetaProp(ModelIndex            )
  CQChartsPropertyMetaProp(ModelInd              )
  CQChartsPropertyMetaProp(NamePair              )
  CQChartsPropertyMetaProp(ObjRef                )
  CQChartsPropertyMetaProp(ObjRefPos             )
  CQChartsPropertyMetaProp(PaletteName           )
  CQChartsPropertyMetaProp(Path                  )
  CQChartsPropertyMetaProp(Points                )
  CQChartsPropertyMetaProp(Polygon               )
  CQChartsPropertyMetaProp(PolygonList           )
  CQChartsPropertyMetaProp(Position              )
  CQChartsPropertyMetaProp(Reals                 )
  CQChartsPropertyMetaProp(Rect                  )
  CQChartsPropertyMetaProp(ShapeData             )
  CQChartsPropertyMetaProp(ShapeType             )
  CQChartsPropertyMetaProp(Sides                 )
  CQChartsPropertyMetaProp(StrokeData            )
  CQChartsPropertyMetaProp(Style                 )
  CQChartsPropertyMetaProp(Symbol                )
  CQChartsPropertyMetaProp(SymbolData            )
  CQChartsPropertyMetaProp(SymbolSizeMap         )
  CQChartsPropertyMetaProp(SymbolType            )
  CQChartsPropertyMetaProp(SymbolTypeMap         )
  CQChartsPropertyMetaProp(TextBoxData           )
  CQChartsPropertyMetaProp(TextData              )
  CQChartsPropertyMetaProp(ThemeName             )
  CQChartsPropertyMetaProp(TitleLocation         )
  CQChartsPropertyMetaProp(Units                 )
  CQChartsPropertyMetaProp(ValueList             )
  CQChartsPropertyMetaProp(Widget                )

  CQChartsPropertyMetaGeomProp(BBox )
  CQChartsPropertyMetaGeomProp(Point)

  CQChartsPropertyMetaProp(OptBool    )
  CQChartsPropertyMetaProp(OptInt     )
  CQChartsPropertyMetaProp(OptReal    )
  CQChartsPropertyMetaProp(OptString  )
  CQChartsPropertyMetaProp(OptLength  )
  CQChartsPropertyMetaProp(OptPosition)
  CQChartsPropertyMetaProp(OptRect    )

  //---

  CQChartsPropertyMetaAccessor(Alpha                 )
  CQChartsPropertyMetaAccessor(Angle                 )
  CQChartsPropertyMetaAccessor(Area                  )
  CQChartsPropertyMetaAccessor(ArrowData             )
  CQChartsPropertyMetaAccessor(AxisSide              )
  CQChartsPropertyMetaAccessor(AxisTickLabelPlacement)
  CQChartsPropertyMetaAccessor(AxisValueType         )
  CQChartsPropertyMetaAccessor(BoxData               )
  CQChartsPropertyMetaAccessor(Color                 )
  CQChartsPropertyMetaAccessor(ColorMap              )
  CQChartsPropertyMetaAccessor(ColorStops            )
  CQChartsPropertyMetaAccessor(Column                )
  CQChartsPropertyMetaAccessor(Columns               )
  CQChartsPropertyMetaAccessor(ColumnNum             )
  CQChartsPropertyMetaAccessor(ConnectionList        )
  CQChartsPropertyMetaAccessor(FillData              )
  CQChartsPropertyMetaAccessor(FillPattern           )
  CQChartsPropertyMetaAccessor(FillUnderPos          )
  CQChartsPropertyMetaAccessor(FillUnderSide         )
  CQChartsPropertyMetaAccessor(Font                  )
  CQChartsPropertyMetaAccessor(Image                 )
  CQChartsPropertyMetaAccessor(KeyLocation           )
  CQChartsPropertyMetaAccessor(KeyPressBehavior      )
  CQChartsPropertyMetaAccessor(Length                )
  CQChartsPropertyMetaAccessor(LineCap               )
  CQChartsPropertyMetaAccessor(LineDash              )
  CQChartsPropertyMetaAccessor(LineData              )
  CQChartsPropertyMetaAccessor(LineJoin              )
  CQChartsPropertyMetaAccessor(Margin                )
  CQChartsPropertyMetaAccessor(ModelColumn           )
  CQChartsPropertyMetaAccessor(ModelIndex            )
  CQChartsPropertyMetaAccessor(ModelInd              )
  CQChartsPropertyMetaAccessor(NamePair              )
  CQChartsPropertyMetaAccessor(ObjRef                )
  CQChartsPropertyMetaAccessor(ObjRefPos             )
  CQChartsPropertyMetaAccessor(PaletteName           )
  CQChartsPropertyMetaAccessor(Path                  )
  CQChartsPropertyMetaAccessor(Points                )
  CQChartsPropertyMetaAccessor(Polygon               )
  CQChartsPropertyMetaAccessor(PolygonList           )
  CQChartsPropertyMetaAccessor(Position              )
  CQChartsPropertyMetaAccessor(Reals                 )
  CQChartsPropertyMetaAccessor(Rect                  )
  CQChartsPropertyMetaAccessor(ShapeData             )
  CQChartsPropertyMetaAccessor(ShapeType             )
  CQChartsPropertyMetaAccessor(Sides                 )
  CQChartsPropertyMetaAccessor(StrokeData            )
  CQChartsPropertyMetaAccessor(Style                 )
  CQChartsPropertyMetaAccessor(Symbol                )
  CQChartsPropertyMetaAccessor(SymbolData            )
  CQChartsPropertyMetaAccessor(SymbolSizeMap         )
  CQChartsPropertyMetaAccessor(SymbolType            )
  CQChartsPropertyMetaAccessor(SymbolTypeMap         )
  CQChartsPropertyMetaAccessor(TextBoxData           )
  CQChartsPropertyMetaAccessor(TextData              )
  CQChartsPropertyMetaAccessor(ThemeName             )
  CQChartsPropertyMetaAccessor(TitleLocation         )
  CQChartsPropertyMetaAccessor(Units                 )
  CQChartsPropertyMetaAccessor(ValueList             )
  CQChartsPropertyMetaAccessor(Widget                )

  CQChartsPropertyMetaGeomAccessor(BBox )
  CQChartsPropertyMetaGeomAccessor(Point)

  CQChartsPropertyMetaAccessor(OptBool    )
  CQChartsPropertyMetaAccessor(OptInt     )
  CQChartsPropertyMetaAccessor(OptReal    )
  CQChartsPropertyMetaAccessor(OptString  )
  CQChartsPropertyMetaAccessor(OptLength  )
  CQChartsPropertyMetaAccessor(OptPosition)
  CQChartsPropertyMetaAccessor(OptRect    )

 public:
  CQChartsPropertyMeta(QObject *parent) :
   QObject(parent) {
  }

  QStringList names() const {
    auto names = CQUtil::getPropertyList(this, /*inherited*/false);

    QStringList strs;

    for (auto &name : names) {
      if (name.left(4) == "Geom")
        strs.push_back("CQChartsGeom::" + name.mid(4));
      else
        strs.push_back("CQCharts" + name);
    }

    return strs;
  }

  bool getTypeStr(const QString &type, QString &str) const {
    CQChartsPropertyMetaGetTypeStr(Alpha                 )
    CQChartsPropertyMetaGetTypeStr(Angle                 )
    CQChartsPropertyMetaGetTypeStr(Area                  )
    CQChartsPropertyMetaGetTypeStr(ArrowData             )
    CQChartsPropertyMetaGetTypeStr(AxisSide              )
    CQChartsPropertyMetaGetTypeStr(AxisTickLabelPlacement)
    CQChartsPropertyMetaGetTypeStr(AxisValueType         )
    CQChartsPropertyMetaGetTypeStr(BoxData               )
    CQChartsPropertyMetaGetTypeStr(Color                 )
    CQChartsPropertyMetaGetTypeStr(ColorMap              )
    CQChartsPropertyMetaGetTypeStr(ColorStops            )
    CQChartsPropertyMetaGetTypeStr(Column                )
    CQChartsPropertyMetaGetTypeStr(Columns               )
    CQChartsPropertyMetaGetTypeStr(ColumnNum             )
    CQChartsPropertyMetaGetTypeStr(ConnectionList        )
    CQChartsPropertyMetaGetTypeStr(FillData              )
    CQChartsPropertyMetaGetTypeStr(FillPattern           )
    CQChartsPropertyMetaGetTypeStr(FillUnderPos          )
    CQChartsPropertyMetaGetTypeStr(FillUnderSide         )
    CQChartsPropertyMetaGetTypeStr(Font                  )
    CQChartsPropertyMetaGetTypeStr(Image                 )
    CQChartsPropertyMetaGetTypeStr(KeyLocation           )
    CQChartsPropertyMetaGetTypeStr(KeyPressBehavior      )
    CQChartsPropertyMetaGetTypeStr(Length                )
    CQChartsPropertyMetaGetTypeStr(LineCap               )
    CQChartsPropertyMetaGetTypeStr(LineDash              )
    CQChartsPropertyMetaGetTypeStr(LineData              )
    CQChartsPropertyMetaGetTypeStr(LineJoin              )
    CQChartsPropertyMetaGetTypeStr(Margin                )
    CQChartsPropertyMetaGetTypeStr(ModelColumn           )
    CQChartsPropertyMetaGetTypeStr(ModelIndex            )
    CQChartsPropertyMetaGetTypeStr(ModelInd              )
    CQChartsPropertyMetaGetTypeStr(NamePair              )
    CQChartsPropertyMetaGetTypeStr(ObjRef                )
    CQChartsPropertyMetaGetTypeStr(ObjRefPos             )
    CQChartsPropertyMetaGetTypeStr(PaletteName           )
    CQChartsPropertyMetaGetTypeStr(Path                  )
    CQChartsPropertyMetaGetTypeStr(Points                )
    CQChartsPropertyMetaGetTypeStr(Polygon               )
    CQChartsPropertyMetaGetTypeStr(PolygonList           )
    CQChartsPropertyMetaGetTypeStr(Position              )
    CQChartsPropertyMetaGetTypeStr(Reals                 )
    CQChartsPropertyMetaGetTypeStr(Rect                  )
    CQChartsPropertyMetaGetTypeStr(ShapeData             )
    CQChartsPropertyMetaGetTypeStr(ShapeType             )
    CQChartsPropertyMetaGetTypeStr(Sides                 )
    CQChartsPropertyMetaGetTypeStr(StrokeData            )
    CQChartsPropertyMetaGetTypeStr(Style                 )
    CQChartsPropertyMetaGetTypeStr(Symbol                )
    CQChartsPropertyMetaGetTypeStr(SymbolData            )
    CQChartsPropertyMetaGetTypeStr(SymbolSizeMap         )
    CQChartsPropertyMetaGetTypeStr(SymbolType            )
    CQChartsPropertyMetaGetTypeStr(SymbolTypeMap         )
    CQChartsPropertyMetaGetTypeStr(TextBoxData           )
    CQChartsPropertyMetaGetTypeStr(TextData              )
    CQChartsPropertyMetaGetTypeStr(ThemeName             )
    CQChartsPropertyMetaGetTypeStr(TitleLocation         )
    CQChartsPropertyMetaGetTypeStr(Units                 )
    CQChartsPropertyMetaGetTypeStr(ValueList             )
    CQChartsPropertyMetaGetTypeStr(Widget                )

    CQChartsPropertyMetaGetGeomTypeStr(BBox )
    CQChartsPropertyMetaGetGeomTypeStr(Point)

    CQChartsPropertyMetaGetTypeStr(OptBool    )
    CQChartsPropertyMetaGetTypeStr(OptInt     )
    CQChartsPropertyMetaGetTypeStr(OptReal    )
    CQChartsPropertyMetaGetTypeStr(OptString  )
    CQChartsPropertyMetaGetTypeStr(OptLength  )
    CQChartsPropertyMetaGetTypeStr(OptPosition)
    CQChartsPropertyMetaGetTypeStr(OptRect    )

    return false;
  }

  bool setTypeStr(const QString &type, const QString &str) {
    CQChartsPropertyMetaSetTypeStr(Alpha                 )
    CQChartsPropertyMetaSetTypeStr(Angle                 )
    CQChartsPropertyMetaSetTypeStr(Area                  )
    CQChartsPropertyMetaSetTypeStr(ArrowData             )
    CQChartsPropertyMetaSetTypeStr(AxisSide              )
    CQChartsPropertyMetaSetTypeStr(AxisTickLabelPlacement)
    CQChartsPropertyMetaSetTypeStr(AxisValueType         )
    CQChartsPropertyMetaSetTypeStr(BoxData               )
    CQChartsPropertyMetaSetTypeStr(Color                 )
    CQChartsPropertyMetaSetTypeStr(ColorMap              )
    CQChartsPropertyMetaSetTypeStr(ColorStops            )
    CQChartsPropertyMetaSetTypeStr(Column                )
    CQChartsPropertyMetaSetTypeStr(Columns               )
    CQChartsPropertyMetaSetTypeStr(ColumnNum             )
    CQChartsPropertyMetaSetTypeStr(ConnectionList        )
    CQChartsPropertyMetaSetTypeStr(FillData              )
    CQChartsPropertyMetaSetTypeStr(FillPattern           )
    CQChartsPropertyMetaSetTypeStr(FillUnderPos          )
    CQChartsPropertyMetaSetTypeStr(FillUnderSide         )
    CQChartsPropertyMetaSetTypeStr(Font                  )
    CQChartsPropertyMetaSetTypeStr(Image                 )
    CQChartsPropertyMetaSetTypeStr(KeyLocation           )
    CQChartsPropertyMetaSetTypeStr(KeyPressBehavior      )
    CQChartsPropertyMetaSetTypeStr(Length                )
    CQChartsPropertyMetaSetTypeStr(LineCap               )
    CQChartsPropertyMetaSetTypeStr(LineDash              )
    CQChartsPropertyMetaSetTypeStr(LineData              )
    CQChartsPropertyMetaSetTypeStr(LineJoin              )
    CQChartsPropertyMetaSetTypeStr(Margin                )
    CQChartsPropertyMetaSetTypeStr(ModelColumn           )
    CQChartsPropertyMetaSetTypeStr(ModelIndex            )
    CQChartsPropertyMetaSetTypeStr(ModelInd              )
    CQChartsPropertyMetaSetTypeStr(NamePair              )
    CQChartsPropertyMetaSetTypeStr(ObjRef                )
    CQChartsPropertyMetaSetTypeStr(ObjRefPos             )
    CQChartsPropertyMetaSetTypeStr(PaletteName           )
    CQChartsPropertyMetaSetTypeStr(Path                  )
    CQChartsPropertyMetaSetTypeStr(Points                )
    CQChartsPropertyMetaSetTypeStr(Polygon               )
    CQChartsPropertyMetaSetTypeStr(PolygonList           )
    CQChartsPropertyMetaSetTypeStr(Position              )
    CQChartsPropertyMetaSetTypeStr(Reals                 )
    CQChartsPropertyMetaSetTypeStr(Rect                  )
    CQChartsPropertyMetaSetTypeStr(ShapeData             )
    CQChartsPropertyMetaSetTypeStr(ShapeType             )
    CQChartsPropertyMetaSetTypeStr(Sides                 )
    CQChartsPropertyMetaSetTypeStr(StrokeData            )
    CQChartsPropertyMetaSetTypeStr(Style                 )
    CQChartsPropertyMetaSetTypeStr(Symbol                )
    CQChartsPropertyMetaSetTypeStr(SymbolData            )
    CQChartsPropertyMetaSetTypeStr(SymbolSizeMap         )
    CQChartsPropertyMetaSetTypeStr(SymbolType            )
    CQChartsPropertyMetaSetTypeStr(SymbolTypeMap         )
    CQChartsPropertyMetaSetTypeStr(TextBoxData           )
    CQChartsPropertyMetaSetTypeStr(TextData              )
    CQChartsPropertyMetaSetTypeStr(ThemeName             )
    CQChartsPropertyMetaSetTypeStr(TitleLocation         )
    CQChartsPropertyMetaSetTypeStr(Units                 )
    CQChartsPropertyMetaSetTypeStr(ValueList             )
    CQChartsPropertyMetaSetTypeStr(Widget                )

    CQChartsPropertyMetaSetGeomTypeStr(BBox )
    CQChartsPropertyMetaSetGeomTypeStr(Point)

    CQChartsPropertyMetaSetTypeStr(OptBool    )
    CQChartsPropertyMetaSetTypeStr(OptInt     )
    CQChartsPropertyMetaSetTypeStr(OptReal    )
    CQChartsPropertyMetaSetTypeStr(OptString  )
    CQChartsPropertyMetaSetTypeStr(OptLength  )
    CQChartsPropertyMetaSetTypeStr(OptPosition)
    CQChartsPropertyMetaSetTypeStr(OptRect    )

    return false;
  }
};

#endif
