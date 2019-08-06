#ifndef UNICODE
#define UNICODE
#endif 

#include "Header.h"
#include "resource.h"

#define HERO_LIFE_CRITICAL .000005
// #define SHOOT_INTERVAL 512

namespace gl
{
	HWND handle_game;

	static void display ();
	static void background (int);
	static void fps_control (int);
	static void reshape (int, int);
	static void key (unsigned char, int, int);
	static void key_up (unsigned char, int, int);
	static void mouse (int, int, int, int);
	static void move (int, int);
	static void active (int, int);
	static void timer_delete (int);
	static void text (std::string *, unsigned int, GLfloat, GLfloat);
	static void text (char *, GLfloat, GLfloat);
	static void text (char *, long long, GLfloat, GLfloat);
	static void quit_game ();
	static void infantry_spawn (int);
	static void set_game_speed ();

	GLuint frame = 0;
	GLfloat brightness_boost = .0;
	GLint background_parameter[3][3] ;//= { {30, -3, 1}, {35, -2, 4}, {29, 7, -3} };
	GLboolean paused = 0;
	GLuint menu_flag = 0;

	UINT ammo_count;
	long long score;

	int mouse_x, mouse_y;
	const GLdouble PI = acos (-1.0);
	GLdouble theta = .0;
	const INT16 N = 64; // a polygen circle // this is now abandoned
	BOOL right_pressed = 0, left_pressed = 0, up_pressed = 0, down_pressed = 0;
	BOOL mouse_down = 0;
	GLuint shoot_frame_count = 500;
	const INT16 max_speed = 7200;
	GLdouble hero_healing_rate = .000005;
	GLuint spawn_rate = 4000, spawn_starter = 5;
	UINT8 universal_speed_multiplier = 8;
	GLdouble speed_multiplier = universal_speed_multiplier * 0.00002;
	GLdouble speed_multiplier_bullets = universal_speed_multiplier * 0.000025;
	GLdouble speed_multiplier_infantry = universal_speed_multiplier * 0.0000004;
	GLdouble speed_multiplier_seeker = universal_speed_multiplier * 0.0000004;
	GLfloat bullet_damage = .04, sonar_damage = .00009;

	GLboolean displaying_strings = 0;
	GLuint num_of_strings = 0;
	GLuint current_string = 0;
	std::string mission_string[50];
	GLuint premission_counter = 0;

	const GLfloat block_w = .045, block_h = .08;
	class class_block
	{
		public:
			GLboolean destroyable;
			GLboolean is_trap;
			GLfloat x, y;
			GLint strength;
			GLboolean trap_attacking = 0;
			class_block (GLboolean _destroyable, GLfloat _x, GLfloat _y)
			{
				destroyable = _destroyable;
				is_trap = 0;
				x = _x;
				y = _y;
				strength = 32;
			}
			class_block (GLfloat _x, GLfloat _y) // constructor for trap
			{
				destroyable = 0;
				is_trap = 1;
				x = _x;
				y = _y;
				strength = 1;
			}
			void
			draw ()
			{
				if (!is_trap)
					{
						if (!destroyable)
							glColor3f (.7, .7, .67);
						else if (strength > 24)
							glColor3f (.55, .55, .55);
						else if (strength > 12)
							glColor3f (.45, .45, .45);
						else
							glColor3f (.35, .35, .35);
					}
				else
					{
						if (trap_attacking)
							{
								glColor3f (.2, .1, .9);
								trap_attacking = 0;
							}
						else
							glColor3f (.7, .4, .4);
					}
				glRectf (x, y, x + block_w, y + block_h);
			}
	};
	class_block **p_block = new class_block *[100];
	std::vector<class_block> vec_block;

	class healing_zone
	{
		public:
			GLboolean activated;
			GLfloat x, y;
			int life_countdown = 63000;
			healing_zone ()
			{
				activated = 0;
			}
			healing_zone (GLfloat _x, GLfloat _y)
			{
				activated = 1;
				x = _x;
				y = _y;
			}
			void
			draw ()
			{
				glColor3f (.25, 1.0, .25);
				glLineWidth (1.75);
				GLdouble phi;
				for (int i = 0; i < 12; i++)
					{
						phi = PI / 6 * i;
						glBegin (GL_LINES);
						glVertex2f (x + 0.063 * cos (phi), y + 0.112 * sin (phi));
						glVertex2f (x + 0.063 * cos (phi + PI / 12), y + 0.112 * sin (phi + PI / 12));
						glEnd ();
					}
				/* glBegin (GL_POLYGON);
				for (int i = 0; i < 12; i++)
					{
						glVertex2f (x + 0.063 * cos (2 * PI / 12 * i), y + 0.112 * sin (2 * PI / 12 * i));
					}
				glEnd (); */
				return;
			}
			void
			update ()
			{
				life_countdown -= universal_speed_multiplier;
				if (life_countdown < 0)
					activated = 0;
				return;
			}
	};
	healing_zone *p_hzone;

	class sonar_zone
	{
		public:
			GLboolean activated;
			GLfloat x, y;
			int life_countdown = 120000;
			sonar_zone ()
			{
				activated = 0;
			}
			sonar_zone (GLfloat _x, GLfloat _y)
			{
				activated = 1;
				x = _x;
				y = _y;
			}
			void
			draw ()
			{
				glColor3f (.4, .4, 1.0);
				glLineWidth (2.5);
				GLdouble phi;
				for (int i = 0; i < 12; i++)
					{
						phi = PI / 6 * i;
						glBegin (GL_LINES);
						glVertex2f (x + 0.063 * cos (phi), y + 0.112 * sin (phi));
						glVertex2f (x + 0.063 * cos (phi + PI / 12), y + 0.112 * sin (phi + PI / 12));
						glEnd ();
					}
				/* glBegin (GL_POLYGON);
				for (int i = 0; i < 12; i++)
					{
						glVertex2f (x + 0.063 * cos (2 * PI / 12 * i), y + 0.112 * sin (2 * PI / 12 * i));
					}
				glEnd (); */
				return;
			}
			void
			update ()
			{
				life_countdown -= universal_speed_multiplier;
				if (life_countdown < 0)
					activated = 0;
				return;
			}
	};
	sonar_zone *p_szone;

