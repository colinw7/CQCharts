# Parallel Plot

CQChartsTest \
 -csv data/parallel_coords.csv -first_line_header \
 -type parallel -columns "x=0,y=1 2 3 4 5 6 7" \
 -plot_title "parallel"

CQChartsTest \
 -csv data/radar.csv -first_line_header \
 -type parallel -columns "x=0,y=1 2 3 4 5" \
 -plot_title "radar"
