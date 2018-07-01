set model [load_model -csv data/name_value.csv -first_line_header]

create_plot_dlg -model $model
