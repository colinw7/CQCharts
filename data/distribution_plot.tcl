set model [load_model -csv data/name_value.csv -first_line_header]

set plot [create_plot -type distribution -model $model -columns "group=0,value=1"]

set_property -plot $plot -name dataGrouping.bucket.exact -value 0

create_plot_dlg -model $model
