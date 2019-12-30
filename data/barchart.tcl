set model [load_charts_model -csv data/multi_bar.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart -columns {{name 0} {value 1}}]

set objs [get_charts_data -plot $plot -name objects -sync]

assert {[llength $objs] == 6}
