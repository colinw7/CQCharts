set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x sepalLength} {y sepalWidth}} \
  -properties {{symbol.symbol circle} {symbol.size 5px}}]

set_charts_palette -theme default -name add_palette -value red_green
set_charts_palette -theme default -name add_palette -value reds

set_charts_palette -theme default -name remove_palette -value set2

set_charts_palette -theme default -name move_palette -value red_green -data 0
set_charts_palette -theme default -name set_palette -value reds -data 1
