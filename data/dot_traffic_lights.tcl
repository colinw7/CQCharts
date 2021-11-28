set model [load_charts_model -csv data/dot/dot_traffic_lights.csv -first_line_header]

set plot [create_charts_plot -model $model -type graph \
  -columns {{from 0} {to 1} {attributes 2} {group 3}} -title "dot traffic lights"]

set_charts_property -plot $plot -name node.scaleX -value 1
set_charts_property -plot $plot -name edge.scaled -value 0
