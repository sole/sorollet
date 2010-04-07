#
# Renoise's xrns song format to song.h converter
# to be used with sorollet player ;-)
# 
import sys
import re
import zipfile
from xml.dom.minidom import parseString

def getText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc

def getNodeText(node):
    return node.childNodes[0].nodeValue

def getChildrenCount(node):
    if not node.hasChildNodes():
        return 0
    else:
        c = 0
        child = node.firstChild
        while child:
            if child.nodeType == child.ELEMENT_NODE:
                c = c + 1
            child = child.nextSibling
        return c
		
def getElementByTagName(node, tagName):
    if not node.hasChildNodes():
        return None
    else:
        child = node.firstChild
        while child:
            if child.nodeType == child.ELEMENT_NODE and child.tagName == tagName:
                return child
            child = child.nextSibling

    return None

noteMap = {}
def initNoteMap():
    notes = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
    for i in range(127):
        key = notes[i % 12]
        octave = i / 12
		
        if len(key) == 1:
            key = key + "-"
		
        key = key + ("%d" % octave)
		
        noteMap[key] = i
		
    noteMap["OFF"] = -2


def main(argv=None):

    if argv is None:
        argv = sys.argv
    
    if argv is None or len(argv) < 3:
        print ""
        print "Usage: "
        print sys.argv[0], "/path/to/input.xrns /path/to/output.h"
        print ""
        sys.exit(0)

    inputFile = argv[1]
    outputFile = argv[2]

    # Get & parse Song.xml
    songXMLData = None
    zsong = zipfile.ZipFile(inputFile, "r")
    for info in zsong.infolist():
        filename = info.filename
        if filename == 'Song.xml':
            songXMLData = zsong.read(filename)
            break
    zsong.close()

    if songXMLData is None:
        print "Song.xml could not be found, maybe ", inputFile, "wasn't actually a Renoise Song?"
        print "Anyway, I can't proceed beyond this point!"
        sys.exit(0)

    dom = parseString(songXMLData) #parse("./data/Song.xml")

    # Process ~~~~~~~~~~~~

    data = []
    initNoteMap()

    # GlobalSongData
    gsd_node = dom.getElementsByTagName("GlobalSongData")[0]
    bpm_node = gsd_node.getElementsByTagName("BeatsPerMin")[0]
    data.append(getNodeText(bpm_node))

    node = gsd_node.getElementsByTagName("LinesPerBeat")[0]
    data.append(getNodeText(node))

    node = gsd_node.getElementsByTagName("TicksPerLine")[0]
    data.append(getNodeText(node))

    # Instruments
    instruments = dom.getElementsByTagName("Instruments")[0]
    instrumentCount = 0
    instrument = instruments.firstChild
    while instrument:
        if instrument.nodeType == instrument.ELEMENT_NODE:
            insName = getElementByTagName(instrument, "Name")
            if insName != None:
                name = getNodeText(insName)
                if len(name) > 0:
                    instrumentCount = instrumentCount + 1
        instrument = instrument.nextSibling

    data.append(instrumentCount) # store instruments count
    instrument = instruments.firstChild
    while instrument:
        if instrument.nodeType == instrument.ELEMENT_NODE:
            insName = getElementByTagName(instrument, "Name")
            if insName != None:
                name = getNodeText(insName)
                if len(name) > 0:
                    pluginProperties = getElementByTagName(instrument, "PluginProperties")
                    pluginDevice = getElementByTagName(pluginProperties, "PluginDevice")
                    parameters = getElementByTagName(pluginDevice, "Parameters")
                    data.append(getChildrenCount(parameters)) # store parameters count

                    parameter = getElementByTagName(parameters, "Parameter")
                    while parameter:
                        if parameter.nodeType == parameter.ELEMENT_NODE:
                            value = getNodeText(getElementByTagName(parameter, "Value"))
                            data.append(value)
                        parameter = parameter.nextSibling

        instrument = instrument.nextSibling

    # Tracks
    tracks = dom.getElementsByTagName("Tracks")[0]
    trackCount = 0
    track = tracks.firstChild
    while track:
        if track.nodeType == track.ELEMENT_NODE and track.tagName == "SequencerTrack":
            trackCount = trackCount + 1 # do not take into account non sequencer tracks such as master track, send tracks, etc
        track = track.nextSibling

    data.append(trackCount) # store tracks count
    track = tracks.firstChild
    while track:
        if track.nodeType == track.ELEMENT_NODE and track.tagName == "SequencerTrack":
            # is this track automated at any point?
            automationDevice = track.getElementsByTagName("InstrumentAutomationDevice")
            if len(automationDevice) == 0:
                data.append(0.0)
            else:
                data.append(1.0)
                automationDevice = automationDevice[0]
                linkedInstrument = getElementByTagName(automationDevice, "LinkedInstrument")
                data.append(getNodeText(linkedInstrument)) # instrument this automation modifies
                for i in range(14):
                    parameterTagName = "ParameterNumber" + ("%d" % i)
                    parameter = getElementByTagName(automationDevice, parameterTagName)
                    if parameter != None:
                        data.append(getNodeText(parameter))
                    else:
                        data.append(0.0) # in the strange case something is missing, store 0.0s so as to not to lose the "alignment"
        track = track.nextSibling

    # Patterns
    patterns = dom.getElementsByTagName("Patterns")[0]
    data.append(getChildrenCount(patterns))
    pattern = patterns.firstChild
    while pattern:
        if pattern.nodeType == pattern.ELEMENT_NODE:
            numberOfLines = getElementByTagName(pattern, "NumberOfLines")
            data.append(getNodeText(numberOfLines))

            tracks = getElementByTagName(pattern, "Tracks")
            track = tracks.firstChild
            while track:
                if track.nodeType == track.ELEMENT_NODE and track.tagName == "PatternTrack":
                    automations = getElementByTagName(track, "Automations")
                    if automations != None:
                        data.append(1.0) # make clear we have automation for this track, in this pattern

                        envelopes = getElementByTagName(automations, "Envelopes")
                        if envelopes != None:
                            envCount = 0
                            envelope = getElementByTagName(envelopes, "Envelope")
                            while envelope:
                                if envelope.nodeType == envelope.ELEMENT_NODE and envelope.tagName == "Envelope":
                                    envCount = envCount + 1
                                envelope = envelope.nextSibling
                            data.append(envCount) # store envelope count

                            # There may be more than one envelope, each one controlling a parameter
                            envelope = getElementByTagName(envelopes, "Envelope")
                            while envelope:
                                if envelope.nodeType == envelope.ELEMENT_NODE and envelope.tagName == "Envelope":
                                    parameterIndex = getElementByTagName(envelope, "ParameterIndex")
                                    data.append(getNodeText(parameterIndex)) # store parameter index

                                    envelopeData = getElementByTagName(envelope, "Envelope")

                                    playMode = getElementByTagName(envelopeData, "PlayMode")
                                    mode = getNodeText(playMode)
                                    if mode == "Points":
                                        data.append(0.0)
                                    else:
                                        data.append(1.0)

                                    length = getElementByTagName(envelopeData, "Length")
                                    data.append(getNodeText(length)) # length in rows
                                    
                                    points = getElementByTagName(envelopeData, "Points")
                                    data.append(getChildrenCount(points)) # length in points
                                    
                                    # And there go the points themselves
                                    point = getElementByTagName(points, "Point")
                                    while point:
                                        if point.nodeType == point.ELEMENT_NODE:
                                            pvalue = getNodeText(point)
                                            envRow, envValue = re.split(",", pvalue) # they are encoded as row,value pairs
                                            data.append(envRow)
                                            data.append(envValue)
                                            
                                        point = point.nextSibling

                                envelope = envelope.nextSibling
                    else:
                        data.append(0.0) # no automation

                    lines = getElementByTagName(track, "Lines")
                    if lines != None:
                        data.append(getChildrenCount(lines))

                        line = getElementByTagName(lines, "Line")
                        while line:
                            if line.nodeType == line.ELEMENT_NODE:
                                row = line.attributes["index"].nodeValue
                                
                                # set null values to start with
                                valNote = -1
                                valInstrument = -1
                                valVolume = -1
                                valEffectNumber = -1
                                valEffectValue = -1

                                noteColumns = getElementByTagName(line, "NoteColumns")
                                if noteColumns != None:
                                    noteColumn = getElementByTagName(noteColumns, "NoteColumn")
                                    if noteColumn != None:
                                        note = getElementByTagName(noteColumn, "Note")
                                        if note != None:
                                            noteText = getNodeText(note)
                                            valNote = noteMap[noteText]

                                        instrument = getElementByTagName(noteColumn, "Instrument")
                                        if instrument:
                                            instrText = getNodeText(instrument)
                                            valInstrument = int(instrText, 16)

                                            if valInstrument >= instrumentCount:
                                                print "Warning!!! Instrument", valInstrument, "doesn't exist! (max", instrumentCount, ")"

                                        volume = getElementByTagName(noteColumn, "Volume")
                                        if volume:
                                            valVolume = int(getNodeText(volume), 16)

                                effectColumns = getElementByTagName(line, "EffectColumns")
                                if effectColumns != None:
                                    effectColumn = getElementByTagName(effectColumns, "EffectColumn")
                                    if effectColumn != None:
                                        effectValue = getElementByTagName(effectColumn, "Value")
                                        effectNumber = getElementByTagName(effectColumn, "Number") # name of the effect
                                        valEffectNumber = int(getNodeText(effectNumber), 16)
                                        valEffectValue = int(getNodeText(effectValue), 16)
                                data.append(row)
                                data.append(valNote)
                                data.append(valInstrument)
                                data.append(valVolume)
                                data.append(valEffectNumber)
                                # do not add the effect value if there's no effect
                                # (saves a few bytes, I guess ;))
                                if valEffectNumber >= 0:
                                    data.append(valEffectValue)
                            line = line.nextSibling
                    else:
                        data.append(0.0) # when there's no data for a track in a pattern
                track = track.nextSibling

        pattern = pattern.nextSibling

    # Order list
    sequenceEntries = dom.getElementsByTagName("SequenceEntries")[0]
    data.append(getChildrenCount(sequenceEntries))
    sequenceEntry = getElementByTagName(sequenceEntries, "SequenceEntry")
    while sequenceEntry:
        if sequenceEntry.nodeType == sequenceEntry.ELEMENT_NODE:
            pattern = getElementByTagName(sequenceEntry, "Pattern")
            data.append(getNodeText(pattern))
        sequenceEntry = sequenceEntry.nextSibling

    # And 'data' to a floats array
    dataf = []
    print len(data), "values"
    for v in data:
        dataf.append("%ff" % float(v))

    song_h = "/*\n Automatically generated file! Do not modify!\n */\n"
    song_h += "float song[" + ("%d" % len(dataf)) + "] = {" + ",\n".join(dataf) + "};"

    f = open(outputFile, 'w')
    f.write(song_h)
    f.close()


# and this is where the process is launched...
if __name__ == "__main__":
    main()