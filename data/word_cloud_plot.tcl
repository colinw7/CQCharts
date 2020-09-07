# sankey link model
set model [load_charts_model -csv data/word_cloud.csv -comment_header]

set plot [create_charts_plot -model $model -type wordCloud -columns {{value 0}}]
