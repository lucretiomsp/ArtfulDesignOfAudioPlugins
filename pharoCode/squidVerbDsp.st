"pitchShifeters"
pitchShiftL := (PhHSlider new label: 'ShiftL' values: #(0 -12 12 0.1)) * ( PhButton new label: 'leftEye').
pitchShiftR := (PhHSlider new label: 'ShiftR' values: #(0 -12 12 0.1) )* (PhButton new label: 'rightEye').
"samplePlayer into filter"
samplere := (TpSampler  new label: 'bleep' ) => OberheimBPF new  .


transposerL := samplere => (PhTranspose new shift: pitchShiftL).
transposerR := samplere => (PhTranspose new shift: pitchShiftR).

reverb := GreyHole  new inputL: transposerL ; inputR: transposerR.

dsp := reverb asDsp 
dsp init.
dsp start.

"export to Juce project"
dsp export2JuceWithName: 'SquidVerb' type: 'Effect'. 
