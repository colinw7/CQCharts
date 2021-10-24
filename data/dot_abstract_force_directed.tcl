set model [load_charts_model -csv data/dot/dot_abstract.csv -first_line_header]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{from 0} {to 1} {attributes 2}} -title "sankey from/to"]
