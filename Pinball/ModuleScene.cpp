#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleFonts.h"
#include <iostream>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
	initial_position.x = START_BALL_POSITION_X;
	initial_position.y = START_BALL_POSITION_Y;
	//initial_position.x = 80;
	//initial_position.y = 450;
}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//sprites

	board_tex = App->textures->Load("assets/sprites/Rat_and_roll_board.png");
	spritesheet = App->textures->Load("assets/sprites/interactive_elements.png");
	letters = App->textures->Load("assets/sprites/Letra_derecha.png");
	letters_2 = App->textures->Load("assets/sprites/Letra_derecha2.png");
	Point_number = App->fonts->Load("assets/sprites/Point_numbers.png", "0123456789", 1);
	
	//sounds and music

	//App->audio->PlayMusic("assets/sound/background_music.ogg", 2.0f);
	bonus_fx = App->audio->LoadFx("assets/bonus.wav");
	pan_fx = App->audio->LoadFx("assets/sound/pan.wav");
	triangle_fx = App->audio->LoadFx("assets/sound/triangle.wav");
	ball_lost_fx = App->audio->LoadFx("assets/sound/ball_lost.wav");
	flipper_fx = App->audio->LoadFx("assets/sound/flipper.wav");
	ring_fx = App->audio->LoadFx("assets/sound/ring.wav");
	drain_fx = App->audio->LoadFx("assets/sound/drain.wav");
	capsule_fx = App->audio->LoadFx("assets/sound/capsule.wav");
	stove_2_fx = App->audio->LoadFx("assets/sound/stove_2.wav");
	ball_release_fx = App->audio->LoadFx("assets/sound/ball_release.wav");
	kicker_fx = App->audio->LoadFx("assets/sound/kicker.wav");


	InitializeSceneColliders();

	initializeInteractiveElements();

	//player ball
	ball = App->physics->CreateCircle(initial_position.x, initial_position.y, BALL_SIZE, 0.5f);
	
	return ret;
}

bool ModuleScene::CleanUp()
{
	//App->physics->world->DestroyJoint(left_revolute_joint);
	//left_revolute_joint = NULL;
	App->fonts->UnLoad(Point_number);

	LOG("Unloading Intro scene");
	App->textures->Unload(spritesheet);
	return true;
}

