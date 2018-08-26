set nsamp 160

proc rrand { args } {
}

proc invnorm { x } {
  return $x
}

rrand 0

set ind {}
set x   {}
set y   {}

for {set i 0} {$i < $nsamp} {incr i} {
  lappend ind $i

  lappend x [expr {[invnorm [expr rand()]]}]
  lappend y [expr {100.0*[invnorm [expr rand()]]}]
}

set modelId [load_model -var ind -var y]

set plotInd [create_plot -model $modelId -type scatter -columns "x=0,y=1"]
