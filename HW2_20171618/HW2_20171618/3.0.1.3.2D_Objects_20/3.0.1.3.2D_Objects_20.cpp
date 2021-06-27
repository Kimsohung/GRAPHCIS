#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

														  // include glm/*.hpp only if necessary
														  //#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 0, win_height = 0;
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.5f; axes[1][1] = 0.0f;
	axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.5f;
	axes[3][0] = 0.0f; axes[3][1] = win_height / 2.5f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.25f; axes[1][0] = win_width / 2.25f;
	axes[2][1] = -win_height / 2.25f;
	axes[3][1] = win_height / 2.25f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
					   // y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

//person
#define PERSON_FACE 0
#define PERSON_LEFT_EYE 1
#define PERSON_RIGHT_EYE 2
#define PERSON_BODY 3
#define PERSON_LEFT_ARM 4
#define PERSON_RIGHT_ARM 5
#define PERSON_LEFT_LEG 6
#define PERSON_RIGHT_LEG 7
GLfloat face[360][2];
GLfloat left_eye[4][2] = { {-5.0, 50.0}, {-8.0, 50.0}, {-8.0, 55.0}, {-5.0, 55.0} };
GLfloat right_eye[4][2] = { {5.0, 50.0}, { 8.0, 50.0 }, { 8.0, 55.0 }, { 5.0, 55.0 } };
GLfloat person_body[4][2] = { {20.0,-20.0}, {-20.0, -20.0}, {-20.0, 30.0}, {20.0, 30.0} };
GLfloat left_arm[4][2] = { {-20.0, 30.0}, {-20.0, 10.0}, {-40.0, -10.0}, {-50.0, 0.0} };
GLfloat right_arm[4][2] = { {20.0, 30.0}, {20.0, 10.0}, {40.0, -10.0}, {50.0, 0.0} };
GLfloat left_leg[4][2] = { {-20.0, -20.0}, {-5.0, -20.0}, {-5.0, -90.0}, {-20.0, -90.0} };
GLfloat right_leg[4][2] = { {20.0, -20.0}, {5.0, -20.0}, {5.0, -90.0}, {20.0, -90.0} };
GLfloat person_colors[8][3] = {
	{255 / 255.0f, 204 / 255.0f, 204 / 255.0f},
	{0 / 255.0f, 0 / 255.0f, 0 / 255.0f},
	{0 / 255.0f, 0 / 255.0f, 0 / 255.0f},
	{255 / 255.0f, 153 / 255.0f, 204 / 255.0f},
	{255 / 255.0f, 153 / 255.0f, 204 / 255.0f},
	{255 / 255.0f, 153 / 255.0f, 204 / 255.0f},
	{102 / 255.0f, 102 / 255.0f, 204 / 255.0f},
	{102 / 255.0f, 102 / 255.0f, 204 / 255.0f},
};

GLuint VBO_person, VAO_person;