update_status ModuleScene::Update()
{

	if (lives > 0) 
	{
		if (change_ball_position) {
			TranslateBall(new_ball_x, new_ball_y, new_ball_speed);
			change_ball_position = false;
		}
		if (center_body != nullptr) {
			ball->body->SetTransform(center_body->body->GetWorldCenter(), 0);
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			ray_on = !ray_on;
			ray.x = App->input->GetMouseX();
			ray.y = App->input->GetMouseY();
		}

		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			//circles.add(App->physics->CreateCircle(START_BALL_POSITION_X, START_BALL_POSITION_X, BALL_SIZE));
			//circles.getLast()->data->listener = this;
			TranslateBall(initial_position.x, initial_position.y, b2Vec2_zero);
		}

		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
		}

		if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		{
			LOG("");
		}

		if (!(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)) playing_left_flipper_fx = false;

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
			b2Vec2 impulse = b2Vec2(0, -200);
			left_flipper->body->ApplyForceToCenter(impulse, 1);
			left_flipper_joint.lowerAngle = 45 * DEGTORAD;
			if (playing_left_flipper_fx == false)
			{
				App->audio->PlayFx(flipper_fx);
				playing_left_flipper_fx = true;
			}
		}

		if (!(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)) playing_right_flipper_fx = false;

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
			b2Vec2 impulse = b2Vec2(0, -200);
			right_flipper->body->ApplyForceToCenter(impulse, 1);
			right_flipper_joint.lowerAngle = 45 * DEGTORAD;
			if (playing_right_flipper_fx == false)
			{
				App->audio->PlayFx(flipper_fx);
				playing_right_flipper_fx = true;
			}
		}

		static int pow = 0;
		static int impulse = 100;
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
			kicker->body->ApplyForceToCenter(b2Vec2(0, pow*0.3 ),0);
			pow += 2;
			
			if (pow > 40)
				pow = 40;
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
		{
			kicker->body->ApplyForceToCenter(b2Vec2(0, -pow), 1);
			App->audio->PlayFx(kicker_fx);
		}

	}
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (restart_sensor->Contains(App->input->GetMouseX(), App->input->GetMouseY())) {
			lives = 3;
			change_ball_position = true;
			new_ball_x = START_BALL_POSITION_X;
			new_ball_y = START_BALL_POSITION_Y;
			new_ball_speed = b2Vec2_zero;
			score = 0;
		}

	}
	

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

	App->renderer->Blit(board_tex, 0, 0);

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
		//App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
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

	//blit game elements

	//blit flippers
	int x, y;
	SDL_Rect flipper_rect = { 4,13,48,15 };
	left_flipper->GetPosition(x, y);
	App->renderer->Blit(spritesheet, x , y, &flipper_rect, 1.0f, left_flipper->GetRotation());
	right_flipper->GetPosition(x, y);
	App->renderer->Blit(spritesheet, x, y, &flipper_rect, 1.0f,right_flipper->GetRotation(),SDL_FLIP_HORIZONTAL);

	//blit pans
	SDL_Rect pan_rect = {62,0,34,34};
	App->renderer->Blit(spritesheet, 162, 97, &pan_rect);
	App->renderer->Blit(spritesheet, 216, 103, &pan_rect);
	App->renderer->Blit(spritesheet, 178, 143, &pan_rect);

	//blit ball
	SDL_Rect ball_rect = { 130,0,16,16 };
	ball->GetPosition(x, y);
	App->renderer->Blit(spritesheet, x, y, &ball_rect);

	// blit kicker
	SDL_Rect kicker_rect = { 71,50,17,51 }; 
	kicker->GetPosition(x, y);
	App->renderer->Blit(spritesheet, 328, y, &kicker_rect);


	SDL_Rect red_carpet = { 373, 0, 139, 252 };
	App->renderer->Blit(spritesheet, 34, 30, &red_carpet);

	//blit sign
	SDL_Rect sign = { 0,139,70,53 };
	App->renderer->Blit(spritesheet, 35, 210, &sign);

	SDL_Rect knives = { 191, 0, 169 ,206 };
	App->renderer->Blit(spritesheet, 210, 20, &knives);

	//blit lights

	SDL_Rect triangle_light = { 5, 107, 21, 29 };
	if (show_left_light) {
		App->renderer->Blit(spritesheet, 66, 482, &triangle_light, NULL, NULL, SDL_FLIP_HORIZONTAL);
		show_left_light = false;
	}
	if (show_right_light)
	{
		App->renderer->Blit(spritesheet, 238, 482, &triangle_light);
		show_right_light = false;
	}

	//blit letters
	SDL_Rect letters_rect = { 2, 19, 208, 181};
	

	//blit letters 2
	SDL_Rect letters_rect2 = { 2, 19, 208, 181 };
	
	//play letters
	SDL_Rect play_rect = { 0, 386, 60, 18 };

	//replay letters
	SDL_Rect restart_rect = { 0, 424, 118, 18 };
	
	
	sprintf_s(point_text, 10, "%7d", score);
	App->fonts->BlitText(480, 171, Point_number, point_text);

	
	sprintf_s(point_text, 10, "%7d", score);
	App->fonts->BlitText(480, 200, Point_number, point_text);
	

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

	if (lives > 2)
	{
		App->renderer->Blit(spritesheet, 519, 224, &ball_rect);
		App->renderer->Blit(spritesheet, 499, 224, &ball_rect);
		App->renderer->Blit(spritesheet, 479, 224, &ball_rect);
		App->renderer->Blit(spritesheet, 450, 546, &play_rect);
		App->renderer->Blit(letters, 370, 350, &letters_rect);
	}
	if (lives > 1 && lives < 3)
	{
		App->renderer->Blit(spritesheet, 499, 224, &ball_rect);
		App->renderer->Blit(spritesheet, 479, 224, &ball_rect);
		App->renderer->Blit(spritesheet, 430, 546, &restart_rect);
		App->renderer->Blit(letters, 370, 350, &letters_rect);
	}
	if (lives > 0 && lives < 2)
	{
		App->renderer->Blit(spritesheet, 479, 224, &ball_rect);
		App->renderer->Blit(spritesheet, 430, 546, &restart_rect);
		App->renderer->Blit(letters, 370, 350, &letters_rect);
	}
	if (lives < 1)
	{
		App->renderer->Blit(spritesheet, 450, 546, &play_rect);
		App->renderer->Blit(letters_2, 370, 350, &letters_rect2);
	}

	if (score > high_score) high_score = score;

	return UPDATE_CONTINUE;
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
	

	//collider creation
	App->physics->CreateStaticChain(0, 0, board_points, 58);
	left_triangle = App->physics->CreateStaticChain(0, 0, triangle_1_points, 10);
	right_triangle = App->physics->CreateStaticChain(0, 0, triangle_2_points, 10);
	right_bar = App->physics->CreateStaticChain(0, 0, right_bar_points, 30);
	left_wood = App->physics->CreateStaticChain(0, 0, left_wood_points, 56);
	right_wood = App->physics->CreateStaticChain(0, 0, right_wood_points, 48);
    left_L = App->physics->CreateStaticChain(0, 0, left_l_points, 12);
	right_L = App->physics->CreateStaticChain(0, 0, right_l_points, 12);

	//collider listeners
	left_triangle->listener = this;
	right_triangle->listener = this;
	right_bar->listener = this;
	left_wood->listener = this;
	right_wood->listener = this;
	left_L->listener = this;
	right_L->listener = this;
}

