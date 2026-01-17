#include "framework.h"
#include <iostream>

using std::cout;
using std::cin;
using std::endl;

// --- WINDOW SIZE ---
const int winWidth = 600, winHeight = 600;

// --- SHADERS ---
const char* vertSource = R"(
	#version 330
	precision highp float;

	uniform mat4 MVP;
	layout(location = 0) in vec2 vertexPosition;

	out vec2 texCoord; 

	void main() {
		texCoord = (vertexPosition + vec2(1, 1)) / 2.0;
		gl_Position = MVP * vec4(vertexPosition, 0, 1);
	}
)";

const char* fragSource = R"(
	#version 330
	precision highp float;

	uniform vec3 color;
	uniform sampler2D samplerUnit;
	uniform int useTexture;       // 1 = texture | 0 = color

	in vec2 texCoord;
	out vec4 fragmentColor;

	void main() {
		if (useTexture == 1) {
			fragmentColor = texture(samplerUnit, texCoord);
		} else {
			fragmentColor = vec4(color, 1);
		}
	}
)";

// --- HELPER FUNCTIONS ---
void CreateHead(Geometry<vec2>& vertices, int& headStart, int& headCount) {
	headStart = (int)vertices.Vtx().size();
	int perimeterPoints = 100;
	float r = 0.5f;
	vertices.Vtx().push_back(vec2(0, 0));

	for (int i = 0; i < perimeterPoints; i++) {
		float phi = i * 2.0f * (float)M_PI / (float)(perimeterPoints - 1);
		float x = r * cosf(phi);
		float y = r * sinf(phi);
		vertices.Vtx().push_back(vec2(x, y));
	}

	headCount = (int)vertices.Vtx().size() - headStart;
}

void CreateEyes(Geometry<vec2>& vertices, int& eyesStart, int& eyesCount) {
	eyesStart = (int)vertices.Vtx().size();
	eyesCount = 2;

	vertices.Vtx().push_back(vec2(-0.2f, 0.2f));
	vertices.Vtx().push_back(vec2(0.2f, 0.2f));
}

void CreateNose(Geometry<vec2>& vertices, int& noseStart, int& noseCount) {
	noseStart = (int)vertices.Vtx().size();
	noseCount = 2;

	vertices.Vtx().push_back(vec2(0.0f, 0.05f));
	vertices.Vtx().push_back(vec2(0.0f, -0.05f));
}

void CreateMouth(Geometry<vec2>& vertices, int& mouthStart, int& mouthCount) {
	mouthStart = (int)vertices.Vtx().size();
	mouthCount = 11;

	float r_mouth = 0.4f;
	float startAngle = (float)M_PI + ((float)M_PI / 4.0f);
	float endAngle = 2.0f * (float)M_PI - ((float)M_PI / 4.0f);

	for (int i = 0; i < mouthCount; i++) {
		float t = (float)i / (float)(mouthCount - 1);
		float phi = startAngle * (1 - t) + endAngle * t;
		float x = r_mouth * cosf(phi);
		float y = r_mouth * sinf(phi) + 0.1f;
		vertices.Vtx().push_back(vec2(x, y));
	}
}

// --- APPLICATION ---
class SmileDemoApp : public glApp {
	GPUProgram* gpuProgram = nullptr;
	Geometry<vec2>* vertices = nullptr; 
	Geometry<vec2>* background = nullptr;
	Texture* bgTexture = nullptr;

	int headStart = 0, headCount = 0;
	int eyesStart = 0, eyesCount = 0;
	int noseStart = 0, noseCount = 0;
	int mouthStart = 0, mouthCount = 0;

	// Transformation matrix
	mat4 M = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	// State variables for transformations
	bool transToggle = true, scaleToggle = true;

public:
	SmileDemoApp() : glApp("Smile Demo App") {}

	void onInitialization() {
		gpuProgram = new GPUProgram();
		gpuProgram->create(vertSource, fragSource);	// Call shaders

		// Backgorund incialization
		background = new Geometry<vec2>();
		background->Vtx().push_back(vec2(-1, -1));
		background->Vtx().push_back(vec2(1, -1));
		background->Vtx().push_back(vec2(1, 1));
		background->Vtx().push_back(vec2(-1, 1));
		background->updateGPU();

		bgTexture = new Texture("bg.png");

		// Smile incialization
		vertices = new Geometry<vec2>();
		gpuProgram = new GPUProgram();

		CreateHead(*vertices, headStart, headCount);
		CreateEyes(*vertices, eyesStart, eyesCount);
		CreateNose(*vertices, noseStart, noseCount);
		CreateMouth(*vertices, mouthStart, mouthCount);

		glPointSize(15.0f);
		glLineWidth(10.0f);

		vertices->updateGPU();

		// Inject shaders
		gpuProgram->create(vertSource, fragSource);
	}

	void onDisplay() {
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);	// Gray background
		glClear(GL_COLOR_BUFFER_BIT);			// Clear the screen

		if (!gpuProgram) return;

		gpuProgram->Use();	// Activating shaders

		// --- Draw backgorund ---
		gpuProgram->setUniform(M, "MVP");
		gpuProgram->setUniform(1, "useTexture");
		gpuProgram->setUniform(0, "samplerUnit");

		if (bgTexture) bgTexture->Bind(0);

		if (background) {
			background->Bind();
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		// --- Draw Smile ---
		gpuProgram->setUniform(M, "MVP");		// Set transformation matrix
		gpuProgram->setUniform(0, "useTexture");// Disable texture for colors

		if (vertices) {
			vertices->Bind();	// Activate vertex array object

			gpuProgram->setUniform(vec3(1.0f, 1.0f, 0.0f), "color");	// Color yellow
			glDrawArrays(GL_TRIANGLE_FAN, headStart, headCount);		// --- 1. Head Fill ---

			gpuProgram->setUniform(vec3(0.0f, 0.0f, 0.0f), "color");	// Color black
			glDrawArrays(GL_LINE_STRIP, headStart + 1, headCount - 1);	// --- 1. Head Border ---
			glDrawArrays(GL_POINTS, eyesStart, eyesCount);				// --- 2. Eyes ---
			glDrawArrays(GL_LINES, noseStart, noseCount);				// --- 3. Nose ---
			glDrawArrays(GL_LINE_STRIP, mouthStart, mouthCount);		// --- 4. Mouth ---
		}
	}

	void onKeyboard(int key) {
		switch (key) {
		case 'i': case 'I':
			M = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
			transToggle = true;
			cout << "Reseted!" << endl;
			break;

		case 't': case 'T':
			if (transToggle) {
				M = M * translate(vec3(0.1f, 0.1f, 0.0f));
				cout << "Translate: +0.1, +0.1" << endl;
			}
			else {
				M = M * translate(vec3(-0.1f, -0.1f, 0.0f));
				cout << "Translate: -0.1, -0.1 (Back)" << endl;
			}
			transToggle = !transToggle;
			break;

		case 's': case 'S':
			if (scaleToggle) {
				M = M * scale(vec3(1.2f, 1.5f, 1.0f));
				cout << "Scale: X*1.2, Y*1.5" << endl;
			}
			else {
				M = M * scale(vec3(0.833f, 0.666f, 1.0f));
				cout << "Scale: X*0.83, Y*0.66 (Back)" << endl;
			}
			scaleToggle = !scaleToggle;
			break;

		case 'r': case 'R':
			M = M * rotate(45.0f * (float)M_PI / 180.0f, vec3(0, 0, 1));
			cout << "Rotate: 45 degrees" << endl;
			break;

		case 'q': case 'Q':
			exit(0);
			break;
		}
		refreshScreen();
	}

	~SmileDemoApp() {
		if(!gpuProgram) delete gpuProgram;
		if(!vertices) delete vertices;
		if (!background) delete background;
		if (!bgTexture) delete bgTexture;
	}
};

SmileDemoApp app;