#include "simulation/ElementCommon.h"
#include "HALN.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_HALN()
{
	Identifier = "DEFAULT_PT_HALN";
	Name = "HALN";
	Colour = 0xC8E0B0_rgb;
	MenuVisible = 1;
	MenuSection = SC_GAS;
	Enabled = 1;

	Advection = 0.1f;    // barely carried by air currents, so hot updrafts cannot lift it
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.80f;        // keeps its falling speed, so it pours down onto fires
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
	Description = "Halon. Very heavy, inert gas that sinks and pools. Smothers fire. Liquefies above 25 pressure when below 67C.";

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
	// compressed enough (and cool enough): condense into liquid halon,
	// pulling in nearby gas so the liquid takes up less room than the gas did
	if (sim->pv[y/CELL][x/CELL] >= HALN_LIQUEFY_PRESSURE && parts[i].temp < HALN_CRITICAL_TEMP)
	{
		int amount = 1 + std::max(parts[i].tmp, 0); // include gas it was still holding
		for (auto rx = -1; rx <= 1 && amount < HALN_EXPANSION; rx++)
		{
			for (auto ry = -1; ry <= 1 && amount < HALN_EXPANSION; ry++)
			{
				auto r = pmap[y+ry][x+rx];
				if ((rx || ry) && TYP(r) == PT_HALN)
				{
					amount += 1 + std::max(parts[ID(r)].tmp, 0); // keep any gas it was still holding
					sim->kill_part(ID(r));
				}
			}
		}
		sim->part_change_type(i, x, y, PT_LHLN);
		parts[i].tmp = amount;
		parts[i].vx = parts[i].vy = 0;
		return 1;
	}

	// gas left over from liquid that flashed with no room around it: let it out as space frees up
	if (parts[i].tmp > 0)
		Element_HALN_release(sim, i, x, y);

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
