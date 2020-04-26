set model [load_charts_model -expr -num_rows 10000]

process_charts_model -model $model -add -expr "rnorm(0.0, 0.5)" -header "x"
process_charts_model -model $model -add -expr "rnorm(1.0, 1.0)" -header "y"
process_charts_model -model $model -add -expr "rnorm(2.0, 2.0)" -header "z"

set plot [create_charts_plot -model $model -type scatter3d -columns {{x 1} {y 2} { z 3}}]
