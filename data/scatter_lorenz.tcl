set model [create_charts_lorenz_model]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1}} -title "Scatter (Lorenz)"]