	class class_hero
	{
		public:
			GLfloat x, y;
			INT16 v_x, v_y;
			GLdouble strength = 1.0;
			GLint game_over_countdown = 14000;
			GLboolean can_emit_sonar = 0;
			class_hero ()
			{
				x = .0;
				y = -.6;
				v_x = v_y = 0;
			}
			void
			draw ()
			{
				if (strength < HERO_LIFE_CRITICAL)
					{
						if (x <= -.925)
							text ("GAME OVER", -.999, y);
						else
							text ("GAME OVER", x - 0.075, y);
					}
				else
					{
						glColor3f (1.0, 1.0, 1.0);
						glBegin (GL_POLYGON);
						/* for (int i = 0; i < N; i++)
							{
								glVertex2f (x + 0.0225 * cos (2 * PI / N * i), y + 0.04 * sin (2 * PI / N * i));
							} */
						glVertex2f (x + 0.027 * cos (0.0 * PI + theta), y + 0.048 * sin (0.0 * PI + theta));
						glVertex2f (x + 0.027 * cos (0.7 * PI + theta), y + 0.048 * sin (0.7 * PI + theta));
						glVertex2f (x + 0.009 * cos (1.0 * PI + theta), y + 0.018 * sin (1.0 * PI + theta));
						glVertex2f (x + 0.027 * cos (1.3 * PI + theta), y + 0.048 * sin (1.3 * PI + theta));
						glEnd ();
						// draw the life bar
						if (strength < 0.3)
							glColor3f (1.0, .0, .0);
						glRectf (x - 0.025, y + 0.072, x - 0.025 + 0.05 * strength, y + 0.0645);
					}
				return;
			}
			void
			update ()
			{
				if (strength < HERO_LIFE_CRITICAL)
					{
						if (game_over_countdown > 0)
							game_over_countdown -= universal_speed_multiplier;
						else
							paused = 1;
					}
				// hero position update
				x += v_x * speed_multiplier / (GLdouble) 1280;
				y += v_y * speed_multiplier / (GLdouble) 720;

				// hero velocity update
				if (left_pressed && v_x > -max_speed)
					v_x -= universal_speed_multiplier;
				else if (right_pressed && v_x < max_speed)
					v_x += universal_speed_multiplier;
				else
					{
						if (v_x < 0)
							v_x += universal_speed_multiplier;
						else if (v_x > 0)
							v_x -= universal_speed_multiplier;
					}
				if (up_pressed && v_y < max_speed)
					v_y += universal_speed_multiplier;
				else if (down_pressed && v_y > -max_speed)
					v_y -= universal_speed_multiplier;
				else
					{
						if (v_y < 0)
							v_y += universal_speed_multiplier;
						else if (v_y > 0)
							v_y -= universal_speed_multiplier;
					}

				GLfloat dx, dy;
				// automatically heal himself
				if (strength >= HERO_LIFE_CRITICAL)
					strength += hero_healing_rate * universal_speed_multiplier;
				if (strength > 1.0)
					strength = 1.0;
				// get the hero healed from the healing zone
				if (p_hzone->activated && strength >= HERO_LIFE_CRITICAL)
					{
						dx = x - p_hzone->x, dy = y - p_hzone->y;
						dx /= .063;
						dy /= .112;
						if (dx * dx + dy * dy < 1 && strength < 1)
							strength += .00009 * universal_speed_multiplier;
						if (strength > 1)
							strength = 1;
					}

				// see if hero is in the sonar zone
				if (p_szone->activated && strength >= HERO_LIFE_CRITICAL)
					{
						dx = x - p_szone->x, dy = y - p_szone->y;
						dx /= .063;
						dy /= .112;
						if (dx * dx + dy * dy < 1)
							can_emit_sonar = 1;
						else
							can_emit_sonar = 0;
					}
				else
					can_emit_sonar = 0;
				
				// What stops the hero?
				if (x < -.98 && v_x < 0 || x > .98 && v_x > 0)
					v_x = 0;
				if (y < -.97 && v_y < 0 || y > .97 && v_y > 0)
					v_y = 0;

				GLfloat tempx, tempy;
				for (auto it = vec_block.begin (); it != vec_block.end (); ++it)
					{
						tempx = it->x, tempy = it->y;
						if (y > tempy - .5 * block_h && y < tempy + 1.5 * block_h
						    && (v_x < 0 && x < tempx + 1.7 * block_w && x > tempx + block_w
								    || v_x > 0 && x > tempx - .7 * block_w && x < tempx))
							{
								v_x = 0;
								if (it->is_trap)
									{
										strength -= universal_speed_multiplier * 0.00012;
										it->trap_attacking = 1;
									}
								break;
							}
						else if (x > tempx - .5 * block_w && x < tempx + 1.5 * block_w
						         && (v_y < 0 && y < tempy + 1.7 * block_h && y > tempy + block_h
										     || v_y > 0 && y > tempy - .7 * block_h && y < tempy))
							{
								v_y = 0;
								if (it->is_trap)
									{
										strength -= universal_speed_multiplier * 0.00012;
										it->trap_attacking = 1;
									}
								break;
							}
					}
				return;
			}
			void try_shoot ();
	};
	class_hero hero;

	// a class for bullets shot by the hero
	class class_bullet
	{
		public:
			GLfloat x, y;
			GLdouble v_x, v_y;
			class_bullet ()
			{
				x = hero.x;
				y = hero.y;
				v_x = cos (theta);
				v_y = sin (theta);
			}
			void
			draw ()
			{
				glColor3f (1.0, .2, .2);
				glBegin (GL_POLYGON);
				for (int i = 0; i < 12; i++)
					{
						glVertex2f (x + 0.0063 * cos (2 * PI / 12 * i), y + 0.0112 * sin (2 * PI / 12 * i));
					}
				glEnd ();
				return;
			}
			void
			update ()
			{
				if (x > -1.1 && x < 1.1 && y > -1.1 && y < 1.1)
					{
						x += 9.0 * speed_multiplier_bullets * v_x;
						y += 16.0 * speed_multiplier_bullets * v_y;
					}
				return;
			}
	};
	class_bullet *p_bullet;
	std::vector<class_bullet> vec_bullet;
	static void bullet_delete ();

	class class_sonar
	{
		public:
			GLfloat x, y;
			GLdouble v_x, v_y;
			GLdouble v_theta;
			GLint life_countdown = 14000;
			class_sonar ()
			{
				x = hero.x;
				y = hero.y;
				v_x = cos (theta);
				v_y = sin (theta);
				v_theta = theta;
			}
			void
			draw ()
			{
				if (life_countdown < 0)
					return;
				glColor3f (.7, .7, 1.0);
				glLineWidth (3.0);
				GLfloat x_base = x - .0108 * cos (v_theta), y_base = y - .0192 * sin (v_theta);
				for (int i = -3; i < 3; i++)
					{
						glBegin (GL_LINES);
						glVertex2f (x_base + 0.0108 * cos (v_theta + 2 * PI / 15 * i),
												y_base + 0.0192 * sin (v_theta + 2 * PI / 15 * i));
						glVertex2f (x_base + 0.0108 * cos (v_theta + 2 * PI / 15 * (i + 1)),
												y_base + 0.0192 * sin (v_theta + 2 * PI / 15 * (i + 1)));
						glEnd ();
					}
				return;
			}
			void
			update ()
			{
				if (life_countdown < 0)
					return;
				life_countdown -= universal_speed_multiplier;
				if (x > -1.1 && x < 1.1 && y > -1.1 && y < 1.1)
					{
						x += 2.7 * speed_multiplier_bullets * v_x;
						y += 4.8 * speed_multiplier_bullets * v_y;
					}
				return;
			}
	};
	class_sonar *p_sonar;
	std::vector<class_sonar> vec_sonar;

