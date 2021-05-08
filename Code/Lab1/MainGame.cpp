#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>


Transform transform;

MainGame::MainGame()
{
	_gameState = GameState::PLAY;
	Display* _gameDisplay = new Display(); //new display
	Shader fogShader();
	Shader toonShader();
	Shader rimShader();
	Shader geoShader();
	Shader myShader();
	//Audio* audioDevice();
}

MainGame::~MainGame()
{
}

void MainGame::run()
{
	initSystems(); 
	gameLoop();
}

void MainGame::initSystems()
{
	_gameDisplay.initDisplay(); 
	//whistle = audioDevice.loadSound("..\\res\\bang.wav");
	//backGroundMusic = audioDevice.loadSound("..\\res\\background.wav");
	
	mesh1.loadModel("..\\res\\monkey3.obj");
	mesh2.loadModel("..\\res\\skull.obj");
	mesh3.loadModel("..\\res\\impostor.obj");
	fogShader.init("..\\res\\fogShader.vert", "..\\res\\fogShader.frag"); //new shader
	toonShader.init("..\\res\\shaderToon.vert", "..\\res\\shaderToon.frag"); //new shader
	rimShader.init("..\\res\\shaderRim.vert", "..\\res\\shaderRim.frag");
	eMapping.init("..\\res\\shaderReflection.vert", "..\\res\\shaderReflection.frag");
	myShader.init("..\\res\\myShader.vert", "..\\res\\myShader.frag");

	geoShader.initGeo();

	myCamera.initCamera(glm::vec3(0, 0, -5), 70.0f, (float)_gameDisplay.getWidth()/_gameDisplay.getHeight(), 0.01f, 1000.0f);

	counter = 1.0f;
	myCounter = 0.0f;
	counterFlag = true;

	vector<std::string> faces
	{
		"..\\res\\skybox\\right.jpg",
		"..\\res\\skybox\\left.jpg",
		"..\\res\\skybox\\top.jpg",
		"..\\res\\skybox\\bottom.jpg",
		"..\\res\\skybox\\front.jpg",
		"..\\res\\skybox\\back.jpg"
	};

	skybox.init(faces);
}

void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		processInput();
		drawGame();
		collision(mesh1.getSpherePos(), mesh1.getSphereRadius(), mesh2.getSpherePos(), mesh2.getSphereRadius());
		//playAudio(backGroundMusic, glm::vec3(0.0f,0.0f,0.0f));
	}
}

void MainGame::processInput()
{
	SDL_Event evnt;

	while(SDL_PollEvent(&evnt)) //get and process events
	{
		switch (evnt.type)
		{
			case SDL_QUIT:
				_gameState = GameState::EXIT;
				break;
		}
	}
	
}


bool MainGame::collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad)
{
	float distance = glm::sqrt((m2Pos.x - m1Pos.x)*(m2Pos.x - m1Pos.x) + (m2Pos.y - m1Pos.y)*(m2Pos.y - m1Pos.y) + (m2Pos.z - m1Pos.z)*(m2Pos.z - m1Pos.z));

	if (distance < (m1Rad + m2Rad))
	{
		//audioDevice.setlistener(myCamera.getPos(), m1Pos); //add bool to mesh
		//playAudio(whistle, m1Pos);
		return true;
	}
	else
	{
		return false;
	}
}

//void MainGame::playAudio(unsigned int Source, glm::vec3 pos)
//{
//	
//	ALint state; 
//	alGetSourcei(Source, AL_SOURCE_STATE, &state);
//	/*
//	Possible values of state
//	AL_INITIAL
//	AL_STOPPED
//	AL_PLAYING
//	AL_PAUSED
//	*/
//	if (AL_PLAYING != state)
//	{
//		audioDevice.playSound(Source, pos);
//	}
//}

void MainGame::linkFogShader()
{
	//fogShader.setMat4("u_pm", myCamera.getProjection());
	//fogShader.setMat4("u_vm", myCamera.getProjection());
	fogShader.setFloat("maxDist", 20.0f);
	fogShader.setFloat("minDist", 0.0f);
	fogShader.setVec3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f));
}

void MainGame::linkToon()
{
	toonShader.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
}

