#include <CQChartsAnalyzeModel.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>

#include <CQPerfMonitor.h>
#include <set>

CQChartsAnalyzeModel::
CQChartsAnalyzeModel(CQCharts *charts, CQChartsModelData *modelData) :
 charts_(charts), modelData_(modelData)
{
}

CQChartsAnalyzeModel::
~CQChartsAnalyzeModel()
{
  for (auto &tnc : typeAnalyzeModelData_)
    delete tnc.second;
}

void
CQChartsAnalyzeModel::
analyze()
{
  CQPerfTrace trace("CQChartsAnalyzeModel::analyze");

  CQCharts::PlotTypes types;

  charts_->getPlotTypes(types);

  for (const auto &type : types) {
    analyzeType(type);
  }
}

bool
CQChartsAnalyzeModel::
analyzeType(CQChartsPlotType *type)
{
  CQChartsAnalyzeModelData *analyzeModelData { nullptr };

  if (! analyzeType(type, analyzeModelData))
    return false;

  typeAnalyzeModelData_[type->name()] = analyzeModelData;

  return true;
}

bool
CQChartsAnalyzeModel::
analyzeType(CQChartsPlotType *type, CQChartsAnalyzeModelData* &analyzeModelData)
{
  CQPerfTrace trace("CQChartsAnalyzeModel::analyzeType");

  auto *details = (modelData_ ? modelData_->details() : nullptr);
  if (! details) return false;

  int nc = details->numColumns();
  int nr = details->numRows   ();

  //---

  using ColumnUsed  = std::pair<CQChartsModelColumnDetails*, bool>;
  using IColumnUsed = std::map<int, ColumnUsed>;

  IColumnUsed columnUsed;

  for (int i = 0; i < nc; ++i) {
    auto *columnDetails = details->columnDetails(CQChartsColumn(i));

    columnUsed[i] = ColumnUsed(columnDetails, false);
  }

  //---

  // get required column parameters for numeric/monotonic
  int numRequired  = 0;
  int numNumeric   = 0;
  int numMonotonic = 0;

  for (const auto &parameter : type->parameters()) {
    if (! parameter->isColumn())
      continue;

    //if (parameter->isMultiple())
    //  continue;

    const auto &attributes = parameter->attributes();

    if (! attributes.isRequired())
      continue;

    ++numRequired;

    if (attributes.isNumeric()) {
      ++numNumeric;

      if (attributes.isMonotonic())
        ++numMonotonic;
    }
  }

  // fail if not enough columns
  if (numRequired > nc)
    return false;

  //----

  // check if model has required number of numeric columns
  if (numNumeric > 0) {
    auto numericColumns = details->numericColumns();

    if (int(numericColumns.count()) < numNumeric)
      return false;
  }

  // check if model has required number of monotonic columns
  if (numMonotonic > 0) {
    auto monotonicColumns = details->monotonicColumns();

    if (int(monotonicColumns.count()) < numMonotonic)
      return false;
  }

  //---

  analyzeModelData = new CQChartsAnalyzeModelData;

  auto &parameterNameColumn = analyzeModelData->parameterNameColumn;

  // check for grouped columns
  using NameSet = std::set<QString>;

  bool grouped = false;

  NameSet requiredInvalid;

  auto columnUsed1 = columnUsed;

  for (const auto &parameter : type->parameters()) {
    if (! parameter->isColumn())
      continue;

    //if (parameter->isMultiple())
    //  continue;

    const auto &attributes = parameter->attributes();

    if (parameter->isGroupable()) {
      bool found = false;

      // find first valid unused column for attribute
      for (auto &cu : columnUsed1) {
        if (cu.second.second)
          continue;

        auto *columnDetails = cu.second.first;

        if (! columnDetails->isKey())
          continue;

        parameterNameColumn[parameter->name()] = columnDetails->column();

        cu.second.second = true;

        found   = true;
        grouped = true;

        break;
      }

      if (found)
        continue;
    }

    //---

    if (! attributes.isRequired()) {
      // if attribute is a discriminator then assign if exact match
      if (attributes.isDiscriminator()) {
        // find first valid unused column for attribute
        for (auto &cu : columnUsed1) {
          if (cu.second.second)
            continue;

          auto *columnDetails = cu.second.first;

          if (! type->isColumnForParameter(columnDetails, parameter))
            continue;

          parameterNameColumn[parameter->name()] = columnDetails->column();

          cu.second.second = true;

          break;
        }
      }
    }
    else {
      bool found = false;

      // find first valid unused column for attribute
      for (auto &cu : columnUsed1) {
        if (cu.second.second)
          continue;

        auto *columnDetails = cu.second.first;

        if (attributes.isMonotonic()) {
          if (! columnDetails->isMonotonic())
            continue;
        }

        if (attributes.isNumeric()) {
          if (! columnDetails->isNumeric())
            continue;
        }

        if (attributes.isUnique()) {
          int numUnique = columnDetails->numUnique();

          if (numUnique == 1 || numUnique >= nr)
            continue;
        }

        if (! type->isColumnForParameter(columnDetails, parameter))
          continue;

        parameterNameColumn[parameter->name()] = columnDetails->column();

        cu.second.second = true;

        found = true;

        break;
      }

      if (! found)
        requiredInvalid.insert(parameter->name());
    }
  }

  // fail if can't find column for required
  // (after check if type can set required columns)
  if (! requiredInvalid.empty()) {
    type->analyzeModel(modelData_, *analyzeModelData);

    bool valid = true;

    for (const auto &name : requiredInvalid) {
      if (analyzeModelData->parameterNameColumn.find(name) ==
            analyzeModelData->parameterNameColumn.end()) {
        valid = false;
        break;
      }
    }

    if (! valid) {
      delete analyzeModelData;
      return false;
    }
  }

  //---

  // if group type and group column not found
  if (type->isGroupType() & ! grouped) {
    int            bestNumUnique = -1;
    CQChartsColumn bestColumn;
    int            bestI = -1;

    for (auto &cu : columnUsed1) {
      // ignore already used
      if (cu.second.second)
        continue;

      //---

      // must be string or integer column
      auto *columnDetails = cu.second.first;

      if (columnDetails->type() != CQBaseModelType::STRING &&
          columnDetails->type() != CQBaseModelType::INTEGER)
        continue;

      //---

      // check can create multiple groups for column
      int numUnique = columnDetails->numUnique();

      if (numUnique == 1 || numUnique >= nr)
        continue;

      //---

      // update best unique
      if (bestNumUnique < 0 || numUnique < bestNumUnique) {
        bestColumn = columnDetails->column();

        bestNumUnique = numUnique;
        bestI         = cu.first;
      }
    }

    // set to best unique
    if (bestNumUnique >= 0) {
      parameterNameColumn["group"] = bestColumn;

      columnUsed1[bestI].second = true;
    }
  }

  //----

  type->analyzeModel(modelData_, *analyzeModelData);

  return true;
}

