set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot [create_charts_plot -model $model -type scatter3d \
  -columns {{x petalLength} {y sepalLength} {z @GROUP} {group species}} \
  -title "Scatter Group"]
