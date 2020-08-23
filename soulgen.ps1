$patchName = "Level.soulpatch"
soul generate --juce-header --output=Source/soulpatch.h Source/soul/$patchName
soul generate --juce-cpp --output=Source/soulpatch.cpp Source/soul/$patchName
# soul generate --cpp --output=Source/soulpatch.cpp Source/soul/SineSynth.soulpatch;
# soul generate --graph --output=${workspaceRoot}\\Source\\soul\\graph\\graph.html ${workspaceRoot}\\Source\\soul\\SineSynth.soulpatch