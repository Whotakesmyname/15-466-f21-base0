#include "ColorTextureProgram.hpp"

#include "Mode.hpp"
#include "GL.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <random>

/*
 * PongMode is a game mode that implements a single-player game of Pong.
 */

struct PongMode : Mode {
	PongMode();
	virtual ~PongMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	glm::vec2 court_radius = glm::vec2(7.0f, 5.0f);
	glm::vec2 paddle_radius = glm::vec2(0.2f, 1.0f);
	float ball_radius = 0.2f;
	float star_radius = 1.0f;
	float star_radius_p3 = 1.0f;  // star radius^3 is proportional to its gravity force

	glm::vec2 left_paddle = glm::vec2(-court_radius.x + 0.5f, 0.0f);
	glm::vec2 right_paddle = glm::vec2( court_radius.x - 0.5f, 0.0f);

	glm::vec2 ball = glm::vec2(0.0f, 0.0f);
	glm::vec2 ball_velocity = glm::vec2(-1.0f, 0.0f);
	const float ball_max_velocity = 1.5f;

	glm::vec2 star = glm::vec2(5.f, 2.f);
	glm::vec2 star_velocity = glm::vec2(0.f, -1.f);
	uint32_t star_type = 0;
	bool star_valid = true;
	std::mt19937 random_engine;  // prng for star generation

	const int bg_star_n = 100;
	const float bg_star_r_min = 0.005f;
	const float bg_star_r_max = 0.02f;
	std::vector<glm::vec2> bg_stars;
	std::vector<float> bg_stars_radius;
	std::vector<float> bg_stars_velocity_y;


	uint32_t left_score = 0;
	uint32_t right_score = 0;
	uint32_t carry_score = 1;

	float ai_offset = 0.0f;
	float ai_offset_update = 0.0f;

	//----- pretty gradient trails -----

	float trail_length = 1.3f;
	std::deque< glm::vec3 > ball_trail; //stores (x,y,age), oldest elements first

	//----- scene settings -----
	const float wall_radius = 0.05f;
	const float shadow_offset = 0.07f;
	const float padding = 0.14f; //padding between outside of walls and edge of window
	const glm::vec2 score_radius = glm::vec2(.1f, .1f);

	const float upper = court_radius.y + 2.0f * wall_radius + padding + 3 * score_radius.y;
	const float bottom = -court_radius.y - 2 * wall_radius - padding;
	const float left = -court_radius.x - 2 * wall_radius - padding;
	const float right = court_radius.x + 2 * wall_radius + padding;

	//----- opengl assets / helpers ------

	//draw functions will work on vectors of vertices, defined as follows:
	struct Vertex {
		Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_, glm::vec2 const &TexCoord_) :
			Position(Position_), Color(Color_), TexCoord(TexCoord_) { }
		glm::vec3 Position;
		glm::u8vec4 Color;
		glm::vec2 TexCoord;
	};
	static_assert(sizeof(Vertex) == 4*3 + 1*4 + 4*2, "PongMode::Vertex should be packed");

	//Shader program that draws transformed, vertices tinted with vertex colors:
	ColorTextureProgram color_texture_program;

	//Buffer used to hold vertex data during drawing:
	GLuint vertex_buffer = 0;

	//Vertex Array Object that maps buffer locations to color_texture_program attribute locations:
	GLuint vertex_buffer_for_color_texture_program = 0;

	//Solid white texture:
	GLuint white_tex = 0;

	//matrix that maps from clip coordinates to court-space coordinates:
	glm::mat3x2 clip_to_court = glm::mat3x2(1.0f);
	// computed in draw() as the inverse of OBJECT_TO_CLIP
	// (stored here so that the mouse handling code can use it to position the paddle)

};
