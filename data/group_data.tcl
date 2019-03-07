#set model1 [load_charts_model -csv data/group_column.csv -first_line_header]
#set model2 [load_charts_model -csv data/group_bucket.csv -first_line_header]
set model3 [load_charts_model -csv data/group_data.csv -comment_header]

show_charts_create_plot_dlg -model $model3
