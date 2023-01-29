#include <Arduino.h>

int ControlLinePin = 5;                 // GPIO-5   Remote control line

bool ToggleBitValue = 0;                // Toggle bit (T). This bit is inverted each time a key is released and pressed again.

constexpr int RC5Unit = 889;            // In micro seconds - Unit is 32 cycles of 36 kHz carrier signal
constexpr int RC5XPauseDuration = RC5Unit * 2 * 2;

// RC5 packet 14 bit:
// | S1 bit | S2 bit | T bit | Address 5 bits | Command 6 bits |
// |    1   |   2    |  3    |      4 .. 8    |     9 .. 14    |

// RC5X (Extended) packet 20 bit:
// | S1 bit | S2 bit - inverted Command bit 6 | T bit | Address 5 bits | pause | Command 6 bits | Extension 6 bits |
// |    1   |               2                 |  3    |      4 .. 8    | 2 bit |     9 .. 14    |   14 .. 20       |

constexpr uint8_t RC5Len = 14;
constexpr uint8_t RC5XLen = 20;

constexpr uint8_t S1Value = 1;          // Start bit 1. Always = 1.
constexpr uint8_t S2Value = 1;          // Start bit 2.
                                        // RC5 - Always = 1.
                                        // RC5X - The value of S2 must be inverted to get the 7th command bit though!
                                        // That way the first 64 commands remain compatible with the original RC-5 protocol.
constexpr uint8_t TValue = 0;           // Toggle bit (T). This bit is inverted each time a key is released and pressed again.
constexpr uint8_t AddressMax = 0x1F;    // Address is 5 bits. Sent with MSB first
constexpr uint8_t CommandMax = 0x3F;    // Command is 6 bits. Sent with MSB first
constexpr uint8_t CommandExtMax = 0x7F; // Command is 7 bits. Sent with MSB first
constexpr uint8_t ExtensionMax = 0x3F;  // Extension is 6 bits. Sent with MSB first

constexpr uint8_t S1BitPosition = 1; 
constexpr uint8_t S2BitPosition = 2; 
constexpr uint8_t TBitPosition = 3;
constexpr uint8_t AddressPosition = 8;
constexpr uint8_t CommandPosition = 14;

constexpr uint8_t RC5XPausePosition = RC5XLen - AddressPosition;

template<uint8_t CommandExtended> constexpr bool GetExtendedCommandBit()
{
    static_assert(CommandExtended < CommandExtMax, "RC5 extended Command is out of range");
    return !(CommandExtended > CommandMax);
}

template<uint8_t Address, uint8_t Command> constexpr uint16_t GetRC5Code()
{
    static_assert(Address <= AddressMax, "RC5 Address is out of range");
    static_assert(Command <= CommandMax, "RC5 Command is out of range");

    return  (S1Value << RC5Len - S1BitPosition) |
            (S2Value << RC5Len - S2BitPosition) |
            (TValue  << RC5Len - TBitPosition) |
            (Address << RC5Len - AddressPosition) |
            (Command);
}

template<uint8_t Address, uint8_t Command, uint8_t Extension> constexpr uint32_t GetRC5ExtendedCode()
{
    static_assert(Address <= AddressMax, "RC5 Address is out of range");
    static_assert(Command <= CommandExtMax, "RC5 Extended Command is out of range");
    static_assert(Extension <= ExtensionMax, "RC5 Extension is out of range");
    
    return  (S1Value << RC5XLen - S1BitPosition) |
            ((S2Value & GetExtendedCommandBit<Command>())
                     << RC5XLen - S2BitPosition) |
            (TValue  << RC5XLen - TBitPosition) |
            (Address << RC5XLen - AddressPosition) |
            (Command << RC5XLen - CommandPosition) |
            (Extension);
}

inline void Send_0()
{
    digitalWrite(ControlLinePin, HIGH);
    delayMicroseconds(RC5Unit);
    digitalWrite(ControlLinePin, LOW);
    delayMicroseconds(RC5Unit);
}

inline void Send_1()
{
    digitalWrite(ControlLinePin, LOW);
    delayMicroseconds(RC5Unit);
    digitalWrite(ControlLinePin, HIGH);
    delayMicroseconds(RC5Unit);
}

inline void SetToggleBit(uint32_t &packet, bool isExtended)
{
    uint8_t TBitShift = (isExtended ? RC5XLen : RC5Len) - TBitPosition;

    if (!ToggleBitValue)
    {
        ToggleBitValue = true;
        packet |= (1 << TBitShift);
    }
    else
    {
        // packet &= ~(1 << TBitShift);  // already set to 0 in GetRC5Code()
        ToggleBitValue = false;
    }
}

// Space marker for Marantz RC5 extension after Address
inline void RC5ExtensionPause()
{
    delayMicroseconds(RC5XPauseDuration);
}

// Drop signal to zero in case last bit was sent is 1.
inline void DropLineDown(uint32_t &packet)
{
    if (packet & 1)
    {
        digitalWrite(ControlLinePin, LOW);
    }
}

// Wait repeat pause 89ms = Repeat T 114ms - Parcel duration 25ms
inline void RepeatPause()
{
    delay(89);
}

void SendRC5(uint32_t packet, bool isExtended = false, uint8_t repeatCount = 1)
{
    SetToggleBit(packet, isExtended);    

    while (repeatCount-- > 0)
    {
        uint8_t StartShift = (isExtended ? RC5XLen : RC5Len) - 1;
        
        for (int i = StartShift; i >= 0; i--)
        {
            (packet & (1 << i)) ? Send_1() : Send_0();

            if (isExtended && (i == RC5XPausePosition))
            {
                RC5ExtensionPause();
            }
        }

        DropLineDown(packet);
        RepeatPause();
    }
}

extern "C" void SendMarantzPowerOn()
{
    auto packet = GetRC5ExtendedCode<16, 12, 1>();
    SendRC5(packet, true);
}

extern "C" void SendMarantzPowerOff()
{
    auto packet = GetRC5ExtendedCode<16, 12, 2>();
    SendRC5(packet, true);
}

extern "C" void SendMarantzInputCD()
{
    auto packet = GetRC5Code<20, 63>();
    SendRC5(packet);
}

extern "C" void SendMarantzInputPhono()
{
    auto packet = GetRC5Code<21, 63>();
    SendRC5(packet);
}

extern "C" void SendMarantzInputTuner()
{
    auto packet = GetRC5Code<17, 63>();
    SendRC5(packet);
}

extern "C" void SendMarantzInputTape()
{
    auto packet = GetRC5Code<18, 63>();
    SendRC5(packet);
}

extern "C" void SendMarantzInputAux()
{
    auto packet = GetRC5ExtendedCode<16, 0, 6>();
    SendRC5(packet, true);
}

extern "C" void SendMarantzInputCdrMd()
{
    auto packet = GetRC5Code<23, 63>();
    SendRC5(packet);
}

extern "C" void SendMarantzVolumeUp()
{
    auto packet = GetRC5Code<16, 16>();
    SendRC5(packet, false, 2);
}

extern "C" void SendMarantzVolumeDown()
{
    auto packet = GetRC5Code<16, 17>();
    SendRC5(packet, false, 2);
}

extern "C" void SendMarantzMute()
{
    auto packet = GetRC5Code<16,13>();
    SendRC5(packet, false, 2);
}
