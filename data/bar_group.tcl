set model [load_charts_model -csv data/multi_bar.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart -columns {{name 0} {values {1 2 3}}}]

#show_charts_create_plot_dlg -model $model