void prepare_person() {
	GLsizeiptr buffer_size = sizeof(face) + sizeof(left_eye) + sizeof(right_eye) + sizeof(person_body) + sizeof(left_arm) + sizeof(right_arm) + sizeof(left_leg) + sizeof(right_leg);

	for (int i = 0; i < 360; i++) {
		face[i][0] = 20 * cos(i);
		face[i][1] = 20 * sin(i) + 50;
	}

	glGenBuffers(1, &VBO_person);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_person);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(face), face);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(face), sizeof(left_eye), left_eye);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(face) + sizeof(left_eye), sizeof(right_eye), right_eye);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(face) + sizeof(left_eye)+ sizeof(right_eye), sizeof(person_body), person_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(face) + sizeof(left_eye)+ sizeof(right_eye) + sizeof(person_body), sizeof(left_arm), left_arm);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(face) + sizeof(left_eye)+ sizeof(right_eye) + sizeof(person_body) + sizeof(left_arm), sizeof(right_arm), right_arm);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(face) + sizeof(left_eye)+ sizeof(right_eye) + sizeof(person_body) + sizeof(left_arm) + sizeof(right_arm), sizeof(left_leg), left_leg);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(face) + sizeof(left_eye)+ sizeof(right_eye) + sizeof(person_body) + sizeof(left_arm) + sizeof(right_arm) + sizeof(left_leg), sizeof(right_leg), right_leg);

	glGenVertexArrays(1, &VAO_person);
	glBindVertexArray(VAO_person);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_person);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_person() {
	glBindVertexArray(VAO_person);

	glUniform3fv(loc_primitive_color, 1, person_colors[PERSON_FACE]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 360);

	glUniform3fv(loc_primitive_color, 1, person_colors[PERSON_LEFT_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, 360, 4);

	glUniform3fv(loc_primitive_color, 1, person_colors[PERSON_RIGHT_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, 364, 4);

	glUniform3fv(loc_primitive_color, 1, person_colors[PERSON_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 368, 4);

	glUniform3fv(loc_primitive_color, 1, person_colors[PERSON_LEFT_ARM]);
	glDrawArrays(GL_TRIANGLE_FAN, 372, 4);

	glUniform3fv(loc_primitive_color, 1, person_colors[PERSON_RIGHT_ARM]);
	glDrawArrays(GL_TRIANGLE_FAN, 376, 4);

	glUniform3fv(loc_primitive_color, 1, person_colors[PERSON_LEFT_LEG]);
	glDrawArrays(GL_TRIANGLE_FAN, 380, 4);

	glUniform3fv(loc_primitive_color, 1, person_colors[PERSON_RIGHT_LEG]);
	glDrawArrays(GL_TRIANGLE_FAN, 384, 4);

	glBindVertexArray(0);
}

#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 },{ -20.0, 15.0 },{ -20.0, 20.0 },{ 0.0, 23.0 },{ 20.0, 20.0 },{ 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 },{ -11.0, -12.0 },{ -12.0, -7.0 },{ 0.0, -10.0 },{ 12.0, -7.0 },{ 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 },{ -6.0, 0.0 },{ -6.0, 22.0 },{ 6.0, 22.0 },{ 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 },{ -7.0, 24.0 },{ -7.0, 21.0 },{ 7.0, 21.0 },{ 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 },{ -18.0, 3.0 },{ -16.0, 10.0 },{ -18.0, 20.0 },{ -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 },{ 18.0, 3.0 },{ 16.0, 10.0 },{ 18.0, 20.0 },{ 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

int airplane_clock = 0;
float airplane_s_factor = 1.0f;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2) + sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}

//draw car2
#define CAR2_BODY 0
#define CAR2_FRONT_WINDOW 1
#define CAR2_BACK_WINDOW 2
#define CAR2_FRONT_WHEEL 3
#define CAR2_BACK_WHEEL 4
#define CAR2_LIGHT1 5
#define CAR2_LIGHT2 6

GLfloat car2_body[8][2] = { { -18.0, -7.0 },{ -18.0, 0.0 },{ -13.0, 0.0 },{ -10.0, 8.0 },{ 10.0, 8.0 },{ 13.0, 0.0 },{ 18.0, 0.0 },{ 18.0, -7.0 } };
GLfloat car2_front_window[4][2] = { { -10.0, 0.0 },{ -8.0, 6.0 },{ -2.0, 6.0 },{ -2.0, 0.0 } };
GLfloat car2_back_window[4][2] = { { 0.0, 0.0 },{ 0.0, 6.0 },{ 8.0, 6.0 },{ 10.0, 0.0 } };
GLfloat car2_front_wheel[8][2] = { { -11.0, -11.0 },{ -13.0, -8.0 },{ -13.0, -7.0 },{ -11.0, -4.0 },{ -7.0, -4.0 },{ -5.0, -7.0 },{ -5.0, -8.0 },{ -7.0, -11.0 } };
GLfloat car2_back_wheel[8][2] = { { 7.0, -11.0 },{ 5.0, -8.0 },{ 5.0, -7.0 },{ 7.0, -4.0 },{ 11.0, -4.0 },{ 13.0, -7.0 },{ 13.0, -8.0 },{ 11.0, -11.0 } };
GLfloat car2_light1[3][2] = { { -18.0, -1.0 },{ -17.0, -2.0 },{ -18.0, -3.0 } };
GLfloat car2_light2[3][2] = { { -18.0, -4.0 },{ -17.0, -5.0 },{ -18.0, -6.0 } };

GLfloat car2_color[7][3] = {
	{ 100 / 255.0f, 141 / 255.0f, 159 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f }
};

GLuint VBO_car2, VAO_car2;
void prepare_car2() {
	GLsizeiptr buffer_size = sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1) + sizeof(car2_light2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car2_body), car2_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body), sizeof(car2_front_window), car2_front_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window), sizeof(car2_back_window), car2_back_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window), sizeof(car2_front_wheel), car2_front_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel),
		sizeof(car2_back_wheel), car2_back_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel), sizeof(car2_light1), car2_light1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1), sizeof(car2_light2), car2_light2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car2);
	glBindVertexArray(VAO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car2() {
	glBindVertexArray(VAO_car2);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);

	glBindVertexArray(0);
}

// hat
#define HAT_LEAF 0
#define HAT_BODY 1
#define HAT_STRIP 2
#define HAT_BOTTOM 3

GLfloat hat_leaf[4][2] = { { 3.0, 20.0 },{ 3.0, 28.0 },{ 9.0, 32.0 },{ 9.0, 24.0 } };
GLfloat hat_body[4][2] = { { -19.5, 2.0 },{ 19.5, 2.0 },{ 15.0, 20.0 },{ -15.0, 20.0 } };
GLfloat hat_strip[4][2] = { { -20.0, 0.0 },{ 20.0, 0.0 },{ 19.5, 2.0 },{ -19.5, 2.0 } };
GLfloat hat_bottom[4][2] = { { 25.0, 0.0 },{ -25.0, 0.0 },{ -25.0, -4.0 },{ 25.0, -4.0 } };

GLfloat hat_color[4][3] = {
	{ 167 / 255.0f, 255 / 255.0f, 55 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f },
{ 255 / 255.0f, 40 / 255.0f, 33 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f }
};

GLuint VBO_hat, VAO_hat;

void prepare_hat() {
	GLsizeiptr buffer_size = sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip) + sizeof(hat_bottom);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hat_leaf), hat_leaf);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf), sizeof(hat_body), hat_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body), sizeof(hat_strip), hat_strip);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip), sizeof(hat_bottom), hat_bottom);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_hat);
	glBindVertexArray(VAO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_hat() {
	glBindVertexArray(VAO_hat);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_LEAF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_STRIP]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glBindVertexArray(0);
}

// cake
#define CAKE_FIRE 0
#define CAKE_CANDLE 1
#define CAKE_BODY 2
#define CAKE_BOTTOM 3
#define CAKE_DECORATE 4

GLfloat cake_fire[4][2] = { { -0.5, 14.0 },{ -0.5, 13.0 },{ 0.5, 13.0 },{ 0.5, 14.0 } };
GLfloat cake_candle[4][2] = { { -1.0, 8.0 } ,{ -1.0, 13.0 },{ 1.0, 13.0 },{ 1.0, 8.0 } };
GLfloat cake_body[4][2] = { { 8.0, 5.0 },{ -8.0, 5.0 } ,{ -8.0, 8.0 },{ 8.0, 8.0 } };
GLfloat cake_bottom[4][2] = { { -10.0, 1.0 },{ -10.0, 5.0 },{ 10.0, 5.0 },{ 10.0, 1.0 } };
GLfloat cake_decorate[4][2] = { { -10.0, 0.0 },{ -10.0, 1.0 },{ 10.0, 1.0 },{ 10.0, 0.0 } };

GLfloat cake_color[5][3] = {
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 255 / 255.0f, 204 / 255.0f, 0 / 255.0f },
{ 255 / 255.0f, 102 / 255.0f, 255 / 255.0f },
{ 255 / 255.0f, 102 / 255.0f, 255 / 255.0f },
{ 102 / 255.0f, 51 / 255.0f, 0 / 255.0f }
};

GLuint VBO_cake, VAO_cake;

