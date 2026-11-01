#include "simulation/ElementCommon.h"

int Element_TTAN_blockAir(UPDATE_FUNC_ARGS);

void Element::Element_STEL()
{
	Identifier = "DEFAULT_PT_STEL";
	Name = "STEL";
	Colour = 0x7C8794_rgb;
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
	Hardness = 48;

	Weight = 100;

	HeatConduct = 251;
	Description = "Steel. Conducts electricity, blocks all air pressure, melts at 5500C.";

	Properties = TYPE_SOLID|PROP_CONDUCTS|PROP_HOT_GLOW|PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 5500.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA; //@ STEL -> LAVA(STEL)

	Update = &Element_TTAN_blockAir;
}
