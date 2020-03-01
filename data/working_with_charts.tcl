set age { 1 3 5 2 11 9 3 9 12 3 }
set weight { 4.4 5.3 7.2 5.2 8.5 7.3 6.0 10.4 10.2 6.1 }

set model [load_charts_model -tcl [list $age $weight]]

write_charts_model -model $model

get_charts_data -model $model -column 1 -name details.mean
get_charts_data -model $model -column 1 -name details.stdev

get_charts_data -model $model -column 0 -data 1 -name details.correlation
