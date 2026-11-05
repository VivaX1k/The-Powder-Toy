#include "simulation/ElementCommon.h"
#include "HALN.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_HALN()
{
	Identifier = "DEFAULT_PT_HALN";
	Name = "HALN";
	Colour = 0xC8E0B0_rgb;
	MenuVisible = 1;
	MenuSection = SC_GAS;
	Enabled = 1;

	Advection = 0.5f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.1f;
	Gravity = 0.1f;      // the real sinking is done in update(), one pixel at a time
	Diffusion = 0.75f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;         // moves like a gas, not a powder

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 29;         // just under WATR (30): pushes other gases and FIRE aside, rests on water

	HeatConduct = 4;     // poor heat conductor, does not carry fire heat to fuel
	Description = "Halon. Very heavy, inert gas that sinks and pools. Smothers fire. Liquefies above 25 pressure when below 67C. Gas lasts 600 frames (set LIFE to 0 to keep it).";

	Properties = TYPE_GAS | PROP_LIFE_DEC | PROP_LIFE_KILL_DEC; // disappears when its life runs out
	DefaultProperties.life = HALN_LIFETIME;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;
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

	// heavy gas: sink at most one pixel per frame. Moving it here (instead of with strong gravity)
	// keeps it a normal drifting gas, stops it tunnelling through thin walls, and means hot
	// updrafts above a fire cannot hold it up.
	if (sim->gravityMode == GRAV_VERTICAL && sim->rng.chance(1, HALN_SINK_EVERY))
	{
		int dx = sim->rng.between(-1, 1);
		if (!sim->do_move(i, x, y, float(x + dx), float(y + 1)) && dx)
			sim->do_move(i, x, y, float(x), float(y + 1));
		x = int(parts[i].x + 0.5f);
		y = int(parts[i].y + 0.5f);
	}

	// smother any flame touching the halon; the halon itself is never used up
	// (reaches 2 pixels, since a gas cloud is not packed solid)
	for (auto rx = -2; rx <= 2; rx++)
	{
		for (auto ry = -2; ry <= 2; ry++)
		{
			if ((!rx && !ry) || x+rx < 0 || y+ry < 0 || x+rx >= XRES || y+ry >= YRES)
				continue;
			auto r = pmap[y+ry][x+rx];
			auto rt = TYP(r);
			if (rt == PT_FIRE || rt == PT_CFLM)
				sim->kill_part(ID(r));
		}
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	// fade out over the last 100 frames of its life
	if (cpart->life > 0 && cpart->life < 100)
	{
		float f = 0.25f + 0.75f * cpart->life / 100.0f;
		*colr = int(*colr * f);
		*colg = int(*colg * f);
		*colb = int(*colb * f);
	}
	return 0;
}
