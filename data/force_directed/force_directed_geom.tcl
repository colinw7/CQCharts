set model [load_charts_model -csv data/force_directed_geom.csv -first_line_header]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{from 0} {to 1}}]
