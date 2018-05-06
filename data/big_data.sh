# Performance

CQChartsTest \
 -csv data/xy_10000.csv -first_line_header \
 -type xy -columns "x=0,y=1" \
 -plot_title "10000 points" \
 -properties "points.visible=0"
CQChartsTest \
 -csv data/xy_10000.csv -first_line_header \
 -type barchart -columns "x=0,y=1" \
 -plot_title "10000 points" \
 -properties "stroke.visible=0"
CQChartsTest \
 -csv data/xy_10000.csv -first_line_header \
 -type scatter -columns "x=0,y=1" \
 -plot_title "10000 points"
CQChartsTest \
 -csv data/random_10000.csv -first_line_header \
 -type bubble -columns "name=0,value=1" \
 -plot_title "10000 points"

CQChartsTest \
 -csv data/xy_100000.csv -first_line_header \
 -type xy -columns "x=0,y=1" \
 -plot_title "100000 points" \
 -properties "lines.visible=0"
CQChartsTest \
 -csv data/xy_100000.csv -first_line_header \
 -type barchart -columns "x=0,y=1" \
 -plot_title "100000 points" \
 -properties "stroke.visible=0"
CQChartsTest \
 -csv data/xy_100000.csv -first_line_header \
 -type scatter -columns "x=0,y=1" \
 -plot_title "100000 points"
CQChartsTest \
 -csv data/xy_100000_positive.csv -first_line_header \
 -type bubble -columns "name=0,value=1" \
 -plot_title "100000 points"
