set model [load_charts_model -tsv data/digits_1.tsv -first_line_header]

set plot [create_charts_plot -type distribution -model $model -columns {{group 0} {value 1}}]

show_charts_create_plot_dlg -model $model
