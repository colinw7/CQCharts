# force directed link model
set model [load_charts_model -csv data/dot/dot_traffic_lights.csv -first_line_header]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{from 0} {to 1} {attributes 2}} -title "force directed from/to"]
