#!/bin/sh
for ((label=0; label<10; label++)); do
    echo preprocessing digit images $label;
    ls 1_form/$label | xargs -I srcimg ./BoxExtraction/main 1_form/$label/srcimg 2_boxes/$label/;
    ls 2_boxes/$label | xargs -I imgname ./CutEdge/main 2_boxes/$label/imgname 3_cropped/$label/imgname 0.2;
done;