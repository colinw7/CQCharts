# Pie Chart
#

# Tests:
#   Simple
#   Grouped
#   Folded
#   Multi value
#   Multi value (grouped by row)

CQChartsTest \
 -csv data/ages.csv -first_line_header \
 -type pie -columns "label=0,value=1" \
 -plot_title "pie chart" -column_type "1#integer" \
-and \
 -csv data/group_ages.csv -first_line_header \
 -type pie -columns "label=1,value=2,group=0" \
 -plot_title "grouped pie chart" -column_type "2#integer" \
-and \
 -csv data/group_ages.csv -first_line_header -fold 0 \
 -type pie -columns "label=1,value=2" \
 -plot_title "folded pie chart" -column_type "2#integer" \
-and \
 -csv data/group_ages.csv -first_line_header -fold "0:i:10" \
 -type pie -columns "label=1,value=2" \
 -plot_title "folded pie chart" -column_type "2#integer" \
-and \
 -csv data/multi_bar.csv -first_line_header \
 -type pie -columns "label=0,value=1 2 3 4 5 6 7" \
 -plot_title "multi column pie chart" \
-and \
 -csv data/multi_bar.csv -first_line_header \
 -type pie -columns "label=0,value=1 2 3 4 5 6 7" \
 -plot_title "multi column pie chart with row grouping" \
 -properties "dataGrouping.rowGroups=1"