void ModuleScene::initializeInteractiveElements() {
	int left_flipper_points[10] = {
	4, 20,
	7, 14,
	53, 18,
	50, 23,
	5, 27
	};
	int right_flipper_points[10] = {
	48, 15,
	52, 22,
	49, 29,
	4, 25,
	3, 21
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

	//flippers
	left_flipper = App->physics->CreateRectangle(108, 548, 48, 10);
	//left_flipper->body->SetType(b2_staticBody);
	right_flipper = App->physics->CreateRectangle(179, 548, 48, 10);
	//right_flipper->body->SetType(b2_staticBody);

	//flippers anchors
	left_flipper_anchor = App->physics->CreateCircle(110, 560, 3);
	left_flipper_anchor->body->SetType(b2_staticBody);
	right_flipper_anchor = App->physics->CreateCircle(214, 560, 3);
	right_flipper_anchor->body->SetType(b2_staticBody);

	//left flipper movement
	left_flipper_joint.bodyA = left_flipper->body;
	left_flipper_joint.bodyB = left_flipper_anchor->body;
	//left_flipper_joint.referenceAngle = 0;
	left_flipper_joint.lowerAngle = -25 * DEGTORAD;
	left_flipper_joint.upperAngle = 35 * DEGTORAD;
	left_flipper_joint.enableLimit = true;
	left_flipper_joint.localAnchorA.Set(PIXEL_TO_METERS(-14), PIXEL_TO_METERS(0));
	left_flipper_joint.localAnchorB.Set(0, 0);
	b2RevoluteJoint* left_joint = (b2RevoluteJoint*)App->physics->world->CreateJoint(&left_flipper_joint);


	//right flipper movement
	right_flipper_joint.bodyA = right_flipper->body;
	right_flipper_joint.bodyB = right_flipper_anchor->body;
	right_flipper_joint.referenceAngle = 0;
	right_flipper_joint.lowerAngle = -35 * DEGTORAD;
	right_flipper_joint.upperAngle = 25 * DEGTORAD;
	right_flipper_joint.enableLimit = true;
	right_flipper_joint.localAnchorA.Set(PIXEL_TO_METERS(14), PIXEL_TO_METERS(0));
	right_flipper_joint.localAnchorB.Set(0, 0);
	b2RevoluteJoint* right_joint = (b2RevoluteJoint*)App->physics->world->CreateJoint(&right_flipper_joint);

	//pans
	pan1 = App->physics->CreateCircle(180, 115, 16, 1.2f);
	pan1->body->SetType(b2_staticBody);
	pan1->listener = this;
	pan2 = App->physics->CreateCircle(235, 120, 16, 1.2f);
	pan2->body->SetType(b2_staticBody);
	pan2->listener = this;
	pan3 = App->physics->CreateCircle(195, 160, 16, 1.2f);
	pan3->body->SetType(b2_staticBody);
	pan3->listener = this;

	//capsules
	left_capsule = App->physics->CreateStaticChain(0, 0, left_capsule_points, 16, 1.0f);
	right_capsule = App->physics->CreateStaticChain(0, 0, right_capsule_points, 16, 1.0f);
	left_capsule->listener = this;
	right_capsule->listener = this;

	//kicker
	kicker = App->physics->CreateRectangle(START_BALL_POSITION_X-2, START_BALL_POSITION_Y+195, 10, 5);
	static_kicker = App->physics->CreateRectangle(START_BALL_POSITION_X-2, START_BALL_POSITION_Y+195, 10, 5);

	static_kicker->body->SetType(b2_staticBody);

	kicker_joint.bodyA = kicker->body;
	kicker_joint.bodyB = static_kicker->body;
	kicker_joint.collideConnected = false;
	kicker_joint.enableLimit = true;

	kicker_joint.lowerTranslation = PIXEL_TO_METERS(10);
	kicker_joint.upperTranslation = PIXEL_TO_METERS(50);

	kicker_joint.localAnchorA.Set(0, 0);
	kicker_joint.localAnchorB.Set(0, 0);

	kicker_joint.maxMotorForce = 12.0f;
	kicker_joint.motorSpeed = 8.0f;
	kicker_joint.enableMotor = true;

	kicker_joint.localAxisA.Set(0, 2);

	b2PrismaticJoint* joint_launcher = (b2PrismaticJoint*)App->physics->world->CreateJoint(&kicker_joint);

	//sensors

	bottom_sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 4, SCREEN_HEIGHT, SCREEN_WIDTH / 2, 1);
	bottom_sensor->listener = this;
	
	drain_sensor = App->physics->CreateCircleSensor(96, 178, 8);
	drain_sensor->listener = this;

	stove_1_sensor = App->physics->CreateCircleSensor(214, 241, 8);
	stove_1_sensor->listener = this;

	stove_2_sensor = App->physics->CreateCircleSensor(299, 363, 8);
	stove_2_sensor->listener = this;

	restart_sensor = App->physics->CreateRectangleSensor(480, 555, 210, 40);

	left_triangle_bouncer = App->physics->CreateRectangle(0, 0, 60, 3, 1.2f);
	left_triangle_bouncer->body->SetTransform(b2Vec2(PIXEL_TO_METERS(82), PIXEL_TO_METERS(490)), 60 * DEGTORAD);
	left_triangle_bouncer->body->SetType(b2_staticBody);
	left_triangle_bouncer->listener = this;

	right_triangle_bouncer = App->physics->CreateRectangle(0, 0, 60, 3, 1.2f);
	right_triangle_bouncer->body->SetTransform(b2Vec2(PIXEL_TO_METERS(240), PIXEL_TO_METERS(490)), -60 * DEGTORAD);
	right_triangle_bouncer->body->SetType(b2_staticBody);
	right_triangle_bouncer->listener = this;
}

