set model [load_charts_model -expr -num_rows 10000]

process_charts_model -model $model -add -expr "rnorm(0.0, 0.5)" -header "x"
process_charts_model -model $model -add -expr "rnorm(3.0, 2.0)" -header "y"

set plot [create_charts_plot -model $model -type scatter -columns {{x 1} {y 2}}]