	class class_infantry
	{
		public:
			GLfloat x, y;
			UINT16 side;
			UINT16 size;
			GLuint shoot_count = 0;
			GLuint bullet_type; // 0: hunter-seeker; 1: ordinary bullet.
			GLint strength = 6;
			class_infantry () // random
			{
				x = 1.8 * ((float) rand () / RAND_MAX) - 0.9;
				y = 1.8 * ((float) rand () / RAND_MAX) - 0.9;
				side = (rand () & 3) + 3;
				size = 5; // (rand () & 3) + 4;
				bullet_type = rand () & 1;
			}
			class_infantry (char ch) // random but confined
			{
				if (rand () & 1)
					x = .4 * ((float) rand () / RAND_MAX) + .5;
				else
					x = -(.4 * ((float) rand () / RAND_MAX) + .5);
				y = -.8 * ((float) rand () / RAND_MAX);
				side = (rand () & 3) + 3;
				size = 5; // (rand () & 3) + 4;
				bullet_type = rand () & 1;
			}
			class_infantry (GLfloat x_param, GLfloat y_param, UINT16 side_param, UINT16 size_param, GLuint type_param)
			{
				x = x_param;
				y = y_param;
				side = side_param;
				size = size_param;
				bullet_type = type_param;
			}
			class_infantry (GLfloat _x, GLfloat _y) // semi-random
			{
				x = _x;
				y = _y;
				side = (rand () & 3) + 3;
				size = 5;
				bullet_type = rand () & 1;
			}
			void
			draw ()
			{
				if (strength <= 3)
					glColor3f (.6, .6, .5);
				else
					glColor3f (.7, .7, .3);
				glBegin (GL_POLYGON);
				for (int i = 0; i < side; i++)
					{
						glVertex2f (x + 0.0045 * size * cos (2 * PI / side * i), y + 0.008 * size * sin (2 * PI / side * i));
					}
				glEnd ();
				return;
			}
			void
			update () // hunter-seeker logic now unused for infantries
			{
				GLfloat dx, dy;
				dx = hero.x - x;
				dy = hero.y - y;
				GLdouble square = sqrt (dx * dx + dy * dy);
				int speed = (rand () & 3) + 1;
				x += 9.0 * speed * speed_multiplier_infantry * dx / square;
				y += 16.0 * speed * speed_multiplier_infantry * dy / square;
				return;
			}
			void infantry_shoot_seeker (GLfloat, GLfloat);
			void infantry_shoot_bullet (GLfloat, GLfloat);
			void
			try_shoot ()
			{
				switch (bullet_type)
					{
						case 0:
							if (shoot_count >> 13)
								{
									infantry_shoot_seeker (x, y);
									shoot_count = 0;
								}
							else
								shoot_count += universal_speed_multiplier;
							return;
						case 1:
							if (shoot_count >> 12)
							{
								infantry_shoot_bullet (x, y);
								shoot_count = 0;
							}
							else
								shoot_count += universal_speed_multiplier;
							return;
					}
			}
	};
	class_infantry *p_infantry;
	class_infantry **init_infantry = new class_infantry *[100];
	std::vector<class_infantry> vec_infantry;
	static void create_infantry ();

	class class_boss
	{
		public:
			GLfloat x, y;
			GLboolean x_incre = 1;
			GLdouble strength;
			GLfloat red = .45, green = .74, blue = .2;
			GLboolean red_incre = 1, green_incre = 0, blue_incre = 1;
			GLuint shoot_count = 0;
			GLboolean attack_flag = 0;
			GLint accomplished_countdown = 22000;
			class_boss (GLfloat _x, GLfloat _y)
			{
				x = _x;
				y = _y;
				strength = 1.0;
			}
			void
			draw ()
			{
				glColor3f (red, green, blue);
				if (strength < HERO_LIFE_CRITICAL)
					{
						text ("BOSS NEUTRALIZED", x - 0.11, y);
						return;
					}
				glBegin (GL_POLYGON);
				for (int i = 0; i < 20; i++)
					{
						if (i % 2)
							glVertex2f (x + .054 * cos (2 * PI / 20 * i), y + .096 * sin (2 * PI / 20 * i));
						else
							glVertex2f (x + .045 * cos (2 * PI / 20 * i), y + .08 * sin (2 * PI / 20 * i));
					}
				glEnd ();
				// draw the life bar
				glRectf (x - 0.05, y + 0.12, x - 0.05 + 0.1 * strength, y + 0.112);
				return;
			}
			void boss_shoot_bullet (GLfloat, GLfloat);
			void
			update ()
			{
				if (strength < HERO_LIFE_CRITICAL && hero.strength >= HERO_LIFE_CRITICAL)
					{
						if (accomplished_countdown > 0)			
							accomplished_countdown -= universal_speed_multiplier;
						else
							paused = 1;
					}
				if (strength < 1 && strength >= HERO_LIFE_CRITICAL)
					{
						strength += .00004 * universal_speed_multiplier;
						if (!attack_flag)
							strength += .00006 * universal_speed_multiplier;
					}
				if (red_incre)
					red += .00055;
				else
					red -= .00035;
				if (red > .97)
					red_incre = 0;
				else if (red < .2)
					red_incre = 1;

				if (green_incre)
					green += .00032;
				else
					green -= .00032;
				if (green > .97)
					red_incre = 0;
				else if (green < .2)
					red_incre = 1;

				if (blue_incre)
					blue += .00057;
				else
					blue -= .00035;
				if (blue > .97)
					blue_incre = 0;
				else if (blue < .2)
					blue_incre = 1;

				if (x_incre)
					{
						x += 50 * speed_multiplier_infantry;
						if (x > .87)
							x_incre = 0;
					}
				else
					{
						x -= 50 * speed_multiplier_infantry;
						if (x < -.87)
							x_incre = 1;
					}
				return;
			}
			void
			try_shoot ()
			{
				if (strength < HERO_LIFE_CRITICAL || !attack_flag)
					return;
				if (shoot_count >> 12)
					{
						boss_shoot_bullet (x, y);
						shoot_count = 0;
					}
				else
					shoot_count += universal_speed_multiplier;
				return;
			}
	};
	class_boss *p_boss;

	class hostile_seeker
	{
		public:
			GLfloat x, y;
			GLint life_countdown;
			hostile_seeker (GLfloat __x, GLfloat __y)
			{
				x = __x;
				y = __y;
				life_countdown = 70000;
			}
			void
			draw ()
			{
				glColor3f (.8, .8, .7);
				glBegin (GL_POLYGON);
				for (int i = 0; i < 16; i++)
					{
						glVertex2f (x + 0.009 * cos (2 * PI / 12 * i), y + 0.016 * sin (2 * PI / 12 * i));
					}
				glEnd ();
				return;
			}
			void
			update () // hunter-seeker logic
			{
				life_countdown -= universal_speed_multiplier;
				GLfloat dx, dy;
				dx = hero.x - x;
				dy = hero.y - y;
				GLdouble square = sqrt (dx * dx + dy * dy);
				x += 72.0 * speed_multiplier_seeker * dx / square;
				y += 128.0 * speed_multiplier_seeker * dy / square;
				return;
			}
	};
	hostile_seeker *p_seeker;
	std::vector<hostile_seeker> vec_seeker;

	class hostile_bullet
	{
		public:
			GLfloat x, y;
			GLdouble v_x, v_y;
			hostile_bullet (GLfloat __x, GLfloat __y)
			{
				x = __x;
				y = __y;
				v_x = (16.0 / 9.0) * (hero.x - x);
				v_y = hero.y - y;
				GLdouble sq = sqrt (v_x * v_x + v_y * v_y);
				v_x /= sq;
				v_y /= sq;
			}
			hostile_bullet (GLfloat __x, GLfloat __y, GLdouble phi)
			{
				x = __x;
				y = __y;
				v_x = cos (phi);
				v_y = sin (phi);
			}
			void
			draw ()
			{
				glColor3f (.6, .8, .5);
				glBegin (GL_POLYGON);
				for (int i = 0; i < 12; i++)
					{
						glVertex2f (x + 0.0063 * cos (2 * PI / 12 * i), y + 0.0112 * sin (2 * PI / 12 * i));
					}
				glEnd ();
				return;
			}
			void
			update ()
			{
				if (x > -1.1 && x < 1.1 && y > -1.1 && y < 1.1)
					{
						x += 3.6 * speed_multiplier_bullets * v_x;
						y += 6.4 * speed_multiplier_bullets * v_y;
					}
				return;
			}
	};
	hostile_bullet *p_hsbullet;
	std::vector<hostile_bullet> vec_hsbullet;

	static void detect_collision ();
}

HWND hwnd; // handle to main menu window
WINDOWPLACEMENT lwndpl;
WINDOWPLACEMENT *lpwndpl;

int call_time = 0xFF; // prevent multiple fps controllers from overlapping!
int respeed_count = 0;

HBITMAP background_bmp;
HBITMAP background_bmp_altered;
HBITMAP button_bmp[3];
INT32 button_state = 0;
HBITMAP tut_bmp[6];
INT32 tut_state = -1;

INT32 mouse_x = 0, mouse_y = 0;

