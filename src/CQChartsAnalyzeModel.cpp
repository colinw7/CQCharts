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
  CQChartsModelDetails *details = modelData_->details();

  int nc = details->numColumns();
  int nr = details->numRows   ();

  CQChartsModelDetails::Columns numericColumns   = details->numericColumns();
  CQChartsModelDetails::Columns monotonicColumns = details->monotonicColumns();

  using ColumnUsed  = std::pair<CQChartsModelColumnDetails*,bool>;
  using IColumnUsed = std::map<int,ColumnUsed>;

  IColumnUsed columnUsed;

  for (int i = 0; i < nc; ++i) {
    CQChartsModelColumnDetails *columnDetails = details->columnDetails(i);

    columnUsed[i] = ColumnUsed(columnDetails, false);
  }

  CQCharts::PlotTypes types;

  charts_->getPlotTypes(types);

  for (const auto &type : types) {
    int numRequired  = 0;
    int numNumeric   = 0;
    int numMonotonic = 0;

    for (const auto &parameter : type->parameters()) {
      if (! parameter->isColumn())
        continue;

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
      continue;

    if (numNumeric > 0 && int(numericColumns.size()) < numNumeric)
      continue;

    if (numMonotonic > 0 && int(monotonicColumns.size()) < numMonotonic)
      continue;

    //---

    bool requiredValid = true;

    IColumnUsed columnUsed1 = columnUsed;

    NameColumns nameColumns;

    for (const auto &parameter : type->parameters()) {
      if (! parameter->isColumn())
        continue;

      const CQChartsPlotParameter::Attributes &attributes = parameter->attributes();

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
      continue;

    //---

    if (type->isGroupType()) {
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
  }
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
