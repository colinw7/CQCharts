CQChartsTest -csv data/multi_bar.csv -first_line_header -first_column_header \
 -type barchart -columns "group=@VH,value=0" -column_type "1#integer" \
 -plot_title "bar chart"
