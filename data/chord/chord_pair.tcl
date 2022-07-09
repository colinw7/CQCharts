# link column model
set model [load_charts_model -csv data/adjacency.csv -comment_header]

set plot [create_charts_plot -model $model -type chord \
  -columns {{link 0} {value 1} {group 2}} -title "chord"]
