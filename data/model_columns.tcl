set model [load_charts_model -csv data/googleplaystore.csv -first_line_header \
  -columns [list App Price]]

show_charts_manage_models_dlg
