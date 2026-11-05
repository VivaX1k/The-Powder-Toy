#include "simulation/ElementCommon.h"
#include "HALN.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_LHLN()
{
	Identifier = "DEFAULT_PT_LHLN";
	Name = "LHLN";
	Colour = 0x9CD070_rgb;
	MenuVisible = 1;
	MenuSection = SC_LIQUID;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 35;         // denser than water, like real liquid halon

	HeatConduct = 20;
	Description = "Liquid halon. Only stays liquid under high pressure (above 20). Flashes into expanding HALN gas when released or heated.";

	Properties = TYPE_LIQUID;

	DefaultProperties.tmp = HALN_EXPANSION; // how many HALN particles this liquid turns back into

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

static void vaporize(Simulation *sim, int i, int x, int y)
{
	auto &parts = sim->parts;
	int amount = std::max(parts[i].tmp, 1);
	sim->part_change_type(i, x, y, PT_HALN);
	parts[i].tmp = amount - 1; // the rest of the gas, still to be released
	sim->pv[y/CELL][x/CELL] += 0.5f;
	Element_HALN_release(sim, i, x, y);
}

static int update(UPDATE_FUNC_ARGS)
{
	bool boil = sim->pv[y/CELL][x/CELL] <= HALN_LIQUID_MIN_PRESSURE || parts[i].temp >= HALN_CRITICAL_TEMP;
	for (int rx = -1; rx <= 1 && !boil; rx++)
	{
		for (int ry = -1; ry <= 1 && !boil; ry++)
		{
			auto r = pmap[y+ry][x+rx];
			if ((!rx && !ry) || !r)
				continue;
			auto rt = TYP(r);
			// touching flame or anything hot flashes it into gas instantly
			if (rt == PT_FIRE || rt == PT_PLSM || rt == PT_LAVA || rt == PT_CFLM || parts[ID(r)].temp >= HALN_CRITICAL_TEMP)
				boil = true;
		}
	}
	if (boil)
	{
		vaporize(sim, i, x, y);
		return 1;
	}
	return 0;
}
