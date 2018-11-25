set model [load_model -csv data/ToothGrowth.csv -first_line_header]

set model1 [copy_model -model $model]

manage_model_dlg