void prepare_cake() {
	int size = sizeof(cake_fire);
	GLsizeiptr buffer_size = sizeof(cake_fire) * 5;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, size, cake_fire);
	glBufferSubData(GL_ARRAY_BUFFER, size, size, cake_candle);
	glBufferSubData(GL_ARRAY_BUFFER, size * 2, size, cake_body);
	glBufferSubData(GL_ARRAY_BUFFER, size * 3, size, cake_bottom);
	glBufferSubData(GL_ARRAY_BUFFER, size * 4, size, cake_decorate);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cake);
	glBindVertexArray(VAO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cake() {
	glBindVertexArray(VAO_cake);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_FIRE]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_CANDLE]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_DECORATE]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glBindVertexArray(0);
}

// sword

#define SWORD_BODY 0
#define SWORD_BODY2 1
#define SWORD_HEAD 2
#define SWORD_HEAD2 3
#define SWORD_IN 4
#define SWORD_DOWN 5
#define SWORD_BODY_IN 6

GLfloat sword_body[4][2] = { { -6.0, 0.0 },{ -6.0, -4.0 },{ 6.0, -4.0 },{ 6.0, 0.0 } };
GLfloat sword_body2[4][2] = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat sword_head[4][2] = { { -2.0, 0.0 },{ -2.0, 16.0 } ,{ 2.0, 16.0 },{ 2.0, 0.0 } };
GLfloat sword_head2[3][2] = { { -2.0, 16.0 },{ 0.0, 19.46 } ,{ 2.0, 16.0 } };
GLfloat sword_in[4][2] = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat sword_down[4][2] = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 4.0, -8.0 },{ -4.0, -8.0 } };
GLfloat sword_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat sword_color[7][3] = {
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_sword, VAO_sword;

void prepare_sword() {
	GLsizeiptr buffer_size = sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down) + sizeof(sword_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sword_body), sword_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body), sizeof(sword_body2), sword_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2), sizeof(sword_head), sword_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head), sizeof(sword_head2), sword_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2), sizeof(sword_in), sword_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in), sizeof(sword_down), sword_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down), sizeof(sword_body_in), sword_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_sword);
	glBindVertexArray(VAO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sword() {
	glBindVertexArray(VAO_sword);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}

GLuint VBO_cu_line, VAO_cu_line;
GLfloat cu_line[2][2];
void drawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
	cu_line[0][0] = x1; cu_line[0][1] = y1;
	cu_line[1][0] = x2; cu_line[1][1] = y2;

	ModelViewProjectionMatrix = ViewProjectionMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glGenBuffers(1, &VBO_cu_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cu_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cu_line), cu_line, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cu_line), cu_line);

	glGenVertexArrays(1, &VAO_cu_line);
	glBindVertexArray(VAO_cu_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cu_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GLfloat color[3] = { 0.0f, 1.0f, 0.0f };
	glUniform3fv(loc_primitive_color, 1, color);
	glBindVertexArray(VAO_cu_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	glBindVertexArray(0);
}

float max_win_size = win_width < win_height ? win_width : win_height;
float radius = win_width < win_height ? win_width / 2.5f : win_height / 2.5f;
int collisionDetected(float distX, float distY, float xRadius, float yRadius);

//airplane factors
int airplane_timer_rotation = 0;
int airplane_angle = 0;
int airplane_line_angle = 0;
float airplane_timer_x = 0, airplane_timer_y = max_win_size / 16;
int airplane_trans_order = 0;
int isLine = 0;
glm::mat4 initTransform(glm::mat4 ModelMatrix);

//cake 
float cake_scale_x = 1, cake_scale_y = 1, cake_ratio = 0;
int cake_trans_order = 0;
glm::mat4 shearingMat = glm::mat4x4(1.0f);

//hat 
float hat_timer_pos = radius;

//person and car
float person_scale_ratio = 1.0f;
float person_x = -(win_width / 3), person_y = 50;
float car2_x = win_width / 3, car2_y;
float person_width, car2_width;
int hit_wall = 0;
int fly_away = 0;
int fly_order = 0;
int fly_rotate = 0;
float grad;

//sword
#define SWORD_NUM 30
float sword_x[SWORD_NUM] = { 0, }, sword_y[SWORD_NUM] = { 0, };
float x_basis[SWORD_NUM] = { 0, }, y_basis[SWORD_NUM] = { 0, };
int sword_rotate[SWORD_NUM] = { 0, }, sword_prev_rotate[SWORD_NUM] = { 0, };
int sword_angle = 0;
int sword_num = 0, CREATED = 0;

void display(void) {
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	ModelMatrix = glm::mat4(1.0f);
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();

	//airplane
	if (isLine) {
		drawLine(radius * cos((float)airplane_line_angle * TO_RADIAN), radius * sin((float)airplane_line_angle * TO_RADIAN), -radius * cos((float)airplane_line_angle * TO_RADIAN), -radius * sin((float)airplane_line_angle * TO_RADIAN));
		drawLine(radius * cos((float)airplane_angle * TO_RADIAN) - 50, radius * sin((float)airplane_angle * TO_RADIAN), radius * cos((float)airplane_angle * TO_RADIAN) + 50, radius * sin((float)airplane_angle * TO_RADIAN));
		drawLine(radius * cos((float)airplane_angle * TO_RADIAN), radius * sin((float)airplane_angle * TO_RADIAN) - 50, radius * cos((float)airplane_angle * TO_RADIAN), radius * sin((float)airplane_angle * TO_RADIAN) + 50);
		drawLine(radius * cos((float)(airplane_line_angle + 60) * TO_RADIAN) - 50, radius * sin((float)(airplane_line_angle + 60) * TO_RADIAN), radius * cos((float)(airplane_line_angle + 60) * TO_RADIAN) + 50, radius * sin((float)(airplane_line_angle + 60) * TO_RADIAN));
		drawLine(radius * cos((float)(airplane_line_angle + 60) * TO_RADIAN), radius * sin((float)(airplane_angle + 60) * TO_RADIAN) - 50, radius * cos((float)(airplane_line_angle + 60) * TO_RADIAN), radius * sin((float)(airplane_line_angle + 60) * TO_RADIAN) + 50);
	}
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));

	ModelMatrix = initTransform(ModelMatrix);

	glm::mat4 MCtoWC = glm::mat4(1.0f);
	MCtoWC = glm::translate(MCtoWC, glm::vec3(radius * cos((float)airplane_angle * TO_RADIAN), radius * sin((float)airplane_angle * TO_RADIAN), 0.0f));
	glm::mat4 reflectLine = glm::mat4(1.0f);
	reflectLine = rotate(reflectLine, airplane_line_angle * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	reflectLine = scale(reflectLine, glm::vec3(1.0f, -1.0f, 1.0f));
	reflectLine = rotate(reflectLine, -(airplane_line_angle * TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));

	ModelViewProjectionMatrix = ViewProjectionMatrix * MCtoWC * ModelMatrix;	
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	ModelViewProjectionMatrix = ViewProjectionMatrix * reflectLine * MCtoWC * ModelMatrix;		
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(-1.0f, -1.0f, 1.0f));
	ModelMatrix = initTransform(ModelMatrix);

	ModelViewProjectionMatrix = ViewProjectionMatrix * MCtoWC * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();
	ModelViewProjectionMatrix = ViewProjectionMatrix * reflectLine * MCtoWC * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	//cake
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	switch (cake_trans_order) {
	case 0: case 4: case 5: case 6: case 7:
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(cake_scale_x, cake_scale_y, 1.0f));
		break;
	case 1: case 2: case 3:
		shearingMat[0][1] = 0;
		shearingMat[1][0] = cake_ratio;
		ModelMatrix *= shearingMat;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f, 3.0f, 1.0f));
		break;
	}
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cake();

	//hat
	float angle;
	float hat_size;
	glm::vec3 hat_co;
	for (angle = 0; angle < 360; angle += 30) {
		hat_co = glm::vec3(hat_timer_pos * cos((float)angle * TO_RADIAN), hat_timer_pos * sin((float)angle * TO_RADIAN), 0.0f);
		hat_size = hat_timer_pos / radius * 3.5f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		ModelMatrix = glm::translate(ModelMatrix, hat_co);
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(hat_size, hat_size, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (90 + angle) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_hat();
	}

	//car and person
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(person_x, person_y, 0.0f));

	ModelMatrix = glm::rotate(ModelMatrix, fly_rotate * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(person_scale_ratio, 1.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(person_width / 64.0f, person_width / 64.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_person();
	
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(car2_x, car2_y, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(car2_width / 18.0f, car2_width / 18.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	if (isLine) {
		drawLine(person_x + person_width, -300, person_x + person_width, 300);
		drawLine(car2_x - car2_width, -300, car2_x - car2_width, 300);
	}

	//sword
	int idx;
	if (!CREATED) {
		if (sword_angle % 15 == 0) {
			idx = sword_angle / 15;
			sword_rotate[idx] = sword_angle;
			sword_prev_rotate[idx] = sword_angle;
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, sword_angle * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_sword();

			sword_num++;
			if (sword_num == 30) CREATED = 1;
		}
		for (int i = 0; i < sword_num; i++) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(sword_x[i], sword_y[i], 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, sword_rotate[i] * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_sword();
		}
	}
	else {
		for (int i = 0; i < SWORD_NUM; i++) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(sword_x[i], sword_y[i], 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, sword_rotate[i] * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_sword();
		}
	}

	glFlush();
}

glm::mat4 initTransform(glm::mat4 ModelMatrix) {
	glm::vec3 cock_co = glm::vec3(airplane_timer_x, airplane_timer_y, 0.0f);
	ModelMatrix = glm::translate(ModelMatrix, cock_co);
	ModelMatrix = glm::rotate(ModelMatrix, airplane_timer_rotation * TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.1f));
	return ModelMatrix;
}

int collisionDetected(float distX, float distY, float xRadius, float yRadius) {
	float dist = sqrt(distX * distX + distY + distY);
	if (dist <= xRadius + yRadius)
		return 1;
	return 0;
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void reshape(int width, int height) {
	win_width = width, win_height = height;
	radius = win_width < win_height ? win_width / 2.5f : win_height / 2.5f;
	max_win_size = win_width < win_height ? win_width : win_height;

	//car and person
	person_width = win_width / 250.0f < 5 ? win_width / 250.05 : 5.0f;
	car2_width = win_width / 250.0f < 5 ? win_width / 250.05 : 5.0f;
	person_width *= 16.0f;
	car2_width *= 18.0f;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}

int outOfScreen(float x, float y) {
	if (y > win_height / 2.0f - 20) return 1;
	if (x > win_width / 2.0f - 20) return 2;
	if (y < -(win_height / 2.0f) + 20) return 3;
	if (x < -(win_width / 2.0f) + 20) return 4;

	return 0;
}

int person_reflect_angle;
void timer_scene(int timestamp_scene) {
	//airplane
	airplane_timer_rotation = (airplane_timer_rotation + 30) % 360;
	airplane_line_angle = (airplane_line_angle + 1) % 360;
	airplane_angle = (airplane_line_angle + 300) % 360;
	switch (airplane_trans_order) {
	case 0:
		airplane_timer_x -= 5;
		airplane_timer_y = airplane_timer_x + (max_win_size / 16);
		if (airplane_timer_x <= -(max_win_size / 16)) {
			airplane_timer_x = -(max_win_size / 16);
			airplane_timer_y = 0;
			airplane_trans_order = (airplane_trans_order + 1) % 4;
		}
		break;
	case 1:
		airplane_timer_x += 5;
		airplane_timer_y = -airplane_timer_x - (max_win_size / 16);
		if (airplane_timer_x >= 0) {
			airplane_timer_x = 0;
			airplane_timer_y = -(max_win_size / 16);
			airplane_trans_order = (airplane_trans_order + 1) % 4;
		}
		break;
	case 2:
		airplane_timer_x += 5;
		airplane_timer_y = airplane_timer_x - (max_win_size / 16);
		if (airplane_timer_x >= (max_win_size / 16)) {
			airplane_timer_x = (max_win_size / 16);
			airplane_timer_y = 0;
			airplane_trans_order = (airplane_trans_order + 1) % 4;
		}
		break;
	case 3:
		airplane_timer_x -= 5;
		airplane_timer_y = -airplane_timer_x + (max_win_size / 16);
		if (airplane_timer_x <= 0) {
			airplane_timer_x = 0;
			airplane_timer_y = max_win_size / 16;
			airplane_trans_order = (airplane_trans_order + 1) % 4;
		}
		break;
	}

	//cake
	switch (cake_trans_order) {
	case 0:
		cake_scale_y += 0.1f;
		if (cake_scale_y > 3.0f) {
			cake_scale_y = 3.0f;
			cake_trans_order = (cake_trans_order + 1) % 7;
		}
		break;
	case 1:
		cake_ratio += 0.1f;
		if (cake_ratio > 2) {
			cake_ratio = 2.0f;
			cake_trans_order = (cake_trans_order + 1) % 7;
		}
		break;
	case 2:
		cake_ratio -= 0.1f;
		if (cake_ratio < -2) {
			cake_ratio = -2.0f;
			cake_trans_order = (cake_trans_order + 1) % 7;
		}
		break;
	case 3:
		cake_ratio += 0.1f;
		if (cake_ratio > 0) {
			cake_ratio = 0.0f;
			cake_trans_order = (cake_trans_order + 1) % 7;
		}
		break;
	case 4:
		cake_scale_x += 0.1f;
		if (cake_scale_x > 3.0f) {
			cake_scale_x = 3.0f;
			cake_trans_order = (cake_trans_order + 1) % 7;
		}
		break;
	case 5:
		cake_scale_y -= 0.1f;
		if (cake_scale_y < 1.0f) {
			cake_scale_y = 1.0f;
			cake_trans_order = (cake_trans_order + 1) % 7;
		}
		break;
	case 6:
		cake_scale_x -= 0.1f;
		if (cake_scale_x < 1.0f) {
			cake_scale_x = 1.0f;
			cake_trans_order = (cake_trans_order + 1) % 7;
		}
		break;
	}

	//hat
	hat_timer_pos -= 5.0f;
	if (hat_timer_pos < 0) {
		hat_timer_pos = radius;
	}

	//car and person
	float tmpX = person_x, tmpY = person_y;

	if (!fly_away) {
		if (collisionDetected(person_x - car2_x, 0, person_width, car2_width)) {
			person_scale_ratio -= ((1.0f / 3.0f) / ((person_width / 3.0f) / 10));
			car2_x -= 10;
		}
		else car2_x -= 20;

		if ((car2_x - car2_width / 2.0f) - (person_x + person_width / 2.0f) <= person_width / 3.0f) {
			fly_away = 1;
			fly_order = 0;
			tmpX = person_x;
			tmpY = person_y;

			person_reflect_angle = rand() % 60;
			person_reflect_angle = rand() % 2 ? 180 - person_reflect_angle : 180 + person_reflect_angle;
			grad = tan(person_reflect_angle * TO_RADIAN);
		}
	}
	else {
		fly_rotate = (fly_rotate + 50) % 360;
		switch (fly_order) {
		case 0:
			person_x += 30.0f * cos(person_reflect_angle * TO_RADIAN);
			person_y = grad * (person_x - tmpX) + tmpY;

			if ((hit_wall = outOfScreen(person_x, person_y))) {
				fly_order = (fly_order + 1) % 3;
				tmpX = person_x;
				tmpY = person_y;

				person_reflect_angle = rand() % 60;

				if (hit_wall == 1) person_reflect_angle = 180 + person_reflect_angle;
				if (hit_wall == 3) person_reflect_angle = 360 - person_reflect_angle;
				else if (hit_wall == 4 && grad > 0) person_reflect_angle = 360 - person_reflect_angle;

				grad = tan(person_reflect_angle * TO_RADIAN);
			}
			break;
		case 1:
			person_x += 30.0f * cos(person_reflect_angle * TO_RADIAN);

			person_y = grad * (person_x - tmpX) + tmpY;
			if ((hit_wall = outOfScreen(person_x, person_y)) > 0) {
				fly_order = (fly_order + 1) % 3;
				tmpX = person_x;
				tmpY = person_y;

				person_reflect_angle = rand() % 60;

				switch (hit_wall) {
				case 1:
					if (grad > 0) person_reflect_angle = 360 - person_reflect_angle;
					else person_reflect_angle += 180;
					break;
				case 2:
					if (grad > 0) person_reflect_angle = 180 - person_reflect_angle;
					else person_reflect_angle += 180;
					break;
				case 3:
					if (grad > 0) person_reflect_angle = 180 - person_reflect_angle;
					else person_reflect_angle = person_reflect_angle;
					break;
				case 4:
					if (grad < 0) person_reflect_angle = person_reflect_angle;
					else person_reflect_angle = 360 - person_reflect_angle;
					break;
				}
				grad = tan(person_reflect_angle * TO_RADIAN);
			}
			break;
		case 2:
			person_x += 30.0f * cos(person_reflect_angle * TO_RADIAN);

			person_y = grad * (person_x - tmpX) + tmpY;
			if (outOfScreen(person_x, person_y) > 0) {
				fly_order = (fly_order + 1) % 3;
				person_x = -(win_width / 3.0f);
				person_y = 50.0f;
				car2_x = win_width / 3.0f;
				person_scale_ratio = 1.0f;
				fly_rotate = 0;
				fly_away = 0;
			}
			break;
		}
	}

	//sword
	//sword
	sword_angle = (sword_angle + 5) % 360;
	int sword_hit_wall;
	float sword_ro, sword_grad;
	for (int i = 0; i < sword_num; i++) {
		sword_x[i] += 10 * cos(sword_rotate[i] * TO_RADIAN);
		sword_y[i] = tan(sword_rotate[i] * TO_RADIAN) * (sword_x[i] - x_basis[i]) + y_basis[i];
		sword_hit_wall = outOfScreen(sword_x[i], sword_y[i]);
		if (sword_hit_wall) {
			sword_ro = rand() % 60;

			switch (sword_hit_wall) {
			case 1:
				if (sword_rotate[i] < 90) sword_ro = 360 - sword_ro;
				else sword_ro += 180;
				break;
			case 2:
				if (sword_rotate[i] < 90) sword_ro = 180 - sword_ro;
				else sword_ro += 180;

				break;
			case 3:
				if (sword_rotate[i] < 270) sword_ro = 180 - sword_ro;
				else sword_ro = sword_ro;
				break;
			case 4:
				if (sword_rotate[i] < 180) sword_ro = sword_ro;
				else sword_ro = 360 - sword_ro;
				break;
			}
			sword_prev_rotate[i] = sword_rotate[i];
			sword_rotate[i] = sword_ro;
			x_basis[i] = sword_x[i];
			y_basis[i] = sword_y[i];
		}
	}
	glutPostRedisplay();
	glutTimerFunc(40, timer_scene, 1);
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);

	// Delete others here too!!!
	glDeleteVertexArrays(1, &VAO_car2);
	glDeleteBuffers(1, &VBO_car2);

	glDeleteVertexArrays(1, &VAO_sword);
	glDeleteBuffers(1, &VBO_sword);

	glDeleteVertexArrays(1, &VAO_cake);
	glDeleteBuffers(1, &VBO_cake);

	glDeleteVertexArrays(1, &VAO_hat);
	glDeleteBuffers(1, &VBO_hat);

	glDeleteVertexArrays(1, &VAO_person);
	glDeleteBuffers(1, &VBO_person);

	glDeleteVertexArrays(1, &VAO_cu_line);
	glDeleteBuffers(1, &VBO_cu_line);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutTimerFunc(40, timer_scene, 1);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(102 / 255.0f, 204 / 255.0f, 255 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_person();
	prepare_airplane();
	prepare_car2();
	prepare_hat();
	prepare_cake();
	prepare_sword();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;
	
	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for 20171618 KimSohung\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 2DObjects_GLSL_3.0.1.3";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC' "
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 700);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


