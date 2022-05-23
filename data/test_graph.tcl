set model [load_charts_model -csv data/test_graph.csv -first_line_header]

set plot [create_charts_plot -model $model -type graph -columns {{from 0} {to 1} {value 2}}]

set_charts_property -plot $plot -name options.orientation -value Horizontal
