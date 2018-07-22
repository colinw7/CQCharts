set model [load_model -expr -num_rows 200 -name "male/female distribution"]

process_model -model $model -add -expr "rnorm(55)" -header "F"
process_model -model $model -add -expr "rnorm(58)" -header "M"

#manage_model_dlg

set plot [create_plot -model $model -type distribution -columns "value=1 2"]
