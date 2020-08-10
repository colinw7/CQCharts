set model [load_charts_model -csv data/contour.csv]

set plot [create_charts_plot -model $model -type contour]

#show_charts_manage_models_dlg
