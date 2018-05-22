set modelId [load_model -tsv data/cities1.dat -comment_header -column_type "2#real:min=0"]

puts $modelId

set nr [get_property -model $modelId -name num_rows]

set rc [get_property -model $modelId -name num_columns]

#process_model -ind $modelId -add "column(2)/20000.0"
process_model -ind $modelId -add "remap(2,0,36)" -header "symbol size"
process_model -ind $modelId -add "remap(2,0,1)" -header color

# columns x, y, name symbolSize, fontSize, color, id
#remove_plot -view view1 -all
#create_plot -model $modelId -type scatter -columns "x=3,y=4,symbolSize=5"

#remove_plot -view view1 -all
create_plot -model $modelId -type scatter -columns "x=3,y=4,symbolSize=5,color=6"
