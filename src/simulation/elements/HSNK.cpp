#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_HSNK()
{
	Identifier = "DEFAULT_PT_HSNK";
	Name = "HSNK";
	Colour = 0xA8B8C8_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 255;       // exchanges heat with neighbours every frame
	HeatCapacity = 4.0f;     // soaks up heat quickly without its own temperature jumping
	Description = "Heatsink. Absorbs heat and holds itself at 25C, removing up to TMP degrees per frame per pixel (default 10).";

	Properties = TYPE_SOLID|PROP_CONDUCTS|PROP_HOT_GLOW|PROP_LIFE_DEC;

	DefaultProperties.temp = 25.0f + 273.15f;
	DefaultProperties.tmp = 10;  // cooling limit, C per frame per pixel (set with PROP tool)

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 5500.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA; //@ HSNK -> LAVA(HSNK)

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	constexpr float target = 25.0f + 273.15f;
	auto limit = float(std::max(parts[i].tmp, 0));
	auto diff = std::clamp(parts[i].temp - target, -limit, limit);
	parts[i].temp -= diff; // pull toward 25C, but never faster than the limit
	return 0;
}
