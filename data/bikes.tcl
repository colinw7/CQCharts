# load model

set model [load_charts_model -csv data/bikes.csv -first_line_header]

set_charts_data -model $model -column 0 -name column_type -value {{time} {format %d/%m/%Y}}

#---

source describe_model.tcl

echo "--- Model Details ---"

describe_model $model

echo ""

#---

# create statistics model
source create_stat_model.tcl

set stat_model [create_stat_model $model]

echo "--- Model Statistics ---"

export_charts_model -model $stat_model -hheader 1 -vheader 1

echo ""

#---

# add day number to model
set day_col [process_charts_model -model $model -add -expr {timeval(0,"%u")} -header Day]

#---

# fold on day number and flatten
set day_model [fold_charts_model -model $model -column $day_col]

set mean_day_model [flatten_charts_model -model $day_model -sum]

#---

# flatten directly on day number
set mean_day_model [flatten_charts_model -model $model -group Day -sum]

show_charts_manage_models_dlg
