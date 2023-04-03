set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:1 -tip "annotation:1" -start {-200 50 P} -end {200 50 P} -fhead none -thead triangle -mid_head none -length {20px 1V 30px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.3}
set_charts_property -annotation $ -name frontHead.angle -value {30}
set_charts_property -annotation $ -name frontHead.length -value {20px}
set_charts_property -annotation $ -name stroke.color -value {palette 0.3}
set_charts_property -annotation $ -name stroke.width -value {3px}
set_charts_property -annotation $ -name tailHead.angle -value {30}
set_charts_property -annotation $ -name tailHead.length -value {30px}
set_charts_property -annotation $ -name tailHead.type -value {TRIANGLE}
set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:2 -tip "annotation:2" -start {-200 40 P} -end {200 40 P} -fhead triangle -thead none -mid_head none -length {30px 1V 45px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.6}
set_charts_property -annotation $ -name fill.visible -value {false}
set_charts_property -annotation $ -name frontHead.angle -value {30}
set_charts_property -annotation $ -name frontHead.length -value {30px}
set_charts_property -annotation $ -name frontHead.type -value {TRIANGLE}
set_charts_property -annotation $ -name stroke.color -value {palette 0.6}
set_charts_property -annotation $ -name stroke.visible -value {true}
set_charts_property -annotation $ -name stroke.width -value {3px}
set_charts_property -annotation $ -name tailHead.angle -value {30}
set_charts_property -annotation $ -name tailHead.length -value {45px}
set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:3 -tip "annotation:3" -start {-200 30 P} -end {200 30 P} -fhead none -thead none -mid_head none -length {30px 1V 45px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.3}
set_charts_property -annotation $ -name frontHead.length -value {30px}
set_charts_property -annotation $ -name stroke.color -value {palette 0.3}
set_charts_property -annotation $ -name stroke.width -value {3px}
set_charts_property -annotation $ -name tailHead.length -value {45px}
set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:4 -tip "annotation:4" -start {-200 20 P} -end {200 20 P} -fhead triangle -thead triangle -mid_head none -length {30px 1V 45px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.6}
set_charts_property -annotation $ -name frontHead.angle -value {30}
set_charts_property -annotation $ -name frontHead.length -value {30px}
set_charts_property -annotation $ -name frontHead.type -value {TRIANGLE}
set_charts_property -annotation $ -name stroke.color -value {palette 0.6}
set_charts_property -annotation $ -name stroke.width -value {3px}
set_charts_property -annotation $ -name tailHead.angle -value {30}
set_charts_property -annotation $ -name tailHead.length -value {45px}
set_charts_property -annotation $ -name tailHead.type -value {TRIANGLE}
set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:5 -tip "annotation:5" -start {-200 10 P} -end {200 10 P} -fhead stealth -thead stealth -mid_head triangle -length {30px 1V 45px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.3}
set_charts_property -annotation $ -name frontHead.angle -value {30}
set_charts_property -annotation $ -name frontHead.length -value {30px}
set_charts_property -annotation $ -name frontHead.type -value {STEALTH}
set_charts_property -annotation $ -name midHead.angle -value {30}
set_charts_property -annotation $ -name midHead.type -value {TRIANGLE}
set_charts_property -annotation $ -name stroke.color -value {palette 0.3}
set_charts_property -annotation $ -name stroke.width -value {3px}
set_charts_property -annotation $ -name tailHead.angle -value {30}
set_charts_property -annotation $ -name tailHead.length -value {45px}
set_charts_property -annotation $ -name tailHead.type -value {STEALTH}
set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:6 -tip "annotation:6" -start {-200 0 P} -end {200 0 P} -fhead diamond -thead diamond -mid_head none -length {30px 1V 45px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.6}
set_charts_property -annotation $ -name frontHead.angle -value {30}
set_charts_property -annotation $ -name frontHead.length -value {30px}
set_charts_property -annotation $ -name frontHead.type -value {DIAMOND}
set_charts_property -annotation $ -name stroke.color -value {palette 0.6}
set_charts_property -annotation $ -name stroke.width -value {3px}
set_charts_property -annotation $ -name tailHead.angle -value {30}
set_charts_property -annotation $ -name tailHead.length -value {45px}
set_charts_property -annotation $ -name tailHead.type -value {DIAMOND}
set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:7 -tip "annotation:7" -start {-200 -10 P} -end {200 -10 P} -fhead line -thead line -mid_head none -length {20px 1V 30px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.3}
set_charts_property -annotation $ -name frontHead.length -value {20px}
set_charts_property -annotation $ -name frontHead.type -value {LINE}
set_charts_property -annotation $ -name stroke.color -value {palette 0.3}
set_charts_property -annotation $ -name stroke.width -value {3px}
set_charts_property -annotation $ -name tailHead.length -value {30px}
set_charts_property -annotation $ -name tailHead.type -value {LINE}
set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:8 -tip "annotation:8" -start {-200 -20 P} -end {200 -20 P} -fhead triangle -thead line -mid_head none -length {20px 1V 30px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.6}
set_charts_property -annotation $ -name fill.visible -value {false}
set_charts_property -annotation $ -name frontHead.angle -value {30}
set_charts_property -annotation $ -name frontHead.length -value {20px}
set_charts_property -annotation $ -name frontHead.type -value {TRIANGLE}
set_charts_property -annotation $ -name stroke.color -value {palette 0.6}
set_charts_property -annotation $ -name stroke.visible -value {true}
set_charts_property -annotation $ -name stroke.width -value {3px}
set_charts_property -annotation $ -name tailHead.length -value {30px}
set_charts_property -annotation $ -name tailHead.type -value {LINE}
set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:9 -tip "annotation:9" -start {-200 -30 P} -end {200 -30 P} -fhead line -thead triangle -mid_head none -length {20px 1V 30px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.3}
set_charts_property -annotation $ -name frontHead.length -value {20px}
set_charts_property -annotation $ -name frontHead.type -value {LINE}
set_charts_property -annotation $ -name stroke.color -value {palette 0.3}
set_charts_property -annotation $ -name stroke.width -value {8px}
set_charts_property -annotation $ -name tailHead.angle -value {30}
set_charts_property -annotation $ -name tailHead.length -value {30px}
set_charts_property -annotation $ -name tailHead.type -value {TRIANGLE}
set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:10 -tip "annotation:10" -start {-200 -40 P} -end {200 -40 P} -fhead none -thead line -mid_head none -length {20px 1V 30px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.3}
set_charts_property -annotation $ -name frontHead.length -value {20px}
set_charts_property -annotation $ -name stroke.color -value {palette 0.3}
set_charts_property -annotation $ -name stroke.width -value {11px}
set_charts_property -annotation $ -name tailHead.length -value {30px}
set_charts_property -annotation $ -name tailHead.type -value {LINE}
set plot [create_charts_arrow_annotation -plot plot:xy:1 -id annotation:11 -tip "annotation:11" -start {-200 -50 P} -end {200 -50 P} -fhead 1 -thead 1 -mid_head none -angle {30 -1 45} -back_angle {40 40} -length {40px 1V 30px}]

set_charts_property -annotation $ -name fill.color -value {palette 0.3}
set_charts_property -annotation $ -name frontHead.angle -value {30}
set_charts_property -annotation $ -name frontHead.length -value {40px}
set_charts_property -annotation $ -name frontHead.type -value {STEALTH}
set_charts_property -annotation $ -name stroke.color -value {palette 0.3}
set_charts_property -annotation $ -name stroke.width -value {5px}
set_charts_property -annotation $ -name tailHead.angle -value {45}
set_charts_property -annotation $ -name tailHead.length -value {30px}
set_charts_property -annotation $ -name tailHead.type -value {STEALTH}
