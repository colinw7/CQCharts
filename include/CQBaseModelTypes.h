#ifndef CQBaseModelTypes_H
#define CQBaseModelTypes_H

#include <QAbstractItemModel>

//! model input data types
enum class CQBaseModelDataType {
  NONE,
  CSV,
  TSV,
  XML,
  JSON,
  GNUPLOT
};

//! model roles supported by CQBaseModel
enum class CQBaseModelRole {
  Type              = Qt::UserRole + 1, //!< column type role
  BaseType          = Type + 1,         //!< column base type role (calculated)
  TypeValues        = Type + 2,         //!< column type values role
  Min               = Type + 3,         //!< column user min role
  Max               = Type + 4,         //!< column user max role
  Sorted            = Type + 5,         //!< sorted role
  SortOrder         = Type + 6,         //!< sort order role
  Title             = Type + 7,         //!< sort order role
  Key               = Type + 8,         //!< title role
  RawValue          = Type + 9,         //!< raw value by role
  IntermediateValue = Type + 10,        //!< intermediate value role
  CachedValue       = Type + 11,        //!< cached value role
  OutputValue       = Type + 12,        //!< output value role
  Group             = Type + 13,        //!< group role
  Format            = Type + 14,        //!< output format
  DataMin           = Type + 15,        //!< calculated data min
  DataMax           = Type + 16         //!< calculated data max
};

//! model value types supported by CQBaseModel
//! (use variant numbers where possible)
enum class CQBaseModelType {
  NONE    = QVariant::Invalid,
  BOOLEAN = QVariant::Bool,
  INTEGER = QVariant::Int,
  REAL    = QVariant::Double,
  STRING  = QVariant::String,
  STRINGS = QVariant::StringList,
  POINT   = QVariant::PointF,
  LINE    = QVariant::LineF,
  RECT    = QVariant::RectF,
  SIZE    = QVariant::SizeF,
#if QT_VERSION >= 0x050000
  POLYGON = QVariant::PolygonF,
#else
  POLYGON = QVariant::UserType + 199,
#endif
  COLOR   = QVariant::Color,
  PEN     = QVariant::Pen,
  BRUSH   = QVariant::Brush,
  IMAGE   = QVariant::Image,
  TIME    = QVariant::Time,

  SYMBOL          = QVariant::UserType + 101,
  SYMBOL_SIZE     = QVariant::UserType + 102,
  FONT_SIZE       = QVariant::UserType + 103,
  PATH            = QVariant::UserType + 104,
  STYLE           = QVariant::UserType + 105,
  POLYGON_LIST    = QVariant::UserType + 106,
  CONNECTION_LIST = QVariant::UserType + 107,
  NAME_PAIR       = QVariant::UserType + 108,
  COLUMN          = QVariant::UserType + 109,
  COLUMN_LIST     = QVariant::UserType + 110,
  ENUM            = QVariant::UserType + 111
};

#endif