BOOL game_started = 0;
static bool game_launch ();
static void reset_game (UINT16);
const std::string mission_id[25] = { "00.ini", "01.ini", "02.ini", "03.ini", "04.ini",
																		 "05.ini", "06.ini", "07.ini", "08.ini", "09.ini",
																		 "10.ini", "11.ini", "12.ini", "13.ini", "14.ini",
																		 "15.ini", "16.ini", "17.ini", "18.ini", "19.ini",
																		 "20.ini", "21.ini", "22.ini", "23.ini", "24.ini" };
UINT16 current_mission = 1;

LRESULT CALLBACK WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static void
paint (HWND hwnd)
{
	PAINTSTRUCT paint_struct;
	HDC hdc = BeginPaint (hwnd, &paint_struct);
	HDC hdc_buffer = CreateCompatibleDC (hdc);
	HDC hdc_loadBmp = CreateCompatibleDC (hdc);

	// initialization
	HBITMAP	blank = CreateCompatibleBitmap (hdc, 1280, 720);
	SelectObject (hdc_buffer, blank);

	if (tut_state == -1)
		{
			SelectObject (hdc_loadBmp, mouse_y > 150 ? background_bmp : background_bmp_altered);
			BitBlt (hdc_buffer, 0, 0, 1280, 720, hdc_loadBmp, 0, 0, SRCCOPY); // cashing bg

			SelectObject (hdc_loadBmp, button_bmp[button_state]);
			TransparentBlt (hdc_buffer, 417, 480, 400, 150, hdc_loadBmp, 0, 0, 400, 150, // 400*150 is the size of button bmps
											RGB (255, 255, 255)); // cashing button
		}
	else
		{
			SelectObject (hdc_loadBmp, tut_bmp[tut_state]);
			BitBlt (hdc_buffer, 0, 0, 1280, 720, hdc_loadBmp, 0, 0, SRCCOPY);
		}

	BitBlt (hdc, 0, 0, 1280, 720, hdc_buffer, 0, 0, SRCCOPY); // paint to the window

	DeleteObject (blank);
	DeleteDC (hdc_buffer);
	DeleteDC (hdc_loadBmp);

	EndPaint (hwnd, &paint_struct);
	return;
}

static void
key_response (HWND hwnd, WPARAM wParam)
{
	if (tut_state >= 0)
		return;
	switch (wParam)
		{
			case VK_UP:
				button_state = (button_state + 2) % 3;
				break;
			case 'W':
				button_state = (button_state + 2) % 3;
				break;
			case VK_DOWN:
				button_state = (button_state + 1) % 3;
				break;
			case 'S':
				button_state = (button_state + 1) % 3;
				break;
			case VK_RETURN:
				switch (button_state)
					{
						case 0: // New Crusade
							game_launch ();
							break;
						case 1: // Tutorial
							if (tut_state == -1)
								{
									tut_state = 0;
									InvalidateRect (hwnd, NULL, FALSE);
								}
							break;
						case 2: // Quit Game
							DestroyWindow (hwnd);
						default:
							break;
					}
			default:
				break;
		}
	InvalidateRect (hwnd, NULL, FALSE);
	return;
}

static void
click_response (HWND hwnd)
{
	if (tut_state >= 0)
		{
			if (mouse_y > 154 && mouse_y < 182)
				{
					if (mouse_x > 921 && mouse_x < 996 && tut_state > 0)
						tut_state--;
					else if (mouse_x > 1014 && mouse_x < 1090 && tut_state < 5)
						tut_state++;
					else if (mouse_x > 1145 && mouse_x < 1248)
						tut_state = -1;
				}
			// (921, 154), (996, 182), (1014), (1090), (1145), (1248)
			InvalidateRect (hwnd, NULL, FALSE);
			return;
		}

	if (mouse_x > 482 && mouse_x < 655 && mouse_y > 497 && mouse_y < 520)
		{
			button_state = (button_state + 2) % 3;
			InvalidateRect (hwnd, NULL, FALSE);
		}
	else if (mouse_x < 754 && mouse_x > 588 && mouse_y > 597 && mouse_y < 620)
		{
			button_state = (button_state + 1) % 3;
			InvalidateRect (hwnd, NULL, FALSE);
		}
	else if (mouse_x < 761 && mouse_x > 479 && mouse_y < 574 && mouse_y > 537)
		{
			switch (button_state)
				{
					case 0: // New Crusade
						game_launch ();
						break;
					case 1: // Tutorial
						if (tut_state == -1)
							{
								tut_state = 0;
								InvalidateRect (hwnd, NULL, FALSE);
							}
						break;
					case 2: // Quit Game
						DestroyWindow (hwnd);
					default:
						break;
				}
		}
	return;
}

int WINAPI
wWinMain (HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Class Unknown";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ICON1));
	wc.hCursor = LoadCursor (nullptr, IDC_ARROW);

	RegisterClass (&wc);

	// Create the window.

	hwnd = CreateWindowEx (
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Very Very Frightening Me - Menu",    // Window text
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,            // Window style

		// Size and position
		100, 50, 1280, 720,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
		);

	if (hwnd == NULL)
		{
			return 0;
		}

	ShowWindow (hwnd, nCmdShow);

	// Run the message loop.

	MSG msg = {};
	while (GetMessage (&msg, NULL, 0, 0))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	return 0;
}

LRESULT CALLBACK
WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
		{
			case WM_CREATE:
				background_bmp = LoadBitmap (((LPCREATESTRUCT) lParam)->hInstance, MAKEINTRESOURCE (IDB_BITMAP1));
				background_bmp_altered = LoadBitmap (((LPCREATESTRUCT) lParam)->hInstance, MAKEINTRESOURCE (IDB_BITMAP12));
				button_bmp[0] = LoadBitmap (((LPCREATESTRUCT) lParam)->hInstance, MAKEINTRESOURCE (IDB_BITMAP3)); // New Crusade 
				button_bmp[1] = LoadBitmap (((LPCREATESTRUCT) lParam)->hInstance, MAKEINTRESOURCE (IDB_BITMAP2)); // Tutorial
				button_bmp[2] = LoadBitmap (((LPCREATESTRUCT) lParam)->hInstance, MAKEINTRESOURCE (IDB_BITMAP4)); // Quit Game
				for (int i = 0; i < 6; i++)
					tut_bmp[i] = LoadBitmap (((LPCREATESTRUCT) lParam)->hInstance, MAKEINTRESOURCE (IDB_BITMAP13 + i)); // Tutorials
				InvalidateRect (hwnd, NULL, 0);
				return 0;
			case WM_DESTROY:
				PostQuitMessage (0);
				return 0;
			case WM_PAINT:
				paint (hwnd);
				return 0;
			case WM_MOUSEMOVE:
				mouse_x = LOWORD (lParam);
				mouse_y = HIWORD (lParam);
				InvalidateRect (hwnd, NULL, 0);
				return 0;
			case WM_LBUTTONDOWN:
				click_response (hwnd);
				return 0;
			case WM_KEYDOWN:
				key_response (hwnd, wParam);
				return 0;
			case WM_MOUSEWHEEL:
				if (tut_state >= 0)
					{
						if (tut_state > 0 && (int) wParam > 0)
							tut_state--;
						else if (tut_state >= 0 && tut_state < 5 && (int) wParam < 0)
							tut_state++;
					}
				else 
					{
						if ((int) wParam > 0)
							button_state = (button_state + 2) % 3;
						else
							button_state = (button_state + 1) % 3;
					}
				InvalidateRect (hwnd, NULL, 0);
				return 0;
			default:
				break;
		}
	return DefWindowProc (hwnd, uMsg, wParam, lParam);
}


