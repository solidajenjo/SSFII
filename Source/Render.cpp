#include "Game.h"
#include "Globals.h"
#include "Render.h"
#include "Sprite.h"
#include "ExternalLibraries/SDL/include/SDL.h"
#include "ExternalLibraries/imgui/imgui.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_opengl3.h"
#include "ExternalLibraries/glew-2.1.0/include/GL/glew.h"
#include "ExternalLibraries/MathGeoLib/include/Math/float4x4.h"
#include "ExternalLibraries/MathGeoLib/include/Math/float2.h"


bool Render::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{			
		LOG("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow("Super Street Fighter II", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
		if (window == nullptr)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			screenSurface = SDL_GetWindowSurface(window);
			SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
			SDL_UpdateWindowSurface(window);

		}
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		LOG("Error Initializating GLEW");
		return false;
	}

	program = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	char* vsCode = readFile("default.vs");

	glShaderSource(vs, 1, &vsCode , 0);
	glCompileShader(vs);
	GLint isCompiled;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* infoLog = (GLchar*)malloc(sizeof(char) * (maxLength + 1));
		glGetShaderInfoLog(vs, maxLength, &maxLength, &infoLog[0]);
		glDeleteShader(vs);
		LOG("Vertex shader compilation error: %s", infoLog);
		free(infoLog);
		return false;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fsCode = readFile("default.fs");

	glShaderSource(fs, 1, &fsCode, 0);
	glCompileShader(fs);
	
	isCompiled = 0;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* infoLog = (GLchar*)malloc(sizeof(char) * (maxLength + 1));
		glGetShaderInfoLog(fs, maxLength, &maxLength, &infoLog[0]);
		glDeleteShader(fs);
		LOG("Fragment shader compilation error: %s", infoLog);
		free(infoLog);
		return false;
	}

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* infoLog = (GLchar*)malloc(sizeof(char) * (maxLength + 1));
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		glDeleteProgram(program);
		glDeleteShader(vs);
		glDeleteShader(fs);
		LOG("Program Linking error: %s", infoLog);
		free(infoLog);
		return false;
	}
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glClearDepth(1.0f);
	glClearColor(.5f, 0.5f, .5f, 1.f);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	return true;
}

bool Render::PreUpdate()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return true;
}

bool Render::Update()
{
	return true;
}

void Render::RenderSprite(const Sprite* sprite, const float3 &pos, float offsetH, float offsetV, bool flip) const
{
	glUseProgram(program);
	float4x4 model;
	float3 spritePos;
	float dir = 1.f;
	if (flip)
	{
		spritePos = float3((((pos.x - offsetH) / SCREEN_WIDTH) * 2.f) - 1.f, (((pos.y + offsetV) / SCREEN_HEIGHT) * 2.f) - 1.f, .0f);
		dir = -1.f;
	}
	else
	{
		spritePos = float3((((pos.x + offsetH) / SCREEN_WIDTH) * 2.f) - 1.f, ((pos.y / SCREEN_HEIGHT) * 2.f) - 1.f, .0f);		
	}
	model = model.FromTRS(spritePos,
		float4x4::identity, 
		float3((dir * sprite->width) / (float)SCREEN_WIDTH, sprite->height / (float)SCREEN_HEIGHT , 1.f) * 2.f);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, float4x4::identity.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, float4x4::identity.ptr());
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, sprite->vbo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)(sizeof(float) * 3 * 6)    // 3 float 6 vertices for jump positions
	);
	glBindTexture(GL_TEXTURE_2D, sprite->texture);
	GLint loc = glGetUniformLocation(program, "texture");
	glUniform1i(loc, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6); 

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Render::DrawBox(const float2 &minPoint, const float2 &maxPoint, bool red)
{
	float2 mPoint, MPoint;
	mPoint.x = ((minPoint.x / SCREEN_WIDTH) * 2.f) - 1.f;
	mPoint.y = ((minPoint.y / SCREEN_HEIGHT) * 2.f) - 1.f;

	MPoint.x = ((maxPoint.x / SCREEN_WIDTH) * 2.f) - 1.f;
	MPoint.y = ((maxPoint.y / SCREEN_HEIGHT) * 2.f - 1.f);

	if (red)
	{
		glBegin(GL_LINE_STRIP);
		glColor3f(1.f, .0f, .0f);
		glVertex3f(mPoint.x, mPoint.y, -1.f);
		glColor3f(1.f, .0f, .0f);
		glVertex3f(mPoint.x, MPoint.y, -1.f);
		glColor3f(1.f, .0f, .0f);
		glVertex3f(MPoint.x, MPoint.y, -1.f);
		glColor3f(1.f, .0f, .0f);
		glVertex3f(MPoint.x, mPoint.y, -1.f);
		glColor3f(1.f, .0f, .0f);
		glVertex3f(mPoint.x, mPoint.y, -1.f);
		glEnd();
	}
	else
	{
		glBegin(GL_LINE_STRIP);
		glColor3f(.0f, 1.f, .0f);
		glVertex3f(mPoint.x, mPoint.y, -1.f);
		glColor3f(.0f, 1.f, .0f);
		glVertex3f(mPoint.x, MPoint.y, -1.f);
		glColor3f(.0f, 1.f, .0f);
		glVertex3f(MPoint.x, MPoint.y, -1.f);
		glColor3f(.0f, 1.f, .0f);
		glVertex3f(MPoint.x, mPoint.y, -1.f);
		glColor3f(.0f, 1.f, .0f);
		glVertex3f(mPoint.x, mPoint.y, -1.f);
		glEnd();
	}
}

bool Render::PostUpdate()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(window);
	return true;
}

bool Render::Quit()
{
	if (renderer != nullptr)
	{
		SDL_DestroyRenderer(renderer);
	}
	SDL_FreeSurface(screenSurface);
	SDL_DestroyWindow(window);
	LOG("Render quit");
	return true;
}

char* Render::readFile(const char * name) const
{
	char* data = nullptr;
	FILE* file = nullptr;
	fopen_s(&file, name, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		rewind(file);
		data = (char*)malloc(size + 1);
		fread(data, 1, size, file);
		data[size] = 0;
		fclose(file);
	}
	return data;
}