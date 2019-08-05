#ifndef CQChartsInitData_H
#define CQChartsInitData_H

#include <CQChartsFileType.h>
#include <CQChartsInputData.h>
#include <CQChartsNameValueData.h>

#include <boost/optional.hpp>

/*!
 * \brief Charts Initialization Data
 * \ingroup Charts
 */
struct CQChartsInitData {
  using FileNames = std::vector<QString>;
  using OptReal   = boost::optional<double>;

  FileNames             filenames;
  CQChartsFileType      fileType        { CQChartsFileType::NONE };
  CQChartsInputData     inputData;
  QString               typeName;
  QString               filterStr;
  QString               process;
  QString               processAdd;
  CQChartsNameValueData nameValueData;
  QString               columnType;
  bool                  xintegral       { false };
  bool                  yintegral       { false };
  bool                  xlog            { false };
  bool                  ylog            { false };
  QString               title;
  QString               viewTitle;
  QString               viewProperties;
  QString               plotProperties;
  OptReal               xmin, ymin, xmax, ymax;
  bool                  x1x2            { false };
  bool                  y1y2            { false };
  bool                  overlay         { false };
  int                   nr              { 1 };
  int                   nc              { 1 };
  double                dx              { 1000.0 };
  double                dy              { 1000.0 };

  QString nameValue(const QString &name) const {
    auto p = nameValueData.values.find(name);

    return (p != nameValueData.values.end() ? (*p).second : QString());
  }

  void setNameValue(const QString &name, const QString &value) {
    nameValueData.values[name] = value;
  }

  QString nameString(const QString &name) const {
    auto p = nameValueData.strings.find(name);

    return (p != nameValueData.strings.end() ? (*p).second : "");
  }

  void setNameString(const QString &name, const QString &value) {
    nameValueData.strings[name] = value;
  }

  double nameReal(const QString &name) const {
    auto p = nameValueData.reals.find(name);

    return (p != nameValueData.reals.end() ? (*p).second : 0.0);
  }

  void setNameReal(const QString &name, double value) {
    nameValueData.reals[name] = value;
  }

  bool nameBool(const QString &name) const {
    auto p = nameValueData.bools.find(name);

    return (p != nameValueData.bools.end() ? (*p).second : false);
  }

  void setNameBool(const QString &name, bool value) {
    nameValueData.bools[name] = value;
  }
};

#endif
