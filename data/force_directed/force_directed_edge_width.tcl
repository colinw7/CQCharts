# force directed link model
set model [load_charts_model -csv data/force_directed_edge_width.csv -first_line_header]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{from 0} {to 1} {value 2} {edgeWidth 3} {attributes 4}} \
  -title "force directed edge width"]
