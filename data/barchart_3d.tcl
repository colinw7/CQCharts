set model [load_charts_model -csv data/multi_bar.csv -first_line_header]

set model1 [group_charts_model -model $model -columns {1 2 3 4 5 6 7}]

set plot [create_charts_plot -model $model1 -type scatter3d -columns {{x 0} {y 1} {z 2}}]
