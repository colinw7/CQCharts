# Pie Chart

CQChartsTest \
 -csv data/ages.csv -first_line_header \
 -type pie -columns "label=0,data=1" \
 -plot_title "pie chart" -column_type "1#integer"

CQChartsTest \
 -csv data/group_ages.csv -first_line_header \
 -type pie -columns "label=1,data=2,group=0" \
 -plot_title "grouped pie chart" -column_type "2#integer"

CQChartsTest \
 -csv data/group_ages.csv -first_line_header -fold 0 \
 -type pie -columns "label=1,data=2" \
 -plot_title "folded pie chart" -column_type "2#integer"
CQChartsTest \
 -csv data/group_ages.csv -first_line_header -fold "0:i:10" \
 -type pie -columns "label=1,data=2" \
 -plot_title "folded pie chart" -column_type "2#integer"

CQChartsTest \
 -csv data/multi_bar.csv -first_line_header \
 -type pie -columns "label=0,data=1 2 3 4 5 6 7" \
 -plot_title "multi column pie chart"
CQChartsTest \
 -csv data/multi_bar.csv -first_line_header \
 -type pie -columns "label=0,data=1 2 3 4 5 6 7" \
 -plot_title "multi column pie chart" \
 -properties "options.rowGrouping=1"
