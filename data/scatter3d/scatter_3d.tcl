set model [load_charts_model -data data/glass.dat]

set plot [create_charts_plot -model $model -type scatter3d \
  -columns {{x 0} {y 1} {z 2}} -title "Scatter Plot"]
