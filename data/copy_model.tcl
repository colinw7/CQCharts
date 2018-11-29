set model [load_model -csv data/HairEyeColor.csv -first_line_header]

filter_model -model $model  -expr {$Sex=="Male"}

set model1 [copy_model -model $model]

filter_model -model $model  -expr {$Sex=="Female"}

set model2 [copy_model -model $model]

manage_model_dlg
