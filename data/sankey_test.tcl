set model [load_charts_model -csv data/sankey_test.csv -first_line_header]

set plot [create_charts_plot -model $model -type sankey \
  -columns {{from 0} {to 1} {depth 2}} -title "sankey from/to"]

set_charts_property -plot $plot -name placement.align  -value LARGEST
set_charts_property -plot $plot -name placement.spread -value NONE
