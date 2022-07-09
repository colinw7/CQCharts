#set model [load_charts_model -tsv data/scatter.tsv -first_line_header]
#set plot [create_charts_plot -type delaunay -columns {{x sepalLength} {y sepalWidth} {value petalLength}}]

#set model [load_charts_model -csv data/diamonds.csv -first_line_header]
#filter_charts_model -model $model -expr {$color == "J" || $color == "D"}
#set plot [create_charts_plot -type delaunay -columns {{x carat} {y price} {value depth}}]

set model [load_charts_model -tsv data/cities1.dat -comment_header]
set plot [create_charts_plot -type delaunay -columns {{x 4} {y 3} {value 2} {tips 0}}]
