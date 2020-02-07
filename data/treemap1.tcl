set model [load_charts_model -csv data/treemap.csv -first_line_header]

process_charts_model -model $model -add -expr {concat(column("Owner"),{/},column("Path"))} \
  -header "Owner/Path"

set plot [create_charts_plot -model $model -type treemap -columns {{name 3} {value 1} {id 0}}]

set_charts_property -plot $plot -name "header.depth" -value 1
