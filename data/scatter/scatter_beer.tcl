# ,abv,ibu,id,name,style,brewery_id,ounces
set model [load_charts_model -csv data/beers.csv -first_line_header]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x ibu} {y abv} {group brewery_id} {tips {name style ounces}}}]
