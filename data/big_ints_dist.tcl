set model [load_charts_model -csv data/big_ints.csv -first_line_header \
 -column_type {{{1 real} {format %gM} {format_scale 0.000001}}}]

set plot [create_charts_plot -type distribution -model $model -columns {{values 1}}]
