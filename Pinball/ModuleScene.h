#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "ModulePhysics.h"

#define START_BALL_POSITION_X 338
#define START_BALL_POSITION_Y 400
#define BALL_SIZE 6


class PhysBody;
struct b2RevoluteJoint;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void InitializeSceneColliders();
	void initializeFlippers();

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	p2List<PhysBody*> board;

	PhysBody* triangle1;
	PhysBody* ball;

	//flippers
	PhysBody* left_flipper_anchor;
	PhysBody* left_flipper;
	PhysBody* right_flipper_anchor;
	PhysBody* right_flipper;

	//b2RevoluteJoint* left_flipper_joint = nullptr;
	b2RevoluteJointDef left_flipper_joint;
	b2RevoluteJointDef right_flipper_joint;

	PhysBody* sensor;
	bool sensed;

	//sprites
	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* board_tex;
	SDL_Texture* flipper_tex;
	SDL_Texture* spritesheet;

	//fx
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
};
