set model [load_charts_model -csv data/diamonds.csv -first_line_header]

set view [create_charts_view -3d]

set plot [create_charts_plot -view $view -type scatter3d \
  -columns {{x depth} {y carat} {z price}} -title "Scatter Plot"]
