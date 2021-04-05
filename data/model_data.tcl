set model [load_charts_model -csv data/ToothGrowth.csv -first_line_header]

set nr [get_charts_data -model $model -name num_rows]

set mean [get_charts_data -model $model -name details.mean]
echo $mean

set row [get_charts_data -model $model -name header]
set row [get_charts_data -model $model -name row -row 1]
set col [get_charts_data -model $model -name column -column 1]

set nu [get_charts_data -model $model -name details.num_unique]
set uv [get_charts_data -model $model -name details.unique_values]
echo $nu
echo $uv

set vr [process_charts_model -model $model -query -expr {$supp == "VC"}]
