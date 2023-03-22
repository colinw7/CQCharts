set model [load_charts_model -csv data/HairEyeColor.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart \
  -columns {{group Hair} {values Freq} {color Eye}} \
  -where {$Sex=="Male"} -title "Male"]

set view [get_charts_data -plot $plot -name view]

#set_charts_property -view $view -name text.scaled -value 0

set_charts_property -plot $plot -name title.text.string -value "Some very long text to test fit with title that is wider than plot"
set_charts_property -plot $plot -name title.text.font -value 36

set_charts_property -plot $plot -name title.fit.horizontal -value 1

set_charts_property -plot $plot -name range.autoFit -value 1

#set_charts_property -plot $plot -name debug.showBoxes -value 1

set_charts_property -plot $plot -name title.editable -value 1

set_charts_property -plot $plot -name title.subtitle.string -value "Test subtitle"
