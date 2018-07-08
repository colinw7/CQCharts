set model [load_model -csv data/multi_bar.csv -first_line_header]

set plot [create_plot -model $model -type barchart -columns "value=1 2 3"]

create_plot_dlg -model $model