void ModuleScene::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;
	LOG("");

	if ((bodyA == pan1) || (bodyA == pan2) || (bodyA == pan3))
	{
		App->audio->PlayFx(pan_fx);
		score += 10;
	}

	if (bodyA == bottom_sensor)
	{
		if (sensed == false)
		{
			change_ball_position = true;
			new_ball_x = START_BALL_POSITION_X;
			new_ball_y = START_BALL_POSITION_Y;
			new_ball_speed = b2Vec2_zero;
			App->audio->PlayFx(ball_lost_fx);
			lives--;
			sensed = true;
		}
	}

	if ((bodyA == left_capsule)||(bodyA == right_capsule))
	{
		App->audio->PlayFx(capsule_fx);
	}

	if (bodyA == drain_sensor)
	{
		if (sensor_holding == false) {
			sensor_contact_moment = SDL_GetTicks();
			new_ball_x = 160;
			new_ball_y = SCREEN_HEIGHT - 10;
			new_ball_speed.x = 10;
			new_ball_speed.y = -25;
			sensor_holding = true;
			App->audio->PlayFx(drain_fx);
			score += 100;
		}
		else
		{
			if (SDL_GetTicks() - sensor_contact_moment > stove_1_time * 1000)
			{
				change_ball_position = true;
				center_body = nullptr;
				sensor_holding = false;
				last_time_hold = SDL_GetTicks();
			}
			else
			{
				center_body = drain_sensor;
			}
		}
	}

	if (bodyA == stove_1_sensor)
	{
		if (sensor_holding == false)
		{
			sensor_contact_moment = SDL_GetTicks();
			sensor_holding = true;
			score += 75;
		}
		else
		{
			if (SDL_GetTicks() - sensor_contact_moment > stove_1_time * 1000)
			{
				ball->body->SetLinearVelocity(b2Vec2(-8, 12));
				center_body = nullptr;
				sensor_holding = false;
				last_time_hold = SDL_GetTicks();
				App->audio->PlayFx(ring_fx);
			}
			else
			{
				center_body = stove_1_sensor;
			}
		}
	}

	if (bodyA == stove_2_sensor)
	{
		if (sensor_holding == false)
		{
			sensor_contact_moment = SDL_GetTicks();
			sensor_holding = true;
			App->audio->PlayFx(stove_2_fx);
			score += 125;
		}
		else
		{
			if (SDL_GetTicks() - sensor_contact_moment > stove_2_time * 1000)
			{
				ball->body->SetLinearVelocity(b2Vec2(-8, -14));
				center_body = nullptr;
				sensor_holding = false;
				last_time_hold = SDL_GetTicks();
			}
			else
			{
				center_body = stove_2_sensor;
			}
		}
	}

	if ((bodyA == left_triangle_bouncer)&&(bodyB == ball))
	{
		App->audio->PlayFx(triangle_fx);
		score += 10;
		show_left_light = true;
	}

	if ((bodyA == right_triangle_bouncer) && (bodyB == ball)) {
		App->audio->PlayFx(triangle_fx);
		score += 10;
		show_right_light = true;
	}
}

void ModuleScene::NotOnCollision(PhysBody* bodyA, PhysBody* bodyB) {
	if (bodyA == bottom_sensor)
	{
		sensed = false;
	}
	if ((bodyA == stove_1_sensor)||(bodyA == stove_2_sensor)||(bodyA == drain_sensor))
	{
		sensor_holding = false;
	}
}

void ModuleScene::TranslateBall(int x, int y, b2Vec2 speed) {
	ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y)), 0);
	ball->body->SetLinearVelocity(speed);
	ball->body->SetAngularVelocity(0);
	if ((x == START_BALL_POSITION_X) && (y == START_BALL_POSITION_Y))	App->audio->PlayFx(ball_release_fx);
}
