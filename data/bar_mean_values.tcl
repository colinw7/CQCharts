set model [load_charts_model -csv data/bar_values.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart -columns {{group 0} {values 1}}]

set_charts_property -plot $plot -name options.plotType  -value STACKED
set_charts_property -plot $plot -name options.valueType -value MEAN
