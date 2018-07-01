set model1 [load_model -csv data/group_column.csv -first_line_header]
set model2 [load_model -csv data/group_bucket.csv -first_line_header]

create_plot_dlg -model $model2
