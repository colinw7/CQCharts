#ifndef CQChartsTextOptions_H
#define CQChartsTextOptions_H

#include <CQChartsAngle.h>
#include <CQChartsAlpha.h>
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

  CQChartsAngle angle;                                                  //!< angle
  Qt::Alignment align            { Qt::AlignHCenter|Qt::AlignVCenter }; //!< align
  bool          contrast         { false };                             //!< contrast
  CQChartsAlpha contrastAlpha    { 0.5 };                               //!< contrast alpha
  bool          formatted        { false };                             //!< formatted
  QString       formatSeps;                                             //!< format separators
  bool          scaled           { false };                             //!< scaled
  double        scale            { -1.0 };                              //!< fixed scale
  bool          html             { false };                             //!< html
  bool          clipped          { true };                              //!< clipped
  int           margin           { 0 };                                 //!< margin (pixels)
  double        minScaleFontSize { 6.0 };                               //!< min scaled font size
  double        maxScaleFontSize { 48.0 };                              //!< max scaled font size

  CQChartsTextOptions() = default;
};

#endif
