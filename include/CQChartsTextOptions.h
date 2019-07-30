#ifndef CQChartsTextOptions_H
#define CQChartsTextOptions_H

#include <QString>

struct CQChartsTextOptions {
  double        angle            { 0.0 };   //!< text angle
  bool          contrast         { false }; //!< text contrast
  bool          formatted        { false }; //!< text formatted
  bool          scaled           { false }; //!< text scaled
  bool          html             { false }; //!< text html
  bool          clipped          { true };  //!< text clipped
  int           margin           { 0 };     //!< text margin (pixels)
  Qt::Alignment align            { Qt::AlignHCenter | Qt::AlignVCenter }; //!< text align
  double        minScaleFontSize { 6.0 };   //!< min scaled font size
  double        maxScaleFontSize { 48.0 };  //!< max scaled font size

  CQChartsTextOptions() = default;
};

#endif