static bool
game_launch ()
{
	if (game_started)
		return 0;

	current_mission = 1;
	reset_game (current_mission);

	game_started = 1;
	lpwndpl = &lwndpl;
	GetWindowPlacement (hwnd, lpwndpl);
	lpwndpl->showCmd = SW_SHOWMINIMIZED;
	SetWindowPlacement (hwnd, lpwndpl);

	// GLUT launch-up
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition (100, 50);
	glutInitWindowSize (1280, 720);
	glutCreateWindow ("Very Very Frightening Me");
	glutReshapeFunc (gl::reshape);
	glutKeyboardFunc (gl::key);
	glutKeyboardUpFunc (gl::key_up);
	glutMouseFunc (gl::mouse);
	glutPassiveMotionFunc (gl::move);
	glutMotionFunc (gl::active);
	glutDisplayFunc (gl::display);
	glutIdleFunc (gl::display);
	glutMainLoop ();

	return 1;
}

inline void
skip_comment (std::ifstream *ifs) // skip comment lines starting with #
{
	long long pos = ifs->tellg ();
	std::string comment;
	char ch = ifs->get ();
	while (ch == '#' || ch == ' ' || ch == '\n')
		{	
			if (ch == '#')
				{
					std::getline (*ifs, comment);
				}
			pos = ifs->tellg ();
			ch = ifs->get ();
		}
	ifs->seekg (pos);
	return;
}

static void
reset_game (UINT16 mission)
{
	if (mission > 49)
		return;
	call_time++;

	srand (time (0));
	using namespace gl;

	vec_block.clear ();
	vec_bullet.clear ();
	vec_sonar.clear ();
	vec_hsbullet.clear ();
	vec_infantry.clear ();
	vec_seeker.clear ();

	std::string filename = ".\\missions\\mission" + mission_id[mission];
	std::ifstream in (filename);
	if (!in.is_open ())
		return;
	bool temp_bool;
	float temp_float[5];
	// set background
	for (int i = 0; i < 3; i++)
		{
			skip_comment (&in);
			for (int j = 0; j < 3; j++)
				{
					in >> background_parameter[i][j];
				}
		}
	// set ordinary blocks
	unsigned num_of_blocks;
	skip_comment (&in);
	in >> num_of_blocks;
	for (unsigned i = 0; i < num_of_blocks; i++)
		{
			skip_comment (&in);
			in >> temp_bool >> temp_float[0] >> temp_float[1];
			p_block[i] = new class_block (temp_bool, temp_float[0], temp_float[1]);
		}
	// set traps
	unsigned num_of_traps;
	skip_comment (&in);
	in >> num_of_traps;
	for (unsigned i = 0; i < num_of_traps; i++)
		{
			skip_comment (&in);
			in >> temp_float[0] >> temp_float[1];
			p_block[num_of_blocks + i] = new class_block (temp_float[0], temp_float[1]);
		}
	for (unsigned j = 0; j < num_of_blocks + num_of_traps; j++)
		vec_block.push_back (*p_block[j]);
	delete [] *p_block;
	// set infantries
	unsigned num_of_drones;
	skip_comment (&in);
	in >> num_of_drones;
	for (unsigned i = 0; i < num_of_drones; i++)
		{
			skip_comment (&in);
			in >> temp_float[0] >> temp_float[1];
			init_infantry[i] = new class_infantry (temp_float[0], temp_float[1]);
		}
	for (unsigned i = 0; i < num_of_drones; i++)
		vec_infantry.push_back (*init_infantry[i]);
	delete [] *init_infantry;
	// set boss
	skip_comment (&in);
	in >> temp_float[0] >> temp_float[1];
	p_boss = new class_boss (temp_float[0], temp_float[1]);
	// set hero
	skip_comment (&in);
	in >> hero.x >> hero.y;
	skip_comment (&in);
	in >> hero_healing_rate;
	hero.v_x = hero.v_y = .0;
	hero.strength = 1.0;
	hero.game_over_countdown = 14000;
	shoot_frame_count = 500;
	// set ammo
	skip_comment (&in);
	in >> ammo_count;
	// set weapon damage
	skip_comment (&in);
	in >> bullet_damage >> sonar_damage;
	// set infantry spawn parameters
	skip_comment (&in);
	in >> spawn_starter;
	skip_comment (&in);
	in >> spawn_rate;
	// set pre-mission strings
	skip_comment (&in);
	in >> num_of_strings;
	for (unsigned i = 0; i < num_of_strings; i++)
		{
			skip_comment (&in);
			std::getline (in, mission_string[i]);
		}
		
	in.close ();

	displaying_strings = 1;
	current_string = 0;
	premission_counter = 0;

	p_hzone = new healing_zone;
	p_szone = new sonar_zone;

	left_pressed = right_pressed = up_pressed = down_pressed = 0;
	mouse_down = 0;
	score = 0;
	frame = 0;
	respeed_count = 0;
	universal_speed_multiplier = 8;
	brightness_boost = .0;
	paused = 0;
	menu_flag = 0;

	glutTimerFunc (1000, gl::fps_control, call_time);
	glutTimerFunc (1000, gl::timer_delete, call_time + 0xFF);
	glutTimerFunc (4000, gl::infantry_spawn, call_time + 0x1FF);

	return;
}

/* GLUT SECTION */

static void
gl::display ()
{
	if (game_started && !paused)
		{
			if (!displaying_strings)
				{
					detect_collision ();

					glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glPushMatrix ();
					/* draw a background! */
					background (0);

					text ("AMMO: ", (long long) ammo_count, -.995, -1.0); // print ammo count
					text ("Your Score: ", score, -.085, .96); // print player's score

					// draw and update healing zone
					if (p_hzone->activated)
						{
							p_hzone->draw ();
							p_hzone->update ();
						}
					//draw and update sonar zone
					if (p_szone->activated)
						{
							p_szone->draw ();
							p_szone->update ();
						}
					// draw and update bullets
					for (auto it = vec_bullet.begin (); it != vec_bullet.end (); ++it)
						{
							(*it).draw ();
							(*it).update ();
						}
					// draw and update sonars
					for (auto it = vec_sonar.begin (); it != vec_sonar.end (); ++it)
						{
							(*it).draw ();
							(*it).update ();
						}
					// draw and update hostile seekers
					for (auto it = vec_seeker.begin (); it != vec_seeker.end (); ++it)
						{
							(*it).draw ();
							(*it).update ();
						}
					// draw and update hostile bullets
					for (auto it = vec_hsbullet.begin (); it != vec_hsbullet.end (); ++it)
						{
							(*it).draw ();
							(*it).update ();
						}
					// draw blocks
					for (auto it = vec_block.begin (); it != vec_block.end (); ++it)
						{
							(*it).draw ();
						}
					// draw enemy infantries
					for (auto it = vec_infantry.begin (); it != vec_infantry.end (); ++it)
						{
							(*it).draw ();
							// (*it).update ();
							(*it).try_shoot ();
						}

			
					p_boss->draw (); // draw the boss
					hero.draw (); // draw the hero

					glFlush ();
					glPopMatrix ();
					glutSwapBuffers ();

					p_boss->update (); // update the boss
					p_boss->try_shoot ();

					hero.update (); // update the hero
					hero.try_shoot ();
				}
			else // display the pre-mission strings
				{
					glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glPushMatrix ();

					background (1);
					if (current_string == 0)
						text ("Press [SPACE] to skip.", -.16, -.96);
					if (premission_counter > 40000)
						{
							current_string++;
							premission_counter = 0;
							if (current_string == num_of_strings)
								{
									displaying_strings = 0;
									return;;
								}
						}
					text (&mission_string[current_string], premission_counter / 300 + 1, -.8, .0);
					glFlush ();
					glPopMatrix ();
					glutSwapBuffers ();

					premission_counter += universal_speed_multiplier;
				}
		}
	else if (game_started && paused) /* draw the pause menu */
		{
			glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glPushMatrix ();

			menu_flag = 0;
			if (hero.strength >= HERO_LIFE_CRITICAL && p_boss->strength >= HERO_LIFE_CRITICAL)
				{
					if (mouse_x > -617 && mouse_x < -538 && mouse_y < -234 && mouse_y > -252)
						{
							text ("[ Resume ]", -.96, -.7);
							menu_flag = 1;
						}
					else
						text ("Resume", -.96, -.7);
				}
			if (p_boss->strength < HERO_LIFE_CRITICAL && hero.strength >= HERO_LIFE_CRITICAL)
				{
					text (&std::string("MISSION ACCOMPLISHED!"), (premission_counter / 1000) % 120, -.175, .0);
					premission_counter += universal_speed_multiplier;
					if (mouse_x > -617 && mouse_x < -565 && mouse_y < -272 && mouse_y > -289)
						{
							text ("[ Next ]", -.96, -.8);
							menu_flag = 4;
						}
					else
						text ("Next", -.96, -.8);
				}
			else
				{
					if (mouse_x > -617 && mouse_x < -546 && mouse_y < -272 && mouse_y > -289)
						{
							text ("[ Restart ]", -.96, -.8);
							menu_flag = 2;
						}
					else
						text ("Restart", -.96, -.8);
				}
			if (mouse_x > -617 && mouse_x < -573 && mouse_y < -305 && mouse_y > -324)
				{
					text ("[ Exit ]", -.96, -.9);
					menu_flag = 3;
				}
			else
				text ("Exit", -.96, -.9);

			glFlush ();
			glPopMatrix ();
			glutSwapBuffers ();
		}
	frame++;
	return;
}

