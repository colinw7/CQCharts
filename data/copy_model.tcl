set model [load_charts_model -csv data/HairEyeColor.csv -first_line_header]

filter_charts_model -model $model  -expr {$Sex=="Male"}

set model1 [copy_model -model $model]

filter_charts_model -model $model  -expr {$Sex=="Female"}

set model2 [copy_model -model $model]

show_charts_manage_models_dlg
