proc objPressed { view plot id } {
  set inds [get_charts_data -plot $plot -object $id -name inds]
  if {[llength $inds] < 1} { return }

  set ind [lindex $inds 0]

  set model [get_charts_data -plot $plot -name model]

  set name [get_charts_data -model $model -name value -ind $ind]

  set_charts_property -annotation $::rectId -name fill.pattern -value "palette:$name"

  set_charts_property -annotation $::textId -name text.string -value "Palette $name"
}

set palettes [get_charts_palette -name palettes]

set model [load_charts_model -tcl [list $palettes]]

set plot [create_charts_plot -type table -model $model -columns {{columns {0}}}]

set_charts_property -plot $plot -name header.visible -value 0

set view [get_charts_data -plot $plot -name view]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed

set textId [create_charts_text_annotation -view $view -id paletteText -tip "Palette Text" \
  -position {10 80}]

set rectId [create_charts_rectangle_annotation -view $view -id paletteRect -tip "Palette Preview" \
  -start {10 40} -end {40 70} -filled 1 -fill_color white]
