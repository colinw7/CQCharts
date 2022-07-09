set model [load_charts_model -csv data/mtcars.csv -first_line_header]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x wt} {y mpg} {color {("blue")}}} \
  -properties {{mapping.color.enabled 0}} \
  -properties {{symbol.fill.alpha 0.5}}]
