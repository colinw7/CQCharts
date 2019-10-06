#CQChartsTest \
# -csv data/temp_bar_range.csv -first_line_header \
# -type barchart -columns "group=0,value=2 3" \
# -bool "rangeBar=1,horizontal=1" \
# -properties "dataLabel.visible=1,dataLabel.position=TOP_OUTSIDE" \
# -properties "key.visible=0,options.barMargin=12px,fill.color=palette:0.2"

CQChartsTest \
 -csv data/temp_bar_range.csv -first_line_header \
 -type distribution -columns "value=2 3" \
 -bool "bucketed=0,rangeBar=1" \
 -properties "dataGrouping.group=0,dataGrouping.rowGroups=1"
