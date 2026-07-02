/*
 * VT100CtrlCode.h
 *
 *  Created on: 2015/3/27
 *      Author: ChiaYen
 */

#ifndef VT100CTRLCODE_H_
#define VT100CTRLCODE_H_

// 16 Color
// Foreground
#define ForegroundDefault         "\x1B[39m"
#define ForegroundBlack           "\x1B[30m"
#define ForegroundRed             "\x1B[31m"
#define ForegroundGreen           "\x1B[32m"
#define ForegroundYellow          "\x1B[33m"
#define ForegroundBlue            "\x1B[34m"
#define ForegroundMagenta         "\x1B[35m"
#define ForegroundCyan            "\x1B[36m"
#define ForegroundLightGray       "\x1B[37m"
#define ForegroundDarkGray        "\x1B[90m"
#define ForegroundLightRed        "\x1B[91m"
#define ForegroundLightGreen      "\x1B[92m"
#define ForegroundLightYellow     "\x1B[93m"
#define ForegroundLightBlue       "\x1B[94m"
#define ForegroundLightMagenta    "\x1B[95m"
#define ForegroundLightCyan       "\x1B[96m"
#define ForegroundWhite           "\x1B[97m"
// Background
#define BackgroundDefault         "\x1B[49m"
#define BackgroundBlack           "\x1B[40m"
#define BackgroundRed             "\x1B[41m"
#define BackgroundGreen           "\x1B[42m"
#define BackgroundYellow          "\x1B[43m"
#define BackgroundBlue            "\x1B[44m"
#define BackgroundMagenta         "\x1B[45m"
#define BackgroundCyan            "\x1B[46m"
#define BackgroundLightGray       "\x1B[47m"
#define BackgroundDarkGray        "\x1B[100m"
#define BackgroundLightRed        "\x1B[101m"
#define BackgroundLightGreen      "\x1B[102m"
#define BackgroundLightYellow     "\x1B[103m"
#define BackgroundLightBlue       "\x1B[104m"
#define BackgroundLightMagenta    "\x1B[105m"
#define BackgroundLightCyan       "\x1B[106m"
#define BackgroundWhite           "\x1B[107m"

// Reset
#define ResetAll                  "\x1B[0m"

// 256 Color
// Foreground
#define Foreground256(color)     "\x1B[38;5;"#color"m"
// Background
#define Background256(color)     "\x1B[48;5;"#color"m"

// Clear screen
#define ClearScreen              "\x1B[2J"

// Move Cursor to Home
#define Cursor2Home              "\x1B[f"

#endif /* VT100CTRLCODE_H_ */

