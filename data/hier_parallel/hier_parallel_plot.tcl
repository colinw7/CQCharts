set model [load_charts_model -csv data/dendrogram_all_size.csv -first_line_header]

set plot [create_charts_plot -model $model -type hierparallel \
 -columns {{name 0} {value 1}} -title "hier parallel"]

set_charts_property -plot $plot -name lines.stroke.width -value 2px
set_charts_property -plot $plot -name points.symbol.size -value 16px
set_charts_property -plot $plot -name options.labels -value "{} A B C"
