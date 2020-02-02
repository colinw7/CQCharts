set model1 [load_charts_model -csv data/ages.csv       -first_line_header]
set model2 [load_charts_model -csv data/aster_data.csv -first_line_header]
set model3 [load_charts_model -csv data/boxplot.csv    -first_line_header]

show_charts_create_plot_dlg -model $model1
