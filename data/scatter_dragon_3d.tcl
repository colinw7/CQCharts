set model [create_charts_fractal_model -type dragon3d]

set view [create_charts_view -3d]

set plot [create_charts_plot -view $view -model $model -type scatter3d \
  -columns {{x 0} {y 1} {z 2} {color 3}} -title "Scatter (Dragon 3D)"]
