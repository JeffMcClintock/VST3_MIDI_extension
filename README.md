# VST3_MIDI_extension
Support for MIDI in the VST3 Plugin API

The VST MIDI UMP extension allows a VST3 plugin to receive MIDI events.

Both MIDI 1.0 or MIDI 2.0 protocol is supported (the plugin can choose).

##credits
Based on the VST3 SDK provided by Steinberg Media Technologies.
Thanks to reuk on the JUCE forum for the initial idea.
Thanks to Jeff McClintock for typing it up and testing it.

## How to use

Add the contents of this repo to you plugin project.
Ensure the 2 header files are in the projects include paths.

Implement the IProcessMidiProtocol interface in you Processor class:

```cpp
#include "ivstmidiump.h"
//...

class MyProcessor
    : public Steinberg::Vst::AudioEffect,
      public Steinberg::Vst::IProcessMidiProtocol
{
  // IProcessMidiProtocol
  // let the DAW know which MIDI protocol the plugin requires.

  uint32 PLUGIN_API getProcessMidiProtocol() override
  {
    return kMIDIProtocol_1_0;
  }

  //...
};
```

Act upon MIDI events in your process function:
```cpp
tresult PLUGIN_API MyProcessor::process(ProcessData& data)
{
    //...
    // process events as per usual
    if (data.inputEvents)
    {
        int32 numEvents = data.inputEvents->getEventCount();
        for (int32 i = 0; i < numEvents; i++)
        {
            Event e;
            if (data.inputEvents->getEvent(eventIndex, e) != kResultTrue)
            {
                break;
            }
            
            switch (e.type)
            {
                case Steinberg::Vst::kUMPEvent:
                {
                    // cast to UMPEvent
                    auto& midi2event = *reinterpret_cast<const UMPEvent*>(&e.noteOn);
    
                    // get the UMP data from the event
                    auto umpData = reinterpret_cast<const uint8_t*>(&midi2event.words);
    
                    const auto umptype = umpData[0] >> 4;

                    enum MessageType
                    {
                    	Utility        = 0x0, //  32 bits Utility Messages 
                    	System         = 0x1, //  32 bits System Real Time and System Common Messages (except System Exclusive)
                    	ChannelVoice32 = 0x2, //  32 bits MIDI 1.0 Channel Voice Messages
                    	Data64         = 0x3, //  64 bits Data Messages (including System Exclusive)
                    	ChannelVoice64 = 0x4, //  64 bits MIDI 2.0 Channel Voice Messages
                    	Data128        = 0x5, // 128 bits Data Messages
                    	Reserved       = 0x6, //  32 bits Reserved for future definition by MMA/AME
                    };

                    if (ChannelVoice32 == umptype) // 4-bytes. MIDI 1.0 message wrapped in a UMP packet.
                    {
                        const uint8_t* midi_1_0_bytes = umpData + 1; // just skip the first header byte, the remainder is unadulterated MIDI 1.0
                        const int midi2size = 3;
    
                        // Act upon the MIDI 1.0 message ...
                    }
                    else if(ChannelVoice64 == umptype) // 8-bytes. MIDI 2.0 message
                    {
                        const auto midi2data = reinterpret_cast<const unsigned char*>(&midi2event.words);
                        const int midi2size = 8;
    
                        // Act upon the MIDI 2.0 message ...
                    }
                }
                break;

    //...
```

#Hosting support

Assuming you DAW has a pointer to the plugin instance. e.g.

```cpp
Steinberg::Vst::IAudioProcessor* vstEffect_; // your plugin processor instance
```
You can query the MIDI protocol required by the plugin like so:
```cpp
Steinberg::Vst::IProcessMidiProtocol* midiProtocol = nullptr;
if (vstEffect_->queryInterface(Steinberg::Vst::IProcessMidiProtocol::iid, (void**)&midiProtocol) == Steinberg::kResultOk)
{
    uint32 midiProtocolRequired = midiProtocol->getProcessMidiProtocol();
    midiProtocol->release();
    
    if (midiProtocolRequired == Steinberg::Vst::kMIDIProtocol_1_0)
    {
        // Plugin requires MIDI 1.0
    }
    else if (midiProtocolRequired == Steinberg::Vst::kMIDIProtocol_2_0)
    {
        // Plugin requires MIDI 2.0
    }
}
```

When sending MIDI 1.0 events to the plugin...
```cpp
enum MessageType
{
    Utility        = 0x0, //  32 bits Utility Messages 
    System         = 0x1, //  32 bits System Real Time and System Common Messages (except System Exclusive)
    ChannelVoice32 = 0x2, //  32 bits MIDI 1.0 Channel Voice Messages
    Data64         = 0x3, //  64 bits Data Messages (including System Exclusive)
    ChannelVoice64 = 0x4, //  64 bits MIDI 2.0 Channel Voice Messages
    Data128        = 0x5, // 128 bits Data Messages
    Reserved       = 0x6, //  32 bits Reserved for future definition by MMA/AME
};

// Wrap your MIDI 1.0 message in a UMP packet.
// 'ChannelVoice32' = means MIDI 1.0 Message
constexpr int channelGroup = 0;

uint8_t wrappedMidi_1_0_message[4] =
{
	static_cast<uint8_t>((ChannelVoice32 << 4) | (channelGroup & 0x0f)),
	midiBytes[0],
	midiBytes[1],
	midiBytes[2],
};

// tag the Steinberg event as a UMP event.
m.type = Steinberg::Vst::kUMPEvent;
auto& midi2event = *reinterpret_cast<Steinberg::Vst::kUMPEvent*>(&m.noteOn);

// copy the MIDI message into the Steinberg VST3 event.
memcpy(&midi2event.words, &wrappedMidi_1_0_message, sizeof(wrappedMidi_1_0_message));
```
and transfer the event as per usual.