void MainGame::linkGeo()
{
	float randX = ((float)rand() / (RAND_MAX));
	float randY = ((float)rand() / (RAND_MAX));
	float randZ = ((float)rand() / (RAND_MAX));
	// Frag: uniform float randColourX; uniform float randColourY; uniform float randColourZ;
	geoShader.setFloat("randColourX", randX);
	geoShader.setFloat("randColourY", randY);
	geoShader.setFloat("randColourZ", randZ);
	// Geom: uniform float time;
	geoShader.setFloat("time", counter);
}

void MainGame::linkRimLighting()
{
	glm::vec3 camDir;
	camDir = mesh2.getSpherePos() - myCamera.getPos();
	camDir = glm::normalize(camDir);
	rimShader.setMat4("u_pm", myCamera.getProjection());
	rimShader.setMat4("u_vm", myCamera.getView());
	rimShader.setMat4("model", transform.GetModel());
	rimShader.setMat4("view", myCamera.getView());
	rimShader.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
}

void MainGame::linkEmapping()
{
	eMapping.setMat4("projection", myCamera.getProjection());
	eMapping.setMat4("view", myCamera.getView());
	eMapping.setMat4("model", transform.GetModel());
	eMapping.setVec3("cameraPos", myCamera.getPos());
}

// myShader.setVec3("viewPos", myCamera.getPos());
// for somereason this is not working, just gives unable to load shader error
// Really confusing because in other similar cases it just works...

void MainGame::linkMyShader()
{
	if (counterFlag)
		myCounter = myCounter + 0.02f;
	else
		myCounter = myCounter - 0.02f;
	if (myCounter > 1)
	{
		counterFlag = false;
		myCounter = 1;
	}
	else if (myCounter < 0)
	{
		counterFlag = true;
		myCounter = 0;
	}
	myShader.setFloat("myCounter", myCounter);
}

void MainGame::drawGame()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_gameDisplay.clearDisplay(0.8f, 0.8f, 0.8f, 1.0f); //sets our background colour
	
	//linkFogShader();
	//linkToon();
	//linkRimLighting();
	linkMyShader();

	Texture texture("..\\res\\rainbow.jpg"); //load texture
	Texture texture1("..\\res\\water.jpg"); //load texture
	Texture texture2("..\\res\\ice.jpg"); //load texture
	Texture texture3("..\\res\\fire.jpg"); //load textureTexture texture
	Texture texture4("..\\res\\brain_texture3.jpg"); //load texture
	
	transform.SetPos(glm::vec3(2.0, 0.0, -sinf(counter) * 3));
	transform.SetRot(glm::vec3(0.0, -counter * 5, 0.0));
	transform.SetScale(glm::vec3(0.4, 0.4, 0.4));

	geoShader.Bind();
	texture3.Bind(0);
	linkGeo();
	geoShader.Update(transform, myCamera);

	/*toonShader.Bind();
	linkToon();
	toonShader.Update(transform, myCamera);*/

	mesh1.draw();
	mesh1.updateSphereData(*transform.GetPos(), 0.62f);

	transform.SetPos(glm::vec3(-2.0, -sinf(counter), 0.0));
	transform.SetRot(glm::vec3(0.0, counter * 5, 0.0));
	transform.SetScale(glm::vec3(0.4, 0.4, 0.4));
		
	eMapping.Bind();
	linkEmapping();

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID);

	mesh2.draw();
	mesh2.updateSphereData(*transform.GetPos(), 0.62f);

	skybox.draw(&myCamera);

	transform.SetPos(glm::vec3(0.0, -sinf(counter), 0.0));
	transform.SetRot(glm::vec3(0.0, counter * 5, 0.0));
	transform.SetScale(glm::vec3(0.4, 0.4, 0.4));

	myShader.Bind();
	texture2.Bind(0);
	myShader.Update(transform, myCamera);
	mesh3.draw();
	mesh3.updateSphereData(*transform.GetPos(), 0.62f);

	counter = counter + 0.02f;

	glEnableClientState(GL_COLOR_ARRAY); 
	glEnd();


	_gameDisplay.swapBuffer();	
	

} 