#include "simulation/ElementCommon.h"
#include <vector>

int Element_TRNS_update(UPDATE_FUNC_ARGS);
int Element_TRNS_graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_TRNS()
{
	Identifier = "DEFAULT_PT_TRNS";
	Name = "TRNS";
	Colour = 0x6B4E16_rgb;
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
	Description = "Transistor. Passes INST signals only while it is being powered by sparking PSCN.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_TRNS_update;
	Graphics = &Element_TRNS_graphics;
}

// A powered PSCN pulses every 8 frames (4 frames sparked, 4 frames resting),
// so the gate has to hold for a few frames to bridge the gaps between pulses.
// Once the PSCN stops sparking, the gate drops within this many frames.
constexpr int GATE_HOLD = 6;

// Power the whole connected blob of this transistor type at once,
// so the gate PSCN can be placed away from the INST signal wires.
static void powerBlob(Simulation *sim, int x, int y, int type)
{
	auto &parts = sim->parts;
	std::vector<std::pair<int, int>> stack{ { x, y } };
	parts[ID(sim->pmap[y][x])].life = GATE_HOLD;
	while (!stack.empty())
	{
		auto [cx, cy] = stack.back();
		stack.pop_back();
		for (int rx = -1; rx <= 1; rx++)
		{
			for (int ry = -1; ry <= 1; ry++)
			{
				int nx = cx + rx, ny = cy + ry;
				if ((!rx && !ry) || nx < 0 || ny < 0 || nx >= XRES || ny >= YRES)
					continue;
				auto r = sim->pmap[ny][nx];
				if (TYP(r) == type && parts[ID(r)].life < GATE_HOLD)
				{
					parts[ID(r)].life = GATE_HOLD;
					stack.push_back({ nx, ny });
				}
			}
		}
	}
}

int Element_TRNS_update(UPDATE_FUNC_ARGS)
{
	auto type = parts[i].type;
	if (parts[i].life > 0)
		parts[i].life--;

	bool instSparked = false;
	for (int rx = -1; rx <= 1; rx++)
	{
		for (int ry = -1; ry <= 1; ry++)
		{
			auto r = pmap[y+ry][x+rx];
			if ((!rx && !ry) || TYP(r) != PT_SPRK)
				continue;
			auto ct = parts[ID(r)].ctype;
			if (ct == PT_PSCN && parts[i].life < GATE_HOLD)
				powerBlob(sim, x, y, type); // gate input
			else if (ct == PT_INST)
				instSparked = true;
		}
	}

	bool powered = parts[i].life > 0;
	bool conducting = (type == PT_TRNS) ? powered : !powered; // NTRS is inverted
	if (conducting && instSparked)
	{
		// pass the INST signal on to every idle INST wire touching this pixel
		for (int rx = -1; rx <= 1; rx++)
		{
			for (int ry = -1; ry <= 1; ry++)
			{
				auto r = pmap[y+ry][x+rx];
				if ((rx || ry) && TYP(r) == PT_INST && parts[ID(r)].life == 0)
					sim->FloodINST(x+rx, y+ry);
			}
		}
	}
	return 0;
}

int Element_TRNS_graphics(GRAPHICS_FUNC_ARGS)
{
	bool powered = cpart->life > 0;
	bool conducting = (cpart->type == PT_TRNS) ? powered : !powered;
	if (conducting)
	{
		*colr = std::min(*colr * 2 + 40, 255);
		*colg = std::min(*colg * 2 + 40, 255);
		*colb = std::min(*colb * 2 + 40, 255);
		*pixel_mode |= PMODE_GLOW;
	}
	return 0;
}
