set model [load_charts_model -csv data/mtcars.csv -first_line_header]

#set plot [create_charts_plot -model $model -type scatter \
# -columns {{x wt} {y mpg} {group cyl} {symbolSize qsec}}]

#set plot [create_charts_plot -model $model -type scatter \
# -columns {{x wt} {y mpg} {group cyl} {color mpg}}]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x wt} {y mpg} {group cyl} {name gear} {color mpg}}]
