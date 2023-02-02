#pragma once
#include <bitset>
// A simple type alias
using Entity = std::uint32_t;

// Used to define the size of arrays later on
const Entity MAX_ENTITIES = 500;
// A simple type alias
using ComponentType = std::uint8_t;

// Used to define the size of arrays later on
const ComponentType MAX_COMPONENTS = 32;
// A simple type alias
using Signature = std::bitset<MAX_COMPONENTS>;

#define ScreenW  800
#define ScreenH  800

#define RenderW  ScreenW-1
#define RenderH  ScreenH-1

#define HalfScreenW  ScreenW*0.5f
#define HalfScreenH  ScreenH*0.5f

#define ScreenW2  1.0f/(HalfScreenW)
#define ScreenH2  1.0f/(HalfScreenH)