static void
gl::reshape (int width, int height)
{
	glutReshapeWindow (1280, 720);
	return;
}

static void
gl::background (int stage)
{
	if ((mouse_down || stage && premission_counter < 20000) &&
	    brightness_boost < 36.0)
		brightness_boost += universal_speed_multiplier * .0025;
	else if (brightness_boost > .3)
		brightness_boost -= universal_speed_multiplier * .0075;
	for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
				{
					glColor3ub (background_parameter[0][0] + background_parameter[0][1] * i + background_parameter[0][2] * j + 2 * (int) brightness_boost,
											background_parameter[1][0] + background_parameter[1][1] * i + background_parameter[1][2] * j + (int) brightness_boost,
											background_parameter[2][0] + background_parameter[2][1] * i + background_parameter[2][2] * j + (int) brightness_boost);
					glRectf (.25 * i - 1, .25 * j - 1, .25 * (i + 1) - 1, .25 * (j + 1) - 1);
				}
		}
	return;
}

static void
gl::mouse (int button, int state, int x, int y)
{
	mouse_x = x - 640;
	mouse_y = 360 - y;
	theta = atan ((mouse_y - 360 * hero.y) / (mouse_x - 640 * hero.x));
	if (mouse_x < 640 * hero.x)
		theta += PI;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			mouse_down = 1;
			if (paused)
				{
					switch (menu_flag)
						{
							case 1:
								paused = 0;
								return;
							case 2:
								reset_game (current_mission);
								return;
							case 3:
								quit_game ();
								return;
							case 4:
								reset_game (++current_mission); // reset for next mission
							default:
								return;
						}
				}
		}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			mouse_down = 0;
			// bullet_delete ();
		}
	return;
}

static void
gl::move (int x, int y)
{
	mouse_x = x - 640;
	mouse_y = 360 - y;
	theta = atan ((mouse_y - 360 * hero.y) / (mouse_x - 640 * hero.x));
	if (mouse_x < 640 * hero.x)
		theta += PI;
	return;
}

static void
gl::active (int x, int y)
{
	mouse_x = x - 640;
	mouse_y = 360 - y;
	theta = atan ((mouse_y - 360 * hero.y) / (mouse_x - 640 * hero.x));
	if (mouse_x < 640 * hero.x)
		theta += PI;
	return;
}

static void
gl::key (unsigned char key, int parameter_0, int parameter_1)
{
	switch (key)
	{
		case 'a':
			left_pressed = 1;
			break;
		case 'd':
			right_pressed = 1;
			break;
		case 'w':
			up_pressed = 1;
			break;
		case 's':
			down_pressed = 1;
			break;
		case 'A':
			left_pressed = 1;
			break;
		case 'D':
			right_pressed = 1;
			break;
		case 'W':
			up_pressed = 1;
			break;
		case 'S':
			down_pressed = 1;
			break;
		case 27: // esc
			paused = (paused + 1) & 1;
			break;
		case ' ': // space
			if (displaying_strings)
				displaying_strings = premission_counter = 0;
			break;	
		default:
			break;
	}
	return;
}

static void
gl::key_up (unsigned char key, int parameter_0, int parameter_1)
{
	switch (key)
	{
		case 'a':
			left_pressed = 0;
			break;
		case 'd':
			right_pressed = 0;
			break;
		case 'w':
			up_pressed = 0;
			break;
		case 's':
			down_pressed = 0;
			break;
		case 'A':
			left_pressed = 0;
			break;
		case 'D':
			right_pressed = 0;
			break;
		case 'W':
			up_pressed = 0;
			break;
		case 'S':
			down_pressed = 0;
			break;
		default:
			break;
	}
	return;
}

static void
gl::timer_delete (int num)
{
	if (num != call_time + 0xFF)
		return;
	bullet_delete (); // delete bullets every second
	glutTimerFunc (1000, timer_delete, num);
	return;
}

static void
gl::text (std::string *str, unsigned int num, GLfloat x, GLfloat y)
{
	if (str->length () > 2)
		if (str->at (0) == '(' && str->at (1) == 'n' && str->at (2) == 'o')
			return;
	glColor3f (1.0, 1.0, 1.0);
	glRasterPos2f (x, y);
	if (num >= str->length ()) 
		for (char i : *str)
			glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, i);
	else
		for (unsigned i = 0; i < num; i++)
			glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, str->at (i));
	return;
}

static void
gl::text (char *str, GLfloat x, GLfloat y)
{
	glColor3f (1.0, 1.0, 1.0);
	glRasterPos2f (x, y);
	for (char *i = str; *i != '\0'; i++)
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, *i);
	return;
}

static void
gl::text (char *str, long long num, GLfloat x, GLfloat y)
{
	glColor3f (1.0, 1.0, 1.0);
	glRasterPos2f (x, y);
	for (char *i = str; *i != '\0'; i++)
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, *i);
	int digit[6] = { 0, 0, 0, 0, 0, 0 };
	int i = 0;
	while (num != 0 && i < 6)
		{
			digit[i] = num % 10;
			num /= 10;
			i++;
		}
	for (int i = 5; i >= 0; i--)
		{
			glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, (char) (digit[i] + 48));
		}
	return;
}

void
gl::class_hero::try_shoot ()
{
	if (mouse_down && ammo_count > 0 && strength >= HERO_LIFE_CRITICAL && !can_emit_sonar) // prerequisite for a shot
		{
			if (shoot_frame_count >> 9) // rate of fire
				{
					shoot_frame_count = 0;
					p_bullet = new class_bullet;
					vec_bullet.push_back (*p_bullet);
					delete p_bullet;
					ammo_count--;
				}
			else
				shoot_frame_count += universal_speed_multiplier;
		}
	else if (mouse_down && strength >= HERO_LIFE_CRITICAL && can_emit_sonar)
		{
			if (shoot_frame_count >> 10) // slower ROF for sonar
				{
					shoot_frame_count = 0;
					p_sonar = new class_sonar;
					vec_sonar.push_back (*p_sonar);
					delete p_sonar;
				}
			else
				shoot_frame_count += universal_speed_multiplier;		
		}
}
void
gl::bullet_delete ()
{
	for (auto it = vec_bullet.begin (); it != vec_bullet.end (); )
		{
			if ((*it).x < -1.02 || (*it).x > 1.02 || (*it).y < -1.02 || (*it).y > 1.02)
				it = vec_bullet.erase (it);
			else
				++it;
		}
	for (auto it = vec_hsbullet.begin (); it != vec_hsbullet.end (); )
		{
			if ((*it).x < -1.02 || (*it).x > 1.02 || (*it).y < -1.02 || (*it).y > 1.02)
				it = vec_hsbullet.erase (it);
			else
				++it;
		}
	for (auto it = vec_sonar.begin (); it != vec_sonar.end (); )
		{
			if (it->life_countdown <= 0)
				it = vec_sonar.erase (it);
			else
				++it;
		}
	return;
}

