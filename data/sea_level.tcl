set modelId [load_model -csv data/sea_level.csv -first_line_header]

set_model -ind $modelId -column_type "0#real:format=%gM"

set plotId [create_plot -type bar -columns "category=1,value=0,color=2,label=3" -ymax 8000]

set_property -view view1 -plot $plotId -name dataLabel.visible -value 1
set_property -view view1 -plot $plotId -name dataLabel.position -value TOP_OUTSIDE
set_property -view view1 -plot $plotId -name invertY -value 1
set_property -view view1 -plot $plotId -name "yaxis.grid.line.major.visible" -value 1
