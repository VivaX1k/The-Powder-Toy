#include "simulation/ElementCommon.h"

int Element_TRNS_update(UPDATE_FUNC_ARGS);
int Element_TRNS_graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_NTRS()
{
	Identifier = "DEFAULT_PT_NTRS";
	Name = "NTRS";
	Colour = 0x4A1E5C_rgb;
	MenuVisible = 1;
	MenuSection = SC_ELEC;
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
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "NOT transistor. Passes INST signals, but blocks them while powered by sparking PSCN.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_TRNS_update; // shared with TRNS, inverted by type
	Graphics = &Element_TRNS_graphics;
}