void
gl::class_infantry::infantry_shoot_seeker (GLfloat x, GLfloat y)
{
	if (vec_seeker.size () < 16)
		{
			p_seeker = new hostile_seeker (x, y);
			vec_seeker.push_back (*p_seeker);
			delete p_seeker;
		}
	return;
}

void
gl::class_infantry::infantry_shoot_bullet (GLfloat x, GLfloat y)
{
	if (vec_hsbullet.size () < 24)
		{
			p_hsbullet = new hostile_bullet (x, y);
			vec_hsbullet.push_back (*p_hsbullet);
			delete p_hsbullet;
		}
	return;
}

void
gl::class_boss::boss_shoot_bullet (GLfloat x, GLfloat y)
{
	if (vec_hsbullet.size () < 24)
		{
			if (strength > .7)
				{
					for (int i = 0; i < 12; i++)
						{
							p_hsbullet = new hostile_bullet (x, y, i * (PI / 6));
							vec_hsbullet.push_back (*p_hsbullet);
						}
					delete p_hsbullet;
				}
			else
				{
					GLfloat dx = (16.0 / 9.0) * (hero.x - x);
					GLfloat dy = hero.y - y;
					GLdouble phi = atan (dy / dx);
					if (dx < 0)
						phi += PI;
					for (int i = -5; i <= 5; i++)
						{
							p_hsbullet = new hostile_bullet (x, y, phi + i * (PI / 18));
							vec_hsbullet.push_back (*p_hsbullet);
						}
					delete p_hsbullet;
				}
		}
	if (vec_seeker.size () < 12)
		{
			p_seeker = new hostile_seeker (-1.0, y - 0.3 + 0.4 * ((float) rand () / RAND_MAX));
			vec_seeker.push_back (*p_seeker);	
			p_seeker = new hostile_seeker (1.0, y - 0.3 + 0.4 * ((float) rand () / RAND_MAX));
			vec_seeker.push_back (*p_seeker);	
			delete p_seeker;
		}
	return;
}

void
gl::create_infantry ()
{
	p_infantry = new class_infantry ();
	vec_infantry.push_back (*p_infantry);
	delete p_infantry;
	return;
}

void
gl::detect_collision ()
{
	GLfloat dx = .0, dy = .0;
	GLboolean collided;
	volatile GLfloat threshold;
	// check if any bullet or the hero hits an enemy infantry
	for (auto it_ift = vec_infantry.begin (); it_ift != vec_infantry.end (); )
		{
			threshold = 0.0005 * (*it_ift).size;
			dx = hero.x - (*it_ift).x;
			dy = hero.y - (*it_ift).y;
			if (dx > -10.8 * threshold && dx < 10.8 * threshold && dy > -19.2 * threshold && dy < 19.2 * threshold)
				{
					if (hero.strength >= HERO_LIFE_CRITICAL)
						hero.strength -= 0.00003 * universal_speed_multiplier;
				}
			for (auto it_blt = vec_bullet.begin (); it_blt != vec_bullet.end (); )
				{
					dx = (*it_blt).x - (*it_ift).x;
					dy = (*it_blt).y - (*it_ift).y;
					if (dx > -9 * threshold && dx < 9 * threshold && dy > -16 * threshold && dy < 16 * threshold)
						{
							it_ift->strength--;
							it_blt = vec_bullet.erase (it_blt);
							break;
						}
					++it_blt;
				}
			if (!vec_sonar.empty ())
				{
					for (auto it_snr = vec_sonar.begin (); it_snr != vec_sonar.end (); ++it_snr)
						{
							if (it_snr->life_countdown <= 0)
								continue;
							dx = it_snr->x - it_ift->x;
							dy = it_snr->y - it_ift->y;
							if (dx > -18 * threshold && dx < 18 * threshold && dy > -32 * threshold && dy < 32 * threshold)
								{
									it_ift->strength--;
									break;
								}
						}
				}
			if (!it_ift->strength)
				{
					score += 200;
					if ((rand () & 3) == 3)
						{
							if ((rand () >> 3) & 1)
								{
									if (!p_hzone->activated)
										{
											delete p_hzone;
											p_hzone = new healing_zone (it_ift->x, it_ift->y);
										}
								}
							else
								{
									if (!p_szone->activated)
										{
											delete p_szone;
											p_szone = new sonar_zone (it_ift->x, it_ift->y);
										}
								}		
						}
					it_ift = vec_infantry.erase (it_ift);
					if (vec_infantry.empty ())
						{
							p_boss->attack_flag = 1; // player should eliminate all infantries to trigger a 1-on-1 with the boss
							break;
						}	
				}
			else
				++it_ift;
		}
	// check if any bullet hit the boss
	threshold = .0045;
	for (auto it_blt = vec_bullet.begin (); it_blt != vec_bullet.end (); )
		{
			dx = it_blt->x - p_boss->x;
			dy = it_blt->y - p_boss->y;
			if (dx > -9 * threshold && dx < 9 * threshold && dy > -16 * threshold && dy < 16 * threshold)
				{
					if (p_boss->strength >= HERO_LIFE_CRITICAL)
						{
							it_blt = vec_bullet.erase (it_blt);
							p_boss->strength -= bullet_damage;
							if (p_boss->strength <= 0)
								score += 50000;
						}
					return;
				}
			++it_blt;
		}
	if (!vec_sonar.empty ())
		{
			for (auto it_snr = vec_sonar.begin (); it_snr != vec_sonar.end (); ++it_snr)
				{
					if (it_snr->life_countdown <= 0)
						continue;
					dx = it_snr->x - p_boss->x;
					dy = it_snr->y - p_boss->y;
					if (dx > -9 * threshold && dx < 9 * threshold && dy > -16 * threshold && dy < 16 * threshold)
						{
							if (p_boss->strength >= HERO_LIFE_CRITICAL)
								{
									p_boss->strength -= sonar_damage * universal_speed_multiplier;
									if (p_boss->strength <= 0)
										score += 50000;
								}
						}	
				}
		}
	// If hero bumps into the boss, gg
	if (hero.strength >= HERO_LIFE_CRITICAL && p_boss->strength >= HERO_LIFE_CRITICAL)
		{
			dx = abs (hero.x - p_boss->x);
			dy = abs (hero.y - p_boss->y);
			dy = (dy * 9) / 16;
			if (dx * dx + dy * dy < .004)
				hero.strength -= .01 * universal_speed_multiplier;
		}
	// check if any hostile seeker hits the hero or is intercepted by a hero bullet
	threshold = 0.0017;
	for (auto it_hsk = vec_seeker.begin (); it_hsk != vec_seeker.end (); )
		{
			if ((*it_hsk).life_countdown < 1)
				{
					it_hsk = vec_seeker.erase (it_hsk);
					continue;
				}
			collided = 0;
			dx = hero.x - (*it_hsk).x;
			dy = hero.y - (*it_hsk).y;
			if (dx > -18 * threshold && dx < 18 * threshold && dy > -32 * threshold && dy < 32 * threshold)
				{
					it_hsk = vec_seeker.erase (it_hsk);
					if (hero.strength >= HERO_LIFE_CRITICAL)
						hero.strength -= .1;
					continue;
				}
			for (auto it_blt = vec_bullet.begin (); it_blt != vec_bullet.end (); )
				{
					dx = (*it_blt).x - (*it_hsk).x;
					dy = (*it_blt).y - (*it_hsk).y;
					if (dx > -9 * threshold && dx < 9 * threshold && dy > -16 * threshold && dy < 16 * threshold)
						{
							collided = 1;
							it_blt = vec_bullet.erase (it_blt);
							score += 18;
							break;
						}
					++it_blt;
				}
			if (!vec_sonar.empty ())
				{
					for (auto it_snr = vec_sonar.begin (); it_snr != vec_sonar.end (); it_snr++)
						{
							if (it_snr->life_countdown <= 0)
								continue;
							dx = (*it_snr).x - (*it_hsk).x;
							dy = (*it_snr).y - (*it_hsk).y;
							if (dx > -18 * threshold && dx < 18 * threshold && dy > -32 * threshold && dy < 32 * threshold)
								{
									collided = 1;
									score += 18;
									break;
								}
						}					
				}
			if (collided)
				it_hsk = vec_seeker.erase (it_hsk);
			else
				++it_hsk;
		}
	// check if the hero is hit by a hostile bullet
	for (auto it_hbl = vec_hsbullet.begin (); it_hbl != vec_hsbullet.end (); )
		{
			threshold = 0.0015;
			dx = hero.x - (*it_hbl).x;
			dy = hero.y - (*it_hbl).y;
			if (dx > -18 * threshold && dx < 18 * threshold && dy > -32 * threshold && dy < 32 * threshold)
				{
					it_hbl = vec_hsbullet.erase (it_hbl);
					if (hero.strength >= HERO_LIFE_CRITICAL)
						hero.strength -= .06;
					continue;
				}
			else
				++it_hbl;
		}
	// check if any gunfire hits a block
	for (auto it_blt = vec_bullet.begin (); it_blt != vec_bullet.end (); )
		{
			collided = 0;
			for (auto it_blc = vec_block.begin (); it_blc != vec_block.end (); it_blc++)
				{
					if (it_blt->x > it_blc->x && it_blt->x < it_blc->x + block_w &&
					    it_blt->y > it_blc->y && it_blt->y < it_blc->y + block_h)
						{
							it_blt = vec_bullet.erase (it_blt);
							collided = 1;
							if (it_blc->destroyable)
								{
									it_blc->strength -= 4;
									if (it_blc->strength <= 0)
										vec_block.erase (it_blc);
								}
							break;
						}
				}
			if (!collided)
				it_blt++;
		}
	for (auto it_blt = vec_hsbullet.begin (); it_blt != vec_hsbullet.end (); )
		{
			collided = 0;
			for (auto it_blc = vec_block.begin (); it_blc != vec_block.end (); it_blc++)
				{
					if (it_blt->x > it_blc->x && it_blt->x < it_blc->x + block_w &&
					    it_blt->y > it_blc->y && it_blt->y < it_blc->y + block_h)
						{
							it_blt = vec_hsbullet.erase (it_blt);
							collided = 1;
							if (it_blc->destroyable)
								{
									it_blc->strength--;
									if (it_blc->strength <= 0)
										vec_block.erase (it_blc);
								}
							break;
						}
				}
			if (!collided)
				it_blt++;
		}
	for (auto it_blt = vec_seeker.begin (); it_blt != vec_seeker.end (); )
		{
			collided = 0;
			for (auto it_blc = vec_block.begin (); it_blc != vec_block.end (); it_blc++)
				{
					if (it_blt->x > it_blc->x - .2 * block_w && it_blt->x < it_blc->x + 1.2 * block_w &&
					    it_blt->y > it_blc->y - .2 * block_h && it_blt->y < it_blc->y + 1.2 * block_h)
						{
							it_blt = vec_seeker.erase (it_blt);
							collided = 1;
							if (it_blc->destroyable)
								{
									it_blc->strength -= 3;
									if (it_blc->strength <= 0)
										vec_block.erase (it_blc);
								}
							break;
						}
				}
			if (!collided)
				it_blt++;
		}
}

void
gl::quit_game ()
{
	handle_game = GetActiveWindow ();
	DestroyWindow (handle_game); // quit the game without destroying the menu window
	GetWindowPlacement (hwnd, lpwndpl);
	lpwndpl->showCmd = SW_SHOWDEFAULT;
	SetWindowPlacement (hwnd, lpwndpl); // return to the main menu windows
	game_started = 0;
	return;
}

void
gl::set_game_speed ()
{
	speed_multiplier = universal_speed_multiplier * 0.00002;
	speed_multiplier_bullets = universal_speed_multiplier * 0.000025;
	speed_multiplier_infantry = universal_speed_multiplier * 0.0000004;
	speed_multiplier_seeker = universal_speed_multiplier * 0.0000004;
}

static void
gl::infantry_spawn (int num)
{
	if (num != call_time + 0x1FF)
		return;
	if (vec_infantry.size () < spawn_starter && !p_boss->attack_flag)
		{
			p_infantry = new class_infantry ('c');
			vec_infantry.push_back (*p_infantry);
			delete p_infantry;
		}
	glutTimerFunc (spawn_rate, infantry_spawn, num);
	return;
}

static void
gl::fps_control (int num)
{
	// fps * universal_speed_multiplier ~= 8000
	if (!game_started || num != call_time || respeed_count > 3)
		return;
	if (frame > 1000)
		{
			if (frame > 1600)
				{
					if (frame > 2000)
						universal_speed_multiplier = 4;
					else
						universal_speed_multiplier = 5;
				}
			else
				{
					if (frame > 1333)
						universal_speed_multiplier = 6;
					else if (frame > 1142)
						universal_speed_multiplier = 7;
					else
						universal_speed_multiplier = 8;
				}
		}
	else
		{
			if (frame > 640)
				{
					if (frame > 900)
						universal_speed_multiplier = 9;
					else if (frame > 800)
						universal_speed_multiplier = 10;
					else if (frame > 750)
						universal_speed_multiplier = 11;
					else if (frame > 700)
						universal_speed_multiplier = 12;
					else if (frame > 650)
						universal_speed_multiplier = 13;
					else if (frame > 640)
						universal_speed_multiplier = 14;
				}
			else
				{
					if (frame > 450)
						{
							if (frame > 575)
								universal_speed_multiplier = 15;
							else if (frame > 550)
								universal_speed_multiplier = 16;
							else if (frame > 500)
								universal_speed_multiplier = 17;
							else
								universal_speed_multiplier = 18;
						}
					else
						{
							if (frame > 430)
								universal_speed_multiplier = 19;
							else if (frame > 410)
								universal_speed_multiplier = 20;
							else if (frame > 395)
								universal_speed_multiplier = 21;
							else if (frame > 388)
								universal_speed_multiplier = 22;
							else if (frame > 377)
								universal_speed_multiplier = 23;
							else if (frame > 350)
								universal_speed_multiplier = 24;
							else if (frame > 335)
								universal_speed_multiplier = 25;
							else if (frame > 315)
								universal_speed_multiplier = 26;
							else if (frame > 307)
								universal_speed_multiplier = 27;
							else if (frame > 298)
								universal_speed_multiplier = 28;
							else
								universal_speed_multiplier = 29;
						}
				}
		}
	frame = 0;
	if (p_boss->attack_flag)
		respeed_count++;
	set_game_speed ();

	glutTimerFunc (1000, fps_control, num);
	return;
}