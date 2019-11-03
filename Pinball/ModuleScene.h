#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "ModulePhysics.h"
#include "Animation.h"
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
	void NotOnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void InitializeSceneColliders();
	void initializeInteractiveElements();
	void TranslateBall(int x, int y, b2Vec2 speed);

public:

	int points = 0;
	int high_score = 0;
	int lives = 3;
	b2Vec2 initial_position;
	int new_ball_x;
	int new_ball_y;
	b2Vec2 new_ball_speed;

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

	//interactive statics
	PhysBody* pan1;
	PhysBody* pan2;
	PhysBody* pan3;
	PhysBody* left_triangle;
	PhysBody* right_triangle;
	PhysBody* left_L;
	PhysBody* right_L;
	PhysBody* left_capsule;
	PhysBody* right_capsule;
	PhysBody* right_bar;
	PhysBody* left_wood;
	PhysBody* right_wood;

	//b2RevoluteJoint* left_flipper_joint = nullptr;
	b2RevoluteJointDef left_flipper_joint;
	b2RevoluteJointDef right_flipper_joint;

	PhysBody* kicker;
	PhysBody* static_kicker;
	int kicker_y;

	b2PrismaticJointDef kicker_joint;

	//sensors
	PhysBody* bottom_sensor;
	PhysBody* drain_sensor;
	PhysBody* circle_sensor1;
	PhysBody* stove_1_sensor;
	PhysBody* stove_2_sensor;
	PhysBody* restart_sensor;
	PhysBody* left_triangle_bouncer;
	PhysBody* right_triangle_bouncer;
	bool sensed;

	//sprites
	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* board_tex;
	SDL_Texture* flipper_tex;
	SDL_Texture* spritesheet;
	SDL_Texture* letters;
	SDL_Texture* letters_2;
		
	//fx
	bool playing_left_flipper_fx;
	bool playing_right_flipper_fx;
	uint bonus_fx;
	uint pan_fx;
	uint triangle_fx;
	uint ball_lost_fx;
	uint flipper_fx;
	uint ring_fx;
	uint drain_fx;
	uint capsule_fx;
	uint stove_2_fx;
	uint ball_release_fx;

	int stove_1_time = 2;
	int stove_2_time = 2;
	bool sensor_holding = false;
	int sensor_contact_moment = 0;
	int time_between_holds = 2;
	int last_time_hold = 0;

	int Point_numbers = 0;
	char point_text[10];
	int player_point = 0;

	PhysBody* center_body = nullptr;

	bool show_left_light = false;
	bool show_right_light = false;

	p2Point<int> ray;
	bool ray_on;
	bool change_ball_position = false;
	bool game_over = false;
};
