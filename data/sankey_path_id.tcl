# sankey link model
set model [load_charts_model -csv data/sankey_path_id.csv -first_line_header]

set plot [create_charts_plot -model $model -type sankey \
 -columns {{from From} {to To} {value Value} {depth Depth} {attributes Attributes}}]
