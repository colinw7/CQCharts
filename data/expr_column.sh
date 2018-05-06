CQChartsTest -csv data/ages.csv -first_line_header \
  -type xy -columns "x=(row),y=(cos(x))" -plot_title "y = cos(x)" -close_app

CQChartsTest -expr -num_rows 100 \
  -type xy -columns "x=(x),y=(cos(x/20.0))" -plot_title "y = cos(x)" -close_app
