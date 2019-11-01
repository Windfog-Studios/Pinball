#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//sprites

	circle = App->textures->Load("assets/wheel.png"); 
	board_tex = App->textures->Load("assets/sprites/Rat_and_roll_board.png");


	//sounds
	bonus_fx = App->audio->LoadFx("assets/bonus.wav");

	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);
	
	boxes.add(App->physics->CreateRectangle(345, 430, 50, 50));

	InitializeSceneColliders();
	
	initializeFlippers();

	//player ball
	ball = App->physics->CreateCircle(336, 400, BALL_SIZE);


	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	//App->physics->world->DestroyJoint(left_revolute_joint);
	//left_revolute_joint = NULL;
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleScene::Update()
{

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(START_BALL_POSITION_X, START_BALL_POSITION_Y, BALL_SIZE));
		circles.getLast()->data->listener = this;
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64));
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) left_flipper_joint;

	//LOG("motor speed %.2f", left_flipper_joint->GetJointSpeed());

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	//draw scene

	//App->renderer->Blit(board_tex, 0, 0);

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
			App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	return UPDATE_CONTINUE;
}

void ModuleScene::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(bonus_fx);

	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}

void ModuleScene::InitializeSceneColliders() {
	//collider points
	int board_points[58] = {
		1, 597,
		1, 2,
		355, 2,
		356, 596,
		344, 596,
		345, 154,
		319, 76,
		280, 40,
		253, 30,
		223, 20,
		203, 20,
		173, 20,
		150, 20,
		120, 30,
		99, 35,
		68, 57,
		39, 97,
		23, 138,
		18, 193,
		18, 236,
		26, 276,
		40, 318,
		61, 344,
		64, 360,
		60, 370,
		12, 447,
		11, 538,
		104, 585,
		104, 599
	};
	int triangle_1_points[10] = {
	65, 462,
	100, 519,
	98, 526,
	56, 508,
	57, 467
	};
	int triangle_2_points[10] = {
	256, 464,
	264, 465,
	264, 509,
	229, 526,
	222, 520
	};
	int right_bar_points[30] = {
	217, 596,
	217, 582,
	307, 538,
	307, 445,
	265, 371,
	266, 360,
	280, 355,
	294, 374,
	306, 373,
	308, 352,
	309, 261,
	315, 230,
	322, 220,
	330, 223,
	328, 597
	};
	int left_wood_points[56] = {
	51, 272,
	60, 276,
	70, 270,
	60, 255,
	84, 238,
	93, 249,
	105, 240,
	84, 212,
	82, 177,
	89, 166,
	104, 165,
	107, 178,
	111, 215,
	130, 225,
	137, 215,
	136, 197,
	117, 157,
	114, 129,
	132, 87,
	167, 64,
	168, 41,
	155, 40,
	95, 66,
	71, 95,
	55, 129,
	43, 184,
	41, 229,
	45, 247
	};
	int right_wood_points[48] = {
	191, 221,
	197, 214,
	230, 199,
	258, 175,
	274, 138,
	273, 104,
	260, 83,
	254, 74,
	262, 58,
	275, 57,
	301, 90,
	316, 125,
	319, 166,
	312, 200,
	290, 250,
	273, 278,
	262, 278,
	220, 255,
	221, 248,
	225, 239,
	221, 229,
	209, 228,
	198, 234,
	190, 228
	};
	int left_l_points[12] = {
	32, 467,
	38, 467,
	37, 521,
	98, 549,
	95, 555,
	30, 525
	};
	int right_l_points[12] = {
	284, 467,
	290, 467,
	290, 526,
	229, 555,
	226, 548,
	283, 522
	};
	int left_capsule_points[16] = {
	190, 48,
	194, 42,
	203, 42,
	205, 46,
	205, 72,
	201, 76,
	195, 76,
	190, 72
	};
	int right_capsule_points[16] = {
	222, 57,
	225, 51,
	232, 51,
	236, 57,
	235, 80,
	233, 85,
	226, 85,
	222, 81
	};
	int left_flipper_points[10] = {
	6, 13,
	7, 6,
	12, 2,
	52, 24,
	50, 27
	};
	int right_flipper_points[10] = {
	6, 24,
	45, 2,
	52, 6,
	52, 13,
	8, 27
	};

	//collider creation
	App->physics->CreateStaticChain(0, 0, board_points, 58);
	App->physics->CreateStaticChain(0, 0, triangle_1_points, 10);
	App->physics->CreateStaticChain(0, 0, triangle_2_points, 10);
	App->physics->CreateStaticChain(0, 0, right_bar_points, 30);
	App->physics->CreateStaticChain(0, 0, left_wood_points, 56);
	App->physics->CreateStaticChain(0, 0, right_wood_points, 48);
	App->physics->CreateStaticChain(0, 0, left_l_points, 12);
	App->physics->CreateStaticChain(0, 0, right_l_points, 12);
	App->physics->CreateStaticChain(0, 0, left_capsule_points, 16);
	App->physics->CreateStaticChain(0, 0, right_capsule_points, 16);

	//flippers
	left_flipper = App->physics->CreateChain(95, 547, left_flipper_points, 10);
	//left_flipper->body->SetType(b2_staticBody);
	right_flipper = App->physics->CreateChain(170, 547, right_flipper_points, 10);
	//right_flipper->body->SetType(b2_staticBody);

	//flippers anchors
	left_flipper_anchor = App->physics->CreateCircle(108, 557, 3);
	left_flipper_anchor->body->SetType(b2_staticBody);
	right_flipper_anchor = App->physics->CreateCircle(215, 557, 3);
	right_flipper_anchor->body->SetType(b2_staticBody);
}

void ModuleScene::initializeFlippers() {
	left_flipper_joint.Initialize(left_flipper_anchor->body, left_flipper->body, left_flipper_anchor->body->GetWorldCenter());
	left_flipper_joint.referenceAngle = 0;
	left_flipper_joint.enableLimit = true;
	left_flipper_joint.lowerAngle = -0.25f * b2_pi;
	left_flipper_joint.upperAngle = 0.25f * b2_pi;
	//joint.maxMotorTorque = 10.0f;
	//joint.motorSpeed = 1.0f;
	//joint.enableMotor = true;
	b2RevoluteJoint* left_joint = (b2RevoluteJoint*)App->physics->world->CreateJoint(&left_flipper_joint);

	right_flipper_joint.Initialize(right_flipper_anchor->body, right_flipper->body, right_flipper_anchor->body->GetWorldCenter());
	right_flipper_joint.referenceAngle = 0;
	right_flipper_joint.enableLimit = true;
	right_flipper_joint.lowerAngle = -0.25f * b2_pi;
	right_flipper_joint.upperAngle = 0.25f * b2_pi;
	b2RevoluteJoint* right_joint = (b2RevoluteJoint*)App->physics->world->CreateJoint(&right_flipper_joint);
}