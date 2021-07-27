#set model [load_charts_model -tsv data/monthly_sales.tsv -comment_header]
#set plot [create_charts_plot -type summary -model $model -columns {{columns {0 1 2 3 4}}}]

#set model [load_charts_model -csv data/birds.csv -first_line_header]
#set plot [create_charts_plot -type summary -model $model -columns {{columns {0 1 2 3 4 5 6 7 8 9 10 11 12 13}}}]

#set model [load_charts_model -csv data/msleep.csv -first_line_header]
#set plot [create_charts_plot -type summary -model $model -columns {{columns {0 1 2 3 4 5 6 7 8 9}}}]

#set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
#  -column_type {{time {format %Y%m%d}}}]
#set plot [create_charts_plot -type summary -model $model -columns {{columns {0 1 2 3}}}]

#set model [load_charts_model -csv data/housetasks.csv -first_line_header -first_column_header]
#set plot [create_charts_plot -type summary -model $model -columns {{columns {1 2 3}}}]

#set model [load_charts_model -csv data/mtcars.csv -first_line_header]
#set plot [create_charts_plot -type summary -model $model -columns {{columns {1 2 3 4 5 6 7 8 9 10 11}}}]

#set model [load_charts_model -csv data/airports.csv -comment_header]
#set plot [create_charts_plot -type summary -model $model -columns {{columns {1 2 3 4 5 6}}}]

#set model [load_charts_model -tsv data/scatter.tsv -first_line_header]
#set plot [create_charts_plot -type summary -model $model -columns {{group 4} {columns {0 1 2 3}}}]

#set model [load_charts_model -csv data/beers.csv -first_line_header]
#set plot [create_charts_plot -type summary -model $model -columns {{columns {1 2 3 4 5 6 7}}}]

#set model [load_charts_model -csv data/diamonds.csv -first_line_header]
#set plot [create_charts_plot -type summary -model $model -columns {{columns {0 1 2 3 4 5 6 7 8 9}}}]

set model [load_charts_model -csv data/Pokemon.csv -comment_header]
set plot [create_charts_plot -type summary -model $model -columns {{columns {1 2 3 4 5 6 7 8 9 10 11 12}}}]
