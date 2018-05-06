#ifndef CQChartsTextOptions_H
#define CQChartsTextOptions_H

struct CQChartsTextOptions {
  double        angle            { 0.0 };
  bool          contrast         { false };
  bool          formatted        { false };
  bool          scaled           { false };
  bool          clipped          { true };
  Qt::Alignment align            { Qt::AlignHCenter | Qt::AlignVCenter };
  double        minScaleFontSize { 6.0 };  // min scaled font size
  double        maxScaleFontSize { 48.0 }; // max scaled font size

  CQChartsTextOptions() = default;
};

#endif
