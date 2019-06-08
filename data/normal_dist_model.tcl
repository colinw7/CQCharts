set model [load_charts_model -expr -num_rows 200 -name "male/female distribution"]

process_charts_model -model $model -add -expr "rnorm(55)" -header "F"
process_charts_model -model $model -add -expr "rnorm(58)" -header "M"

#show_charts_manage_models_dlg

set plot [create_charts_plot -model $model -type distribution -columns {{value {1 2}}}]
