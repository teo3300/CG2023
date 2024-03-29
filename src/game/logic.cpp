#include "game_main.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/fwd.hpp"
#include "log.h"
#include "project_setup.hpp"
#include <iostream>

// Handle control logic, be sure to dump needed values in the
// GameModel variables

#define SPEED_EFFECT_DAMPING 2.0f
#define ROTATION_EFFECT_DAMPING 8.0f

// Generic damp function
template <class T>
static inline T damp(T oldVal, T newVal, float lambda, float dt) {
	return
		(oldVal * (float)pow(M_E,-lambda * dt) ) +
		(newVal * (float)(1-pow(M_E, -lambda * dt)));
}

#define DAMP(T, val, lambda)\
	do{\
		static T val##Old = val;\
		val = damp<T>(val##Old, val, lambda, deltaT);\
		val##Old = val;\
	}while(0)

void GameMain::gameLogic(GameModel& game) {

	const float nearPlane = 0.1f;
	const float farPlane = 200.0f;

	// Camera target height and distance
	const float camHeight = 0.26;
	const float camDist = 2.2;
	// Camera Pitch limits
	const float minPitch = glm::radians(-90.0f);
	const float maxPitch = glm::radians(90.0f);
	//camera roll limits
	const float minRoll = glm::radians(-90.0f);
	const float maxRoll = glm::radians(90.0f);
	//camera Yaw limits
	const float minYaw = glm::radians(-90.0f);
	const float maxYaw = glm::radians(90.0f);
	// Rotation and motion speed
	const float ROT_SPEED = glm::radians(120.0f);

	const float BOOST_TIME = 4.0f;

	// Integration with the timers and the controllers
	// returns:
	// <float deltaT> the time passed since the last frame
	// <glm::vec3 m> the state of the motion axes of the controllers (-1 <= m.x, m.y, m.z <= 1)
	// <glm::vec3 r> the state of the rotation axes of the controllers (-1 <= r.x, r.y, r.z <= 1)
	// <bool fire> if the user has pressed a fire button (not required in this assginment)
	float deltaT;
	glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
	static bool
		is_on_crystal = false,
		was_on_crystal = false,
		fire;
	static float boost_time = 0.0f;
	this->getSixAxis(deltaT, m, r, fire);
	//here we convert the time passed since last frame into an actual "timestamp" 
	game.time += deltaT;

	static float
		MOVE_SPEED = 2;//standard movement speed
	static float Extra=0;//extra speed gained by the boost obtained by crystals
	static float CAP_SPEED=8;//max speed reachable 
	static float fixed_FOVy = glm::radians(45.0f);//FOV used when not in "boost mode"

	static float
		//variable FOV, to be changed depending on the presence of "boost mode" 
		FOVy = glm::radians(45.0f);

	// check if collision happened with a crystal
	was_on_crystal = is_on_crystal; //here i store the the value in another variable
	is_on_crystal = game.on_crystal(); //here i update the value to the current status
	//every second we output the character position, used for debug on object positions
	/*
	static float internal_timer = 1.0;
	if (internal_timer>0.0)
	{
		internal_timer-=deltaT;
	}
	else {
	std::cout<<game.character->position<<std::endl;
	internal_timer=1.0;
	}*/

	//if we are on a crystal and we werent before, add 4 seconds of boost time 
	if(is_on_crystal && (! was_on_crystal)) {
		boost_time += 4;
		logDebug("Power up");
	}
	
	MOVE_SPEED = 2;
	Extra=0;
	FOVy = glm::radians(45.0f);
	//if we don't have anymore boost we don't show any boost icon
	if(boost_time <= 0.0f) {
		uboBoost.visible=0;
		boost_time = 0.0f;
	} else {
		//if we have boost time we show the icon in the upright corner of the window
		uboBoost.visible=1;	
		//if we activate the boost we increase move speed and cange the FOV to "curve spacetime"
		if(fire && m.z > 0) {
			MOVE_SPEED = 8;
			Extra=25; //I want to go really fast foward
				FOVy = glm::radians(100.0f);
				r.x *= 0.5; // You are going into the hyperspace, drift with care
				r.y *= 0.5;
				boost_time -= deltaT;
		}
	}

	// Game Logic implementation
	// Current Player Position - statc variables make sure that their value remain 
	// unchanged in subsequent calls to the procedure
	static float CamYaw = 0.0f,
		CamPitch = 0.0f,
		CamRoll = 0.0f;

	CamYaw = -ROT_SPEED * deltaT * r.y;
	//CamYaw = glm::clamp(CamYaw,minYaw, maxYaw);
	CamPitch  = -ROT_SPEED * deltaT * r.x;
	//CamPitch = glm::clamp(CamPitch,minPitch, maxPitch);
	CamRoll   = ROT_SPEED * deltaT * r.z;
	//CamRoll = glm::clamp(CamRoll,minRoll, maxRoll);

	//quaternion making to avoid gimball lock on the starhip rotation
	game.character->rotation *=
		  glm::rotate(glm::quat(1,0,0,0), CamPitch, glm::vec3(1,0,0))
		* glm::rotate(glm::quat(1,0,0,0), CamYaw, glm::vec3(0,1,0))
		* glm::rotate(glm::quat(1,0,0,0), CamRoll, glm::vec3(0,0,1));
	//quaternion matrix, used in the world matrix for simplicity and in the creation of ux, uy and uz
	glm::mat4 MQ = glm::mat4(game.character->rotation);

	glm::vec3 ux = glm::vec3(MQ * glm::vec4(1,0,0,1));
	glm::vec3 uy = glm::vec3(MQ * glm::vec4(0,1,0,1));
	glm::vec3 uz = glm::vec3(MQ * glm::vec4(0,0,-1,1));

	DAMP(float, MOVE_SPEED, SPEED_EFFECT_DAMPING);
	DAMP(float, FOVy, SPEED_EFFECT_DAMPING);
	static float Momentum; 
	
	if(m.z>0)Momentum+=MOVE_SPEED*deltaT;
	else if(m.z<0) Momentum-=3*MOVE_SPEED*deltaT; //3* to make it hadle better
	else Momentum-=Momentum*(deltaT); //decrease momentum gradually
	Momentum=glm::max(-CAP_SPEED/3.0f,glm::min(Momentum,CAP_SPEED+Extra)); 
	//caps the speed, I wanna be slower in reverse cuz game mechanics


	//we calculate initial position 
	//we do not use the left/right up/down translation on the starship
	//game.character->position += Momentum.y * uy * deltaT; //for debugging
	game.character->position += Momentum * uz * deltaT;
	//we calculate initial position 
	//we do not use the left and right translation on the starship
	
	float dist = glm::length(game.character->position); //how far are you from the center of the universe
	//skybox limit collision and asteroids collision
	if(game.collision()||(dist > 68)){ //Did you hit anything or are you going out of bounds
		game.character->position -= 2*Momentum * uz * deltaT; //reset position
		Momentum*=(-3); //invert movement (*3 for a satisfying bounce)
	}


	glm::vec3 targetPosition = game.character->position;
	//the camera position is based on the target position with two rotations and a translation at a certain distance
	glm::vec3 cameraPosition = targetPosition+ camHeight*uy-camDist*uz;

	DAMP(glm::vec3, cameraPosition, ROTATION_EFFECT_DAMPING);

	game.camera->position = cameraPosition;

	// check for game ended
	static bool
		is_checkpoint = false,
		was_checkpoint = false;
	static float checkpoint_delay = 0.0f;
	was_checkpoint = is_checkpoint;
	is_checkpoint = game.race_check();

	// Check if game ended
	if(is_checkpoint && !was_checkpoint) {
		checkpoint_delay = 2.0f;
	}

	if(checkpoint_delay > 0.0f) {
		if((checkpoint_delay -= deltaT) <= 0.0f) {
			checkpoint_delay = 0.0f;
			if(game.race_make_next()) {
				logDebug("Implement end game");
			}
		}
	}

	//projection matrix
	glm::mat4 Mprj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
	glm::mat4 fixed_Mprj = glm::perspective(fixed_FOVy, Ar, nearPlane, farPlane);

	//view matrix
	glm::mat4 Mv =glm::lookAt(cameraPosition, targetPosition, uy);
	Mprj[1][1] *= -1;
	fixed_Mprj[1][1] *= -1;

	game.ViewPrj =Mprj*Mv;
	game.fixed_ViewPrj =fixed_Mprj*Mv;
	//world matrix
	game.World =  glm::translate(glm::mat4(1.0), game.character->position) * MQ;
}