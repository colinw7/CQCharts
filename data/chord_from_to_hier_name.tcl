# Chord From/To
set model [load_charts_model -csv data/from_to_depth.csv -first_line_header]

set plot [create_charts_plot -model $model -type chord \
  -columns {{from 0} {to 1} {value 2}} -title "chord from/to"]

set_charts_property -plot $plot -name options.hierName -value 1
