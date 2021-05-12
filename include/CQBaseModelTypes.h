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
  GNUPLOT,
  PIVOT
};

//! model roles supported by CQBaseModel
enum class CQBaseModelRole {
  Type              = Qt::UserRole + 1, //!< column value's type
  BaseType          = Type + 1,         //!< column value's base type (calculated)
  TypeValues        = Type + 2,         //!< column value's type values
  Min               = Type + 3,         //!< column user value min
  Max               = Type + 4,         //!< column user value max
  Sorted            = Type + 5,         //!< column is sorted
  SortOrder         = Type + 6,         //!< column sort order
  Title             = Type + 7,         //!< column custom title (used ?)
  Tip               = Type + 8,         //!< column custom tip
  Key               = Type + 9,         //!< column is key (unique)
  RawValue          = Type + 10,        //!< cell raw value
  IntermediateValue = Type + 11,        //!< cell intermediate value
  CachedValue       = Type + 12,        //!< cell cached value
  OutputValue       = Type + 13,        //!< cell output value
  Group             = Type + 14,        //!< cell group value
  Format            = Type + 15,        //!< input/output format (used ?)
  IFormat           = Type + 16,        //!< input format (used ?)
  OFormat           = Type + 17,        //!< output format (used ?)
  DataMin           = Type + 18,        //!< column calculated value min
  DataMax           = Type + 19,        //!< column calculated value max
  HeaderType        = Type + 20,        //!< column header type
  HeaderTypeValues  = Type + 21         //!< column header type values
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
  FONT    = QVariant::Font,
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
  ENUM            = QVariant::UserType + 111,
  LENGTH          = QVariant::UserType + 112
};

#endif