const CQChartsAnalyzeModelData &
CQChartsAnalyzeModel::
analyzeModelData(const CQChartsPlotType *type)
{
  return *typeAnalyzeModelData_[type->name()];
}

void
CQChartsAnalyzeModel::
print(std::ostream &os) const
{
  for (const auto &tnc : typeAnalyzeModelData_) {
    const auto &typeName = tnc.first;

    os << typeName.toStdString() << "\n";

    auto *analyzeModelData = tnc.second;

    for (const auto &nc : analyzeModelData->parameterNameColumn) {
      const auto &name   = nc.first;
      const auto &column = nc.second;

      os << " " << name.toStdString() << "=" << column.column();
    }

    os << "\n";
  }
}

void
CQChartsAnalyzeModel::
initPlot(CQChartsPlot *plot)
{
  auto *type = plot->type();
  assert(type);

  analyzeType(type);

  const auto &analyzeModelData = this->analyzeModelData(type);

  for (const auto &nc : analyzeModelData.parameterNameColumn) {
    const auto &paramName = nc.first;
    const auto &column    = nc.second;

    const auto *param = type->getParameter(paramName);

    auto propPath = param->propPath();

    if (! propPath.length())
      std::cerr << "No property path for parameter '" << paramName.toStdString() <<
                   "' for '" << type->name().toStdString() << "'\n";

    if (! plot->setProperty(propPath, column.toString()))
      std::cerr << "Failed to set property path '" << propPath.toStdString() <<
                   "' for parameter '" << paramName.toStdString() <<
                   "' for '" << type->name().toStdString() <<
                   "' to '" << column.toString().toStdString() << "'\n";
  }
}
