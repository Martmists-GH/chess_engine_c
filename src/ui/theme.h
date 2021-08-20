//
// Created by mart on 8/16/21.
//

#pragma once

#define SK_GL
#include <include/core/SkColor.h>


typedef struct {
    SkColor4f background;
    SkColor4f backgroundAlt;
    SkColor4f highlight;
    SkColor4f highlightAlt;
    SkColor4f border;
    SkColor4f meta;
    SkColor4f overlay;
} Theme;

static Theme DEFAULT_THEME = {
    SkColor4f::FromColor(0xFF769656),
    SkColor4f::FromColor(0xFFEEEED2),
    SkColor4f::FromColor(0xFFBACA2B),
    SkColor4f::FromColor(0xFFF6F669),
    SkColor4f::FromColor(0xFFFF0000),
    SkColor4f::FromColor(0xA0505050),
    SkColor4f::FromColor(0xEE333333)
};
