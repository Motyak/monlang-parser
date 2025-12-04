/*
    https://www.codestudy.net/blog/how-to-know-the-preferred-display-width-in-columns-of-unicode-characters/
*/

#ifndef UNICODE_EAW
#define UNICODE_EAW

#include <cstdint>

// extracted from https://www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt,
// ..F and W categories (excluded A because depends on locale)
bool is_fullwidth(uint32_t codepoint) {
    if (
        (0x1100 <= codepoint && codepoint <= 0x115F)
        || (0x231A <= codepoint && codepoint <= 0x231B)
        || codepoint == 0x2329
        || codepoint == 0x232A
        || (0x23E9 <= codepoint && codepoint <= 0x23EC)
        || codepoint == 0x23F0
        || codepoint == 0x23F3
        || (0x25FD <= codepoint && codepoint <= 0x25FE)
        || (0x2614 <= codepoint && codepoint <= 0x2615)
        || (0x2630 <= codepoint && codepoint <= 0x2637)
        || (0x2648 <= codepoint && codepoint <= 0x2653)
        || codepoint == 0x267F
        || (0x268A <= codepoint && codepoint <= 0x268F)
        || codepoint == 0x2693
        || codepoint == 0x26A1
        || (0x26AA <= codepoint && codepoint <= 0x26AB)
        || (0x26BD <= codepoint && codepoint <= 0x26BE)
        || (0x26C4 <= codepoint && codepoint <= 0x26C5)
        || codepoint == 0x26CE
        || codepoint == 0x26D4
        || codepoint == 0x26EA
        || (0x26F2 <= codepoint && codepoint <= 0x26F3)
        || codepoint == 0x26F5
        || codepoint == 0x26FA
        || codepoint == 0x26FD
        || codepoint == 0x2705
        || (0x270A <= codepoint && codepoint <= 0x270B)
        || codepoint == 0x2728
        || codepoint == 0x274C
        || codepoint == 0x274E
        || (0x2753 <= codepoint && codepoint <= 0x2755)
        || codepoint == 0x2757
        || (0x2795 <= codepoint && codepoint <= 0x2797)
        || codepoint == 0x27B0
        || codepoint == 0x27BF
        || (0x2B1B <= codepoint && codepoint <= 0x2B1C)
        || codepoint == 0x2B50
        || codepoint == 0x2B55
        || (0x2E80 <= codepoint && codepoint <= 0x2E99)
        || (0x2E9B <= codepoint && codepoint <= 0x2EF3)
        || (0x2F00 <= codepoint && codepoint <= 0x2FD5)
        || (0x2FF0 <= codepoint && codepoint <= 0x2FFF)
        || codepoint == 0x3000
        || (0x3001 <= codepoint && codepoint <= 0x3003)
        || codepoint == 0x3004
        || codepoint == 0x3005
        || codepoint == 0x3006
        || codepoint == 0x3007
        || codepoint == 0x3008
        || codepoint == 0x3009
        || codepoint == 0x300A
        || codepoint == 0x300B
        || codepoint == 0x300C
        || codepoint == 0x300D
        || codepoint == 0x300E
        || codepoint == 0x300F
        || codepoint == 0x3010
        || codepoint == 0x3011
        || (0x3012 <= codepoint && codepoint <= 0x3013)
        || codepoint == 0x3014
        || codepoint == 0x3015
        || codepoint == 0x3016
        || codepoint == 0x3017
        || codepoint == 0x3018
        || codepoint == 0x3019
        || codepoint == 0x301A
        || codepoint == 0x301B
        || codepoint == 0x301C
        || codepoint == 0x301D
        || (0x301E <= codepoint && codepoint <= 0x301F)
        || codepoint == 0x3020
        || (0x3021 <= codepoint && codepoint <= 0x3029)
        || (0x302A <= codepoint && codepoint <= 0x302D)
        || (0x302E <= codepoint && codepoint <= 0x302F)
        || codepoint == 0x3030
        || (0x3031 <= codepoint && codepoint <= 0x3035)
        || (0x3036 <= codepoint && codepoint <= 0x3037)
        || (0x3038 <= codepoint && codepoint <= 0x303A)
        || codepoint == 0x303B
        || codepoint == 0x303C
        || codepoint == 0x303D
        || codepoint == 0x303E
        || (0x3041 <= codepoint && codepoint <= 0x3096)
        || (0x3099 <= codepoint && codepoint <= 0x309A)
        || (0x309B <= codepoint && codepoint <= 0x309C)
        || (0x309D <= codepoint && codepoint <= 0x309E)
        || codepoint == 0x309F
        || codepoint == 0x30A0
        || (0x30A1 <= codepoint && codepoint <= 0x30FA)
        || codepoint == 0x30FB
        || (0x30FC <= codepoint && codepoint <= 0x30FE)
        || codepoint == 0x30FF
        || (0x3105 <= codepoint && codepoint <= 0x312F)
        || (0x3131 <= codepoint && codepoint <= 0x318E)
        || (0x3190 <= codepoint && codepoint <= 0x3191)
        || (0x3192 <= codepoint && codepoint <= 0x3195)
        || (0x3196 <= codepoint && codepoint <= 0x319F)
        || (0x31A0 <= codepoint && codepoint <= 0x31BF)
        || (0x31C0 <= codepoint && codepoint <= 0x31E5)
        || codepoint == 0x31EF
        || (0x31F0 <= codepoint && codepoint <= 0x31FF)
        || (0x3200 <= codepoint && codepoint <= 0x321E)
        || (0x3220 <= codepoint && codepoint <= 0x3229)
        || (0x322A <= codepoint && codepoint <= 0x3247)
        || codepoint == 0x3250
        || (0x3251 <= codepoint && codepoint <= 0x325F)
        || (0x3260 <= codepoint && codepoint <= 0x327F)
        || (0x3280 <= codepoint && codepoint <= 0x3289)
        || (0x328A <= codepoint && codepoint <= 0x32B0)
        || (0x32B1 <= codepoint && codepoint <= 0x32BF)
        || (0x32C0 <= codepoint && codepoint <= 0x32FF)
        || (0x3300 <= codepoint && codepoint <= 0x33FF)
        || (0x3400 <= codepoint && codepoint <= 0x4DBF)
        || (0x4DC0 <= codepoint && codepoint <= 0x4DFF)
        || (0x4E00 <= codepoint && codepoint <= 0x9FFF)
        || (0xA000 <= codepoint && codepoint <= 0xA014)
        || codepoint == 0xA015
        || (0xA016 <= codepoint && codepoint <= 0xA48C)
        || (0xA490 <= codepoint && codepoint <= 0xA4C6)
        || (0xA960 <= codepoint && codepoint <= 0xA97C)
        || (0xAC00 <= codepoint && codepoint <= 0xD7A3)
        || (0xF900 <= codepoint && codepoint <= 0xFA6D)
        || (0xFA6E <= codepoint && codepoint <= 0xFA6F)
        || (0xFA70 <= codepoint && codepoint <= 0xFAD9)
        || (0xFADA <= codepoint && codepoint <= 0xFAFF)
        || (0xFE10 <= codepoint && codepoint <= 0xFE16)
        || codepoint == 0xFE17
        || codepoint == 0xFE18
        || codepoint == 0xFE19
        || codepoint == 0xFE30
        || (0xFE31 <= codepoint && codepoint <= 0xFE32)
        || (0xFE33 <= codepoint && codepoint <= 0xFE34)
        || codepoint == 0xFE35
        || codepoint == 0xFE36
        || codepoint == 0xFE37
        || codepoint == 0xFE38
        || codepoint == 0xFE39
        || codepoint == 0xFE3A
        || codepoint == 0xFE3B
        || codepoint == 0xFE3C
        || codepoint == 0xFE3D
        || codepoint == 0xFE3E
        || codepoint == 0xFE3F
        || codepoint == 0xFE40
        || codepoint == 0xFE41
        || codepoint == 0xFE42
        || codepoint == 0xFE43
        || codepoint == 0xFE44
        || (0xFE45 <= codepoint && codepoint <= 0xFE46)
        || codepoint == 0xFE47
        || codepoint == 0xFE48
        || (0xFE49 <= codepoint && codepoint <= 0xFE4C)
        || (0xFE4D <= codepoint && codepoint <= 0xFE4F)
        || (0xFE50 <= codepoint && codepoint <= 0xFE52)
        || (0xFE54 <= codepoint && codepoint <= 0xFE57)
        || codepoint == 0xFE58
        || codepoint == 0xFE59
        || codepoint == 0xFE5A
        || codepoint == 0xFE5B
        || codepoint == 0xFE5C
        || codepoint == 0xFE5D
        || codepoint == 0xFE5E
        || (0xFE5F <= codepoint && codepoint <= 0xFE61)
        || codepoint == 0xFE62
        || codepoint == 0xFE63
        || (0xFE64 <= codepoint && codepoint <= 0xFE66)
        || codepoint == 0xFE68
        || codepoint == 0xFE69
        || (0xFE6A <= codepoint && codepoint <= 0xFE6B)
        || (0xFF01 <= codepoint && codepoint <= 0xFF03)
        || codepoint == 0xFF04
        || (0xFF05 <= codepoint && codepoint <= 0xFF07)
        || codepoint == 0xFF08
        || codepoint == 0xFF09
        || codepoint == 0xFF0A
        || codepoint == 0xFF0B
        || codepoint == 0xFF0C
        || codepoint == 0xFF0D
        || (0xFF0E <= codepoint && codepoint <= 0xFF0F)
        || (0xFF10 <= codepoint && codepoint <= 0xFF19)
        || (0xFF1A <= codepoint && codepoint <= 0xFF1B)
        || (0xFF1C <= codepoint && codepoint <= 0xFF1E)
        || (0xFF1F <= codepoint && codepoint <= 0xFF20)
        || (0xFF21 <= codepoint && codepoint <= 0xFF3A)
        || codepoint == 0xFF3B
        || codepoint == 0xFF3C
        || codepoint == 0xFF3D
        || codepoint == 0xFF3E
        || codepoint == 0xFF3F
        || codepoint == 0xFF40
        || (0xFF41 <= codepoint && codepoint <= 0xFF5A)
        || codepoint == 0xFF5B
        || codepoint == 0xFF5C
        || codepoint == 0xFF5D
        || codepoint == 0xFF5E
        || codepoint == 0xFF5F
        || codepoint == 0xFF60
        || (0xFFE0 <= codepoint && codepoint <= 0xFFE1)
        || codepoint == 0xFFE2
        || codepoint == 0xFFE3
        || codepoint == 0xFFE4
        || (0xFFE5 <= codepoint && codepoint <= 0xFFE6)
        || (0x16FE0 <= codepoint && codepoint <= 0x16FE1)
        || codepoint == 0x16FE2
        || codepoint == 0x16FE3
        || codepoint == 0x16FE4
        || (0x16FF0 <= codepoint && codepoint <= 0x16FF1)
        || (0x16FF2 <= codepoint && codepoint <= 0x16FF3)
        || (0x16FF4 <= codepoint && codepoint <= 0x16FF6)
        || (0x17000 <= codepoint && codepoint <= 0x187FF)
        || (0x18800 <= codepoint && codepoint <= 0x18AFF)
        || (0x18B00 <= codepoint && codepoint <= 0x18CD5)
        || codepoint == 0x18CFF
        || (0x18D00 <= codepoint && codepoint <= 0x18D1E)
        || (0x18D80 <= codepoint && codepoint <= 0x18DF2)
        || (0x1AFF0 <= codepoint && codepoint <= 0x1AFF3)
        || (0x1AFF5 <= codepoint && codepoint <= 0x1AFFB)
        || (0x1AFFD <= codepoint && codepoint <= 0x1AFFE)
        || (0x1B000 <= codepoint && codepoint <= 0x1B0FF)
        || (0x1B100 <= codepoint && codepoint <= 0x1B122)
        || codepoint == 0x1B132
        || (0x1B150 <= codepoint && codepoint <= 0x1B152)
        || codepoint == 0x1B155
        || (0x1B164 <= codepoint && codepoint <= 0x1B167)
        || (0x1B170 <= codepoint && codepoint <= 0x1B2FB)
        || (0x1D300 <= codepoint && codepoint <= 0x1D356)
        || (0x1D360 <= codepoint && codepoint <= 0x1D376)
        || codepoint == 0x1F004
        || codepoint == 0x1F0CF
        || codepoint == 0x1F18E
        || (0x1F191 <= codepoint && codepoint <= 0x1F19A)
        || (0x1F200 <= codepoint && codepoint <= 0x1F202)
        || (0x1F210 <= codepoint && codepoint <= 0x1F23B)
        || (0x1F240 <= codepoint && codepoint <= 0x1F248)
        || (0x1F250 <= codepoint && codepoint <= 0x1F251)
        || (0x1F260 <= codepoint && codepoint <= 0x1F265)
        || (0x1F300 <= codepoint && codepoint <= 0x1F320)
        || (0x1F32D <= codepoint && codepoint <= 0x1F335)
        || (0x1F337 <= codepoint && codepoint <= 0x1F37C)
        || (0x1F37E <= codepoint && codepoint <= 0x1F393)
        || (0x1F3A0 <= codepoint && codepoint <= 0x1F3CA)
        || (0x1F3CF <= codepoint && codepoint <= 0x1F3D3)
        || (0x1F3E0 <= codepoint && codepoint <= 0x1F3F0)
        || codepoint == 0x1F3F4
        || (0x1F3F8 <= codepoint && codepoint <= 0x1F3FA)
        || (0x1F3FB <= codepoint && codepoint <= 0x1F3FF)
        || (0x1F400 <= codepoint && codepoint <= 0x1F43E)
        || codepoint == 0x1F440
        || (0x1F442 <= codepoint && codepoint <= 0x1F4FC)
        || (0x1F4FF <= codepoint && codepoint <= 0x1F53D)
        || (0x1F54B <= codepoint && codepoint <= 0x1F54E)
        || (0x1F550 <= codepoint && codepoint <= 0x1F567)
        || codepoint == 0x1F57A
        || (0x1F595 <= codepoint && codepoint <= 0x1F596)
        || codepoint == 0x1F5A4
        || (0x1F5FB <= codepoint && codepoint <= 0x1F5FF)
        || (0x1F600 <= codepoint && codepoint <= 0x1F64F)
        || (0x1F680 <= codepoint && codepoint <= 0x1F6C5)
        || codepoint == 0x1F6CC
        || (0x1F6D0 <= codepoint && codepoint <= 0x1F6D2)
        || (0x1F6D5 <= codepoint && codepoint <= 0x1F6D8)
        || (0x1F6DC <= codepoint && codepoint <= 0x1F6DF)
        || (0x1F6EB <= codepoint && codepoint <= 0x1F6EC)
        || (0x1F6F4 <= codepoint && codepoint <= 0x1F6FC)
        || (0x1F7E0 <= codepoint && codepoint <= 0x1F7EB)
        || codepoint == 0x1F7F0
        || (0x1F90C <= codepoint && codepoint <= 0x1F93A)
        || (0x1F93C <= codepoint && codepoint <= 0x1F945)
        || (0x1F947 <= codepoint && codepoint <= 0x1F9FF)
        || (0x1FA70 <= codepoint && codepoint <= 0x1FA7C)
        || (0x1FA80 <= codepoint && codepoint <= 0x1FA8A)
        || (0x1FA8E <= codepoint && codepoint <= 0x1FAC6)
        || codepoint == 0x1FAC8
        || (0x1FACD <= codepoint && codepoint <= 0x1FADC)
        || (0x1FADF <= codepoint && codepoint <= 0x1FAEA)
        || (0x1FAEF <= codepoint && codepoint <= 0x1FAF8)
        || (0x20000 <= codepoint && codepoint <= 0x2A6DF)
        || (0x2A6E0 <= codepoint && codepoint <= 0x2A6FF)
        || (0x2A700 <= codepoint && codepoint <= 0x2B81D)
        || (0x2B81E <= codepoint && codepoint <= 0x2B81F)
        || (0x2B820 <= codepoint && codepoint <= 0x2CEAD)
        || (0x2CEAE <= codepoint && codepoint <= 0x2CEAF)
        || (0x2CEB0 <= codepoint && codepoint <= 0x2EBE0)
        || (0x2EBE1 <= codepoint && codepoint <= 0x2EBEF)
        || (0x2EBF0 <= codepoint && codepoint <= 0x2EE5D)
        || (0x2EE5E <= codepoint && codepoint <= 0x2F7FF)
        || (0x2F800 <= codepoint && codepoint <= 0x2FA1D)
        || (0x2FA1E <= codepoint && codepoint <= 0x2FA1F)
        || (0x2FA20 <= codepoint && codepoint <= 0x2FFFD)
        || (0x30000 <= codepoint && codepoint <= 0x3134A)
        || (0x3134B <= codepoint && codepoint <= 0x3134F)
        || (0x31350 <= codepoint && codepoint <= 0x33479)
        || (0x3347A <= codepoint && codepoint <= 0x3FFFD)
    ) {
        return true;
    }
    return false;
}


#endif // UNICODE_EAW
