# Sankey From/To
set model [load_charts_model -csv data/dot_fsm_1.csv -first_line_header]

set plot [create_charts_plot -model $model -type sankey \
  -columns {{from 0} {to 1} {attributes 2}} -title "sankey from/to"]
