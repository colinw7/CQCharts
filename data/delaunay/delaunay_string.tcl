set model [load_charts_model -csv data/delaunay_string.csv -comment_header]

set plot [create_charts_plot -model $model -type delaunay -columns {{x 0} {y 1} {name 2}} \
  -title delaunay]
