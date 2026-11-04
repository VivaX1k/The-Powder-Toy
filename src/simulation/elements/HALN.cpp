#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_HALN()
{
	Identifier = "DEFAULT_PT_HALN";
	Name = "HALN";
	Colour = 0xC8E0B0_rgb;
	MenuVisible = 1;
	MenuSection = SC_GAS;
	Enabled = 1;

	Advection = 0.15f;   // barely carried by air currents, so hot updrafts cannot lift it
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.1f;
	Gravity = 0.4f;      // sinks much harder than CO2 (0.1)
	Diffusion = 0.4f;
	HotAir = 0.000f	* CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 29;         // just under WATR (30): pushes other gases and FIRE aside, rests on water

	HeatConduct = 4;     // poor heat conductor, does not carry fire heat to fuel
	Description = "Halon. Very heavy, inert gas that sinks and pools. Smothers fire. Never reacts or breaks down.";

	Properties = TYPE_GAS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	// smother any flame touching the halon; the halon itself is never used up
	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (!rx && !ry)
				continue;
			auto r = pmap[y+ry][x+rx];
			auto rt = TYP(r);
			if (rt == PT_FIRE || rt == PT_CFLM)
				sim->kill_part(ID(r));
		}
	}
	return 0;
}
