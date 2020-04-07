set model1 [load_charts_model -csv data/join1.csv -first_line_header]
set model2 [load_charts_model -csv data/join2.csv -first_line_header]

set model3 [join_charts_model -models [list $model1 $model2] -columns {Id1 Id2}]

show_charts_manage_models_dlg
