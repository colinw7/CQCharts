#include <CQChartsAnalyzeModel.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsPlotType.h>
#include <CQCharts.h>

CQChartsAnalyzeModel::
CQChartsAnalyzeModel(CQCharts *charts, CQChartsModelData *modelData) :
 charts_(charts), modelData_(modelData)
{
}

void
CQChartsAnalyzeModel::
analyze()
{
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
  CQChartsModelDetails *details = modelData_->details();

  int nc = details->numColumns();
  int nr = details->numRows   ();

  CQChartsColumns numericColumns   = details->numericColumns();
  CQChartsColumns monotonicColumns = details->monotonicColumns();

  //---

  using ColumnUsed  = std::pair<CQChartsModelColumnDetails*,bool>;
  using IColumnUsed = std::map<int,ColumnUsed>;

  IColumnUsed columnUsed;

  for (int i = 0; i < nc; ++i) {
    CQChartsModelColumnDetails *columnDetails = details->columnDetails(i);

    columnUsed[i] = ColumnUsed(columnDetails, false);
  }

  //---

  int numRequired  = 0;
  int numNumeric   = 0;
  int numMonotonic = 0;

  for (const auto &parameter : type->parameters()) {
    if (! parameter->isColumn())
      continue;

    //if (parameter->isMultiple())
    //  continue;

    const CQChartsPlotParameter::Attributes &attributes = parameter->attributes();

    if (! attributes.isRequired())
      continue;

    ++numRequired;

    if (attributes.isNumeric()) {
      ++numNumeric;

      if (attributes.isMonotonic())
        ++numMonotonic;
    }
  }

  if (numRequired > nc)
    return false;

  if (numNumeric > 0 && int(numericColumns.count()) < numNumeric)
    return false;

  if (numMonotonic > 0 && int(monotonicColumns.count()) < numMonotonic)
    return false;

  //---

  bool grouped       = false;
  bool requiredValid = true;

  IColumnUsed columnUsed1 = columnUsed;

  NameColumns nameColumns;

  for (const auto &parameter : type->parameters()) {
    if (! parameter->isColumn())
      continue;

    //if (parameter->isMultiple())
    //  continue;

    const CQChartsPlotParameter::Attributes &attributes = parameter->attributes();

    if (parameter->isGroupable()) {
      // find first valid unused column for attribute
      for (auto &cu : columnUsed1) {
        if (cu.second.second)
          continue;

        CQChartsModelColumnDetails *columnDetails = cu.second.first;

        if (! columnDetails->isKey())
          continue;

        nameColumns[parameter->name()] = columnDetails->column();

        cu.second.second = true;

        grouped = true;

        break;
      }
    }

    if (! attributes.isRequired()) {
      // if attribute is a discrimator then assign if exact match
      if (attributes.isDiscrimator()) {
        // find first valid unused column for attribute
        for (auto &cu : columnUsed1) {
          if (cu.second.second)
            continue;

          CQChartsModelColumnDetails *columnDetails = cu.second.first;

          if (! type->isColumnForParameter(columnDetails, parameter))
            continue;

          nameColumns[parameter->name()] = columnDetails->column();

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

        CQChartsModelColumnDetails *columnDetails = cu.second.first;

        if      (attributes.isMonotonic()) {
          if (! columnDetails->isMonotonic())
            continue;
        }
        else if (attributes.isNumeric()) {
          if (! columnDetails->isNumeric())
            continue;
        }

        if (! type->isColumnForParameter(columnDetails, parameter))
          continue;

        nameColumns[parameter->name()] = columnDetails->column();

        cu.second.second = true;

        found = true;

        break;
      }

      if (! found) {
        requiredValid = false;
        break;
      }
    }
  }

  if (! requiredValid)
    return false;

  //---

  if (type->isGroupType() & ! grouped) {
    int            bestNumUnique = -1;
    CQChartsColumn bestColumn;
    int            bestI = -1;

    for (auto &cu : columnUsed1) {
      if (cu.second.second)
        continue;

      CQChartsModelColumnDetails *columnDetails = cu.second.first;

      if (columnDetails->type() != CQBaseModel::Type::STRING &&
          columnDetails->type() != CQBaseModel::Type::INTEGER)
        continue;

      int numUnique = columnDetails->numUnique();

      if (numUnique == 1 || numUnique >= nr)
        continue;

      if (bestNumUnique < 0 || numUnique < bestNumUnique) {
        bestColumn = columnDetails->column();

        bestNumUnique = numUnique;
        bestI         = cu.first;
      }
    }

    if (bestNumUnique >= 0) {
      nameColumns["group"] = bestColumn;

      columnUsed1[bestI].second = true;
    }
  }

  //---

  typeNameColumns_[type->name()] = nameColumns;

  return true;
}

void
CQChartsAnalyzeModel::
print() const
{
  for (const auto &tnc : typeNameColumns_) {
    const QString &typeName = tnc.first;

    std::cerr << typeName.toStdString() << "\n";

    const NameColumns &nameColumns = tnc.second;

    for (const auto &nc : nameColumns) {
      const QString        &name   = nc.first;
      const CQChartsColumn &column = nc.second;

      std::cerr << " " << name.toStdString() << "=" << column.column();
    }

    std::cerr << "\n";
  }
}
