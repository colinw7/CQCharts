set model [load_charts_model -csv data/xy_100000.csv -first_line_header]

#set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
#  -column_type {{{0 time} {iformat %Y%m%d} {oformat %d/%m/%Y}}}]

show_charts_create_plot_dlg -model $model
