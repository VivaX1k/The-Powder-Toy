#pragma once

// Shared numbers for HALN (gas) and LHLN (liquid halon)
constexpr int HALN_EXPANSION = 4;                       // 1 liquid particle <-> up to 4 gas particles
constexpr float HALN_LIQUEFY_PRESSURE = 25.0f;          // gas condenses at or above this pressure...
constexpr float HALN_LIQUID_MIN_PRESSURE = 20.0f;       // ...and the liquid flashes back at or below this
constexpr float HALN_CRITICAL_TEMP = 67.0f + 273.15f;   // above this it cannot be liquid, whatever the pressure
#ifndef HALN_SINK_EVERY
#define HALN_SINK_EVERY 3                               // sinks one pixel on average every this many frames
#endif
constexpr int HALN_LIFETIME = 600;                      // frames a HALN gas particle lasts (life 0 = never expires)

#include "simulation/ElementCommon.h"

// Release HALN gas that particle i is still holding (parts[i].tmp) into free space around it.
// Gas that has no room yet stays held and comes out on later frames, so no halon is ever lost.
inline void Element_HALN_release(Simulation *sim, int i, int x, int y)
{
	auto &parts = sim->parts;
	for (int ry = -1; ry <= 1 && parts[i].tmp > 0; ry++)
	{
		for (int rx = -1; rx <= 1 && parts[i].tmp > 0; rx++)
		{
			if (!rx && !ry)
				continue;
			int np = sim->create_part(-1, x+rx, y+ry, PT_HALN);
			if (np >= 0)
			{
				parts[np].temp = parts[i].temp;
				parts[np].tmp = 0;
				parts[i].tmp--;
				sim->pv[(y+ry)/CELL][(x+rx)/CELL] += 0.5f; // expansion push, same as water flashing into steam
			}
		}
	}
	// buried inside a packed gas cloud with no room: pass the held gas up to the HALN above
	// (like bubbles rising), so it reaches the top of the cloud quickly and is released there
	if (parts[i].tmp > 0)
	{
		int rx = sim->rng.between(-1, 1);
		auto r = sim->pmap[y-1][x+rx];
		if (TYP(r) != PT_HALN) // blocked above: try any neighbour instead
		{
			rx = sim->rng.between(-1, 1);
			r = sim->pmap[y+sim->rng.between(-1, 1)][x+rx];
		}
		if (TYP(r) == PT_HALN && ID(r) != i)
		{
			parts[ID(r)].tmp += parts[i].tmp;
			parts[i].tmp = 0;
		}
	}
}
