# Name,Party,Province,Age,Gender
set model1 [load_charts_model -csv data/mps.csv -first_line_header]

#set model2 [create_charts_pivot_model -model $model1 -hcolumns 4 -vcolumns 2 -dcolumn 3]
#set model3 [create_charts_pivot_model -model $model1 -hcolumns 4 -dcolumn 3]
set model4 [create_charts_pivot_model -model $model1 -hcolumns 2 -vcolumns 1 -dcolumn 3]

show_charts_manage_models_dlg
