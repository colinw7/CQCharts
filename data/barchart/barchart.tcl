set model [load_charts_model -csv data/multi_bar.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart -columns {{name 0} {values 1}}]

set_charts_property -plot $plot -name title.text.string -value "Population by State"
set_charts_property -plot $plot -name title.subtitle.string -value "Under 5 years"

set objs [get_charts_data -plot $plot -name objects -sync]

assert {[llength $objs] == 6}
