# Radar Plot

CQChartsTest \
 -csv data/radar.csv -first_line_header \
 -type radar -columns "name=0,value=1 2 3 4 5" \
 -plot_title "radar"

CQChartsTest \
 -csv data/parallel_coords.csv -first_line_header \
 -type radar -columns "name=0,value=1 2 3 4 5 6 7" \
 -plot_title "parallel"
