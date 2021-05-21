#set model [load_charts_model -tsv data/monthly_sales.tsv -comment_header]
#set plot [create_charts_plot -type table -model $model -columns {{columns {0 1 2 3 4}}}]

#set model [load_charts_model -csv data/birds.csv -first_line_header]
#set plot [create_charts_plot -type table -model $model -columns {{columns {0 1 2 3 4 5 6 7 8 9 10 11 12 13 14}}}]

#set model [load_charts_model -csv data/msleep.csv -first_line_header]
#set plot [create_charts_plot -type table -model $model -columns {{columns {0 1 2 3 4 5 6 7 8 9}}}]

#set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
#  -column_type {{time {format %Y%m%d}}}]
#set plot [create_charts_plot -type table -model $model -columns {{columns {0 1 2 3}}}]

set model [load_charts_model -csv data/housetasks.csv -first_line_header -first_column_header]
set plot [create_charts_plot -type table -model $model -columns {{columns {0 1 2 3}}}]

set objs [get_charts_data -plot $plot -name objects -sync]

#assert {[llength $objs] == 0}
