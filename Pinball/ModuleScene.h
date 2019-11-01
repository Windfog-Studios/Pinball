#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

#define START_BALL_POSITION_X 338
#define START_BALL_POSITION_Y 400
#define BALL_SIZE 7


class PhysBody;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	p2List<PhysBody*> board;

	PhysBody* triangle1;

	//bumpers
	PhysBody* left_bumper_anchor;
	PhysBody* left_bumper;
	PhysBody* right_bumper_anchor;
	PhysBody* right_bumper;

	PhysBody* sensor;
	bool sensed;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* board_tex;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
};