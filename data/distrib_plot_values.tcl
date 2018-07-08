set model [load_model -tsv data/digits_1.tsv -first_line_header]

set plot [create_plot -type distribution -model $model -columns "group=0,value=1"]

create_plot_dlg -model $model
