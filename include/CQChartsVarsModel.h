#ifndef CQChartsVarsModel_H
#define CQChartsVarsModel_H

#include <CQDataModel.h>

/*!
 * \brief Wrapper class for CQDataModel to remember tcl variable data
 * \ingroup Charts
 */
class CQChartsVarsModel : public CQDataModel {
 public:
  CQChartsVarsModel(int numCols, int numRows);

  const QStringList &varNames() const { return varNames_; }
  void setVarNames(const QStringList &v) { varNames_ = v; }

  bool isTranspose() const { return transpose_; }
  void setTranspose(bool b) { transpose_ = b; }

  bool isFirstLineHeader() const { return firstLineHeader_; }
  void setFirstLineHeader(bool b) { firstLineHeader_ = b; }

  bool isFirstColumnHeader() const { return firstColumnHeader_; }
  void setFirstColumnHeader(bool b) { firstColumnHeader_ = b; }

 private:
  QStringList varNames_;                    //!< input variable names
  bool        transpose_         { false }; //!< first line header
  bool        firstColumnHeader_ { false }; //!< first column header
  bool        firstLineHeader_   { false }; //!< first line header
};

#endif
