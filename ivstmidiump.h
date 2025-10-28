//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : pluginterfaces/vst/ivstprefetchablesupport.h
// Created by  : Jeff McClintock, 10/2025
// Description : VST MIDI UMP Support Interface
//
//-----------------------------------------------------------------------------
// VST MIDI UMP Interface specification.
// Copyright 2025 Jeff McClintock.
// 
// Permission to use, copy, modify, and /or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS.IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//-----------------------------------------------------------------------------

#pragma once
#include "pluginterfaces/base/ftypes.h"
#include "pluginterfaces/base/funknown.h"

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpush.h"
//------------------------------------------------------------------------

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

// ------------------------------------------------------------------------
/** \ingroup vst3typedef */
/**@{*/
/** MIDI Protocol Support Type */
typedef uint32 MidiProtocolSupport;
/**@}*/

/** MIDI Protocol Support Enum */
enum eMidiProtocolSupport
{
    kMIDIProtocol_1_0 = 0,
    kMIDIProtocol_2_0
};

//------------------------------------------------------------------------
// IMidiProtocolSupport Interface
//------------------------------------------------------------------------
/** Indicates that the plug-in does support MIDI UMP events
\ingroup vstIPlug vst365
- [plug imp]
- [extends IComponent]
- [released: 3.6.5]
- [optional]

The plug-in should implement this interface if supports MIDI UMP note events.
The host asks for this information once between initialize and setActive. It cannot be changed afterwards.
By default (without implementing this interface) the host uses kNoteOnEvent, kNoteOffEvent etc events.

\section IProcessMidiProtocol Example

\code{.cpp}
//------------------------------------------------------------------------
Steinberg::uint32 PLUGIN_API getProcessMidiProtocol() override
{
    return vst3_ext_midi::IProcessMidiProtocol::kMIDIProtocol_1_0;
}
\endcode
*/

class IProcessMidiProtocol : public FUnknown
{
public:
    virtual Steinberg::uint32 PLUGIN_API getProcessMidiProtocol() = 0;

    //------------------------------------------------------------------------
    static const FUID iid;
};

DECLARE_CLASS_IID(IProcessMidiProtocol, 0x61C7B395, 0xC49643B4, 0x93DCEB01, 0x603E29EA)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpop.h"
//------------------------------------------------------------------------