set model [load_model -csv data/bikes.csv -first_line_header]

set_charts_data -model $model -column 0 -name column_type -value "time:format=%d/%m/%Y"

#---

source describe_model.tcl

describe_model $model

echo ""

#---

source create_stat_model.tcl

set stat_model [create_stat_model $model]

export_model -model $stat_model -hheader 1 -vheader 1

echo ""

#---

# add data number to model
set day_col [process_model -model $model -add -expr {timeval(0,"%u")} -header Day]

#---

# fold on day number and flatten
set day_model [fold_model -model $model -column $day_col]

set mean_day_model [flatten_model -model $day_model -sum]

#---

# flatten directly on data number
set mean_day_model [flatten_model -model $model -group Day -sum]

manage_models_dlg
