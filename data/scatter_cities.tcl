set model [load_charts_model -tsv data/cities1.dat -comment_header]

process_charts_model -model $model -add -expr "column(2)/1000.0" -header "pop1"

set plot [create_charts_plot -model $model -type scatter \
 -columns {{x 4} {y 3} {name 0} {fontSize 5}} \
 -parameter {pointLabels 1} \
 -parameter {key 0} \
 -properties {{mapping.font_size.enabled 1}} \
 -properties {{dataLabel.position CENTER}} \
]
