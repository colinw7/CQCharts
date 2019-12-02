#ifndef CQChartsTextOptions_H
#define CQChartsTextOptions_H

#include <QString>

struct CQChartsTextOptions {
  enum ValueType {
    ANGLE     = (1<<0),
    ALIGN     = (1<<1),
    CONTRAST  = (1<<2),
    FORMATTED = (1<<3),
    SCALED    = (1<<4),
    HTML      = (1<<5),

    NONE   = 0,
    SIMPLE = (ANGLE | CONTRAST | HTML),
    ALL    = (ANGLE | ALIGN | CONTRAST | FORMATTED | SCALED | HTML)
  };

  double        angle            { 0.0 };   //!< text angle
  Qt::Alignment align            { Qt::AlignHCenter | Qt::AlignVCenter }; //!< text align
  bool          contrast         { false }; //!< text contrast
  double        contrastAlpha    { 0.5 };   //!< text contrast alpha
  bool          formatted        { false }; //!< text formatted
  bool          scaled           { false }; //!< text scaled
  double        scale            { -1.0 };  //!< text fixed scale
  bool          html             { false }; //!< text html
  bool          clipped          { true };  //!< text clipped
  int           margin           { 0 };     //!< text margin (pixels)
  double        minScaleFontSize { 6.0 };   //!< min scaled font size
  double        maxScaleFontSize { 48.0 };  //!< max scaled font size

  CQChartsTextOptions() = default;
};

#endif
