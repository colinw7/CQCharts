#include <CQChartsLayer.h>

const char *
CQChartsLayer::
typeName(const Type &type)
{
  switch (type) {
    case Type::BACKGROUND : return "background";
    case Type::BG_AXES    : return "bg_axes";
    case Type::BG_KEY     : return "bg_key";
    case Type::BG_PLOT    : return "bg_plot";
    case Type::MID_PLOT   : return "mid_plot";
    case Type::FG_PLOT    : return "fg_plot";
    case Type::FG_AXES    : return "fg_axes";
    case Type::FG_KEY     : return "fg_key";
    case Type::TITLE      : return "title";
    case Type::ANNOTATION : return "annotation";
    case Type::FOREGROUND : return "foreground";
    case Type::EDIT_HANDLE: return "edit_handle";
    case Type::BOXES      : return "boxes";
    case Type::SELECTION  : return "selection";
    case Type::MOUSE_OVER : return "mouse_over";
    default               : return "none";
  }
}

CQChartsLayer::Type
CQChartsLayer::
nameType(const QString &name)
{
  if      (name == "background" ) return Type::BACKGROUND;
  else if (name == "bg_axes"    ) return Type::BG_AXES;
  else if (name == "bg_key"     ) return Type::BG_KEY;
  else if (name == "bg_plot"    ) return Type::BG_PLOT;
  else if (name == "mid_plot"   ) return Type::MID_PLOT;
  else if (name == "fg_plot"    ) return Type::FG_PLOT;
  else if (name == "fg_axes"    ) return Type::FG_AXES;
  else if (name == "fg_key"     ) return Type::FG_KEY;
  else if (name == "title"      ) return Type::TITLE;
  else if (name == "annotation" ) return Type::ANNOTATION;
  else if (name == "foreground" ) return Type::FOREGROUND;
  else if (name == "edit_handle") return Type::EDIT_HANDLE;
  else if (name == "boxes"      ) return Type::BOXES;
  else if (name == "selection"  ) return Type::SELECTION;
  else if (name == "mouse_over" ) return Type::MOUSE_OVER;
  else                            return Type::NONE;
}

//---

CQChartsLayer::
CQChartsLayer(const Type &type, const CQChartsBuffer::Type &buffer) :
 type_(type), buffer_(buffer)
{
}

CQChartsLayer::
~CQChartsLayer()
{
}
