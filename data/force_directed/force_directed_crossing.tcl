# force directed link model
set model [load_charts_model -csv data/force_directed_crossing.csv -first_line_header]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{from 0} {to 1} {value 2}} -title "force directed crossings"]

set_charts_property -plot $plot -name edge.shapeType -value ARC
