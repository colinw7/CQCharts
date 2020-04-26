set model [load_charts_model -csv data/diamonds.csv -first_line_header]

set plot [create_charts_plot -type scatter3d -columns {{x x} {y y} {z z}} -title "Scatter Plot"]
