#include "Tema1.h"

#include <vector>
#include <iostream>
#include <ctime>
#include <cmath>
#include <cstdlib>

#include <Core/Engine.h>
#include "Transform2D.h"
#include "Object2Dv2.h"

using namespace std;

typedef struct
{
	float balloonX;
	float balloonY;
	float balloonScale = 1;
	int colorRed;
	int upSpeed;
	int sideSpeed;
	int direction;
	int releaseTime;
	bool released = false;
	bool hit = false;

}balloon;

typedef struct
{
	float shurikenX;
	float shurikenY;
	float shurikenScale = 1;
	int xSpeed;
	int releaseTime;
	bool released = false;
	bool hit = false;

}shuriken;

vector<balloon> balloons;
vector<shuriken> shurikens;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	Mesh* shuriken = Object2Dv2::CreateShuriken("shuriken", shurikenWidth);
	AddMeshToList(shuriken);

	Mesh* bow = Object2Dv2::CreateBow("bow", bowRadius, bowRadius);
	AddMeshToList(bow);

	// Pentru balon creez 3 mesh-uri. Triunghi , balon si linie.
	Mesh* triangle_balloon = new Mesh("triangle_balloon");
	Mesh* line = new Mesh("line");
	Mesh* balloon = Object2Dv2::CreateBaloon("balloon", balloonOrigin, balloonRadiusX, balloonRadiusY, colorRed, triangle_balloon, line);
	AddMeshToList(balloon);
	AddMeshToList(triangle_balloon);
	AddMeshToList(line);

	Mesh* triangle_balloon_yellow = new Mesh("triangle_balloon_yellow");
	Mesh* line_yellow = new Mesh("line_yellow");
	Mesh* balloon_yellow = Object2Dv2::CreateBaloon("balloon_yellow", balloonOrigin, balloonRadiusX, balloonRadiusY, !colorRed, triangle_balloon_yellow, line_yellow);
	AddMeshToList(balloon_yellow);
	AddMeshToList(triangle_balloon_yellow);
	AddMeshToList(line_yellow);

	// Sageata este formata din doua mesh-uri. Un triunghi si un dreptunghi.
	Mesh* triangle = new Mesh("triangle_arrow");
	Mesh* rectangle = Object2Dv2::CreateArrow("rectangle", false, arrowLength, arrowTipWidth, arrowHigh, triangle);
	AddMeshToList(triangle);
	AddMeshToList(rectangle);

	// Functia "CreateArrow" poate crea si doar un dreptunghi fara triunghi.
	// Aceasta functionalitate o folosesc pentru crearea power barului.
	Mesh* powerBar = Object2Dv2::CreateArrow("powerBar", true, arrowLength, arrowTipWidth, arrowHigh, NULL);
	AddMeshToList(powerBar);

	Text = new TextRenderer(resolution.x, resolution.y);
	Text->Load("Source/TextRenderer/Fonts/Arial.ttf", 18);
}

// Functie pentru randare text pe ecran.
// Am schimbat functia RenderText ca textul sa fie aliniat dupa centrul sau nu dupa stanga sa.
// Am adaugat doua argumente noi pentru a retine lungimea si inaltimea textului.
void Tema1::drawHud()
{
	glm::ivec2 resolution = window->GetResolution();
	float textWidth, textHight;

	// Pe ecranul de joc apare constant scorul si numarul de vieti.
	if (gameStart) {
		std::string scoreText = std::to_string(score);
		std::string lifeText = std::to_string(lifeCount);

		Text->RenderText(&textWidth, &textHight, "Score: " + scoreText, 50, 5.0f, 1.0f, glm::vec3(0, 0, 0));
		Text->RenderText(&textWidth, &textHight, "Lives left: " + lifeText, 50, 25.0f, 1.0f, glm::vec3(0, 0, 0));
	}
	// Ecranul de new game contine doua butoane: New Game si Exit Game. 
	// De asemenea apare si titlul temei.
	// Daca se da hover pe acestea se schimba culoarea la text.
	else if (newGameScreen) {
		Text->RenderText(&textWidth, &textHight, "BOW  AND  ARROW", resolution.x / 2, resolution.y / 2 - 200, 3, glm::vec3(0, 0, 0));
		Text->RenderText(&textWidthNewGame, &textHightNewGame, "New Game", resolution.x / 2, resolution.y / 2 - 100, 2, newGamesNewGameColor);
		Text->RenderText(&textWidthExitGame, &textHightExitGame, "Exit Game", resolution.x / 2, resolution.y / 2 - 50, 2, newGamesExitGameColor);
	}
	// Ecranul de game over contine doua butoane: New Game si Exit Game. De asemenea,
	// cand numarul de vieti ajunge la 0 se face tranzitia de la ecranul precedent
	// la un ecran negru, dupa care apare "incet" textul "YOU DIED".
	// Daca se da hover pe acestea se schimba culoarea la text.
	// La inceperea unui nou joc se reseteaza variabilele de schimbarea dificultatii jocului
	// si vectorii de baloane si shurikenuri sunt goliti
	else if (gameOverScreen && backgoundColorR <= 0 && backgoundColorG <= 0 && backgoundColorB <= 0) {
		if (youDiedColor < 0.75) {
			youDiedColor += 0.002;
		}
		Text->RenderText(&textWidth, &textHight, "YOU DIED", resolution.x / 2, resolution.y / 2 - 200, 3, glm::vec3(youDiedColor, 0, 0));
		if (youDiedColor >= 0.75) {
			std::string scoreText = std::to_string(score);
			Text->RenderText(&textWidthNewGame, &textHightNewGame, "New Game", resolution.x / 2, resolution.y / 2 - 100, 2, exitGamesNewGameColor);
			Text->RenderText(&textWidthExitGame, &textHightExitGame, "Exit Game", resolution.x / 2, resolution.y / 2 - 50, 2, exitGamesExitGameColor);
			Text->RenderText(&textWidth, &textHight, "Score: " + scoreText, resolution.x / 2, resolution.y / 2 , 2, glm::vec3(0.75, 0, 0));
		}
	}
	// Ecranul de pauza este asemanator cu cel de new game cu exceptia faptului
	// ca exista acum si butonul de Continue.
	// Variabilele pentru schimbarea jocului si vectorii de baloane si shurikenuri nu sunt resetate daca se apasa pe continue.
	else if (pauseGameScreen) {
		Text->RenderText(&textWidthContinue, &textHightContinue, "Continue", resolution.x / 2, resolution.y / 2 - 150, 2, newGamesContinueColor);
		//cout << textWidthContinue << " " << textHightContinue << endl;
		Text->RenderText(&textWidthNewGame, &textHightNewGame, "New Game", resolution.x / 2, resolution.y / 2 - 100, 2, newGamesNewGameColor);
		Text->RenderText(&textWidthExitGame, &textHightExitGame, "Exit Game", resolution.x / 2, resolution.y / 2 - 50, 2, newGamesExitGameColor);
	}

}

void Tema1::FrameStart()
{
	
	// clears the color buffer (using the previously set color) and depth buffer
	// Backgroundul din timpul jocului e verde.
	if (gameStart) {
		backgoundColorR = 0.5;
		backgoundColorB = 0.75;
		backgoundColorG = 0;
		glClearColor(0.5, 0.75, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	// Backgroundul din ecranul new game si pause game e alb.
	else if (newGameScreen) {
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	// Backgroundul din ecranul game over e negru 
	// facandu-se tranzitia de la verde la negru mai intai.
	else if (gameOverScreen) {
		if (backgoundColorR >= 0 || backgoundColorB >= 0) {
			backgoundColorR -= 0.001;
			backgoundColorB -= 0.002;
		}
		glClearColor(backgoundColorR, backgoundColorB, backgoundColorG, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else if (pauseGameScreen) {
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

// Functie folosita la coliziunea dintre sageata si shuriken.
bool CheckCollisionPointCircle(float arrowX, float arrowY, float shurikenX, float shurikenY, float shurikenRadius) {

	// Daca distanta dintre centrul cercului si punct este mai mare decat raza cercului atunci nu este coliziune.
	// Daca este mai mica sau egala atunci este coliziune.
	if (sqrt((arrowX - shurikenX) * (arrowX - shurikenX) + (arrowY - shurikenY) * (arrowY - shurikenY)) <= shurikenRadius) {
		return true;
	}
	else {
		return false;
	}

}

// Functie folosita la coliziunea dintre arc si shuriken.
bool CheckCollisionCircles(float bowX, float bowY, float bowRadius, float shurikenX, float shurikenY, float shurikenRadius) {

	// Daca distanta dintre centrele "cercurilor" este mai mare decat razele cercurilor adunate atunci nu este coliziune.
	// Daca este mai mica sau egala atunci este coliziune.
	if (sqrt((bowX - shurikenX) * (bowX - shurikenX) + (bowY - shurikenY) * (bowY - shurikenY)) <= bowRadius + shurikenRadius) {
		return true;
	}
	else {
		return false;
	}

}

// Functie folosita la coliziunea dintre sageata si baloane.
bool CheckCollisionPointElipse(float arrowX, float arrowY, float balloonX, float balloonY, float balloonRadiusX, float balloonRadiusY) {

	// Daca distanta de la centrul elipsei pana la punct este mai mare decat 1 atunci nu este coliziune. (am gasit inegalitatea pe net)
	// Daca este mai mica sau egala atunci este coliziune.
	if (((arrowX - balloonX) * (arrowX - balloonX) / (balloonRadiusX * balloonRadiusX)) + ((arrowY - balloonY) * (arrowY - balloonY) / (balloonRadiusY * balloonRadiusY)) <= 1) {
		return true;
	}
	else {
		return false;
	}

	return true;
}

void Tema1::Update(float deltaTimeSeconds)
{
	if (gameStart) {
		glLineWidth(3);
		glm::ivec2 resolution = window->GetResolution();
		angularStep += deltaTimeSeconds * 10;
		releaseTimeGlobalBalloons += 1;
		releaseTimeGlobalShurikens += 1;

		// SHURIKEN
		// Cand nu mai exita shurikenuri se creaza iar. (acestea functioneaza ca wave-uri)
		// La fiecare wave nou viteza de baza a shurikenurilor is a baloanelor creste
		if (shurikens.size() == 0) {
			waveCount++;
			shurikenXSpeedAdd += 20;
			balloonUpSpeedAdd += 20;

			// La fiecare stagiu nou se creste scale-ul shurikenurilor pana la un maxim de 4.
			// Dupa ce se termina cu scale-ul se creste numarul minim de shurikenuri create.
			if (waveCount % 5 == 0) {
				stageCount++;
				waveCount = 1;

				if (shurikenScaleAdd <= 3) {
					shurikenScaleAdd += 1;
				}
				else if (shurikenVectorAddSize < 15){
					shurikenVectorAddSize += 2;
				}
			}
			
			// Afisez in consola wave-ul si stage-ul.
			// Nu am afisat pe ecran pentru ca am considerat (in viziunea mea), ca ar distrage jucatorul.
			cout << "Stage "<<stageCount << " Wave " << waveCount << endl;
			releaseTimeGlobalShurikens = 0;
			shurikenVectorSize = rand() % shurikenVectorMaxSize + shurikenVectorAddSize;

			for (int i = 0; i < shurikenVectorSize; i++) {
				shuriken s;
				s.shurikenScale += shurikenScaleAdd;
				s.shurikenX = resolution.x + shurikenWidth * 1.41f * s.shurikenScale;
				s.shurikenY = rand() % (resolution.y - 200) + 100;
				s.xSpeed = rand() % 50 + 150 + shurikenXSpeedAdd;
				s.releaseTime = rand() % 5000 + 200;

				shurikens.push_back(s);
			}
		}

		for (int j = 0; j < shurikens.size(); j++) {
			// Se verifica coliziunea cu arcul pentru fiecare shuriken la fiecare moment de timp.
			if (CheckCollisionCircles(bowX, bowY, bowRadius, shurikens[j].shurikenX, shurikens[j].shurikenY, shurikenWidth * shurikens[j].shurikenScale) && !shurikens[j].hit) {
				lifeCount--;
				if (lifeCount == 0) {
					exitGamesNewGameColor = glm::vec3(0.75, 0, 0);
					exitGamesExitGameColor = glm::vec3(0.75, 0, 0);
					gameOverScreen = true;
					gameStart = false;
				}

				shurikens[j].hit = true;
			}

			// Fiecare shuriken pleaca la un moment de timp aleator.
			if (releaseTimeGlobalShurikens >= shurikens[j].releaseTime) {
				shurikens[j].released = true;
				shurikens[j].shurikenX -= deltaTimeSeconds * shurikens[j].xSpeed;
			}

			// Daca shurikenul a fost lovit scaleaza pana la 0 dupa care e sters.
			if (shurikens[j].hit) {
				shurikens[j].shurikenScale -= deltaTimeSeconds * (shurikenScaleAdd + 1);
			}

			// Shurikenul poate fi sters daca scale-ul acestuia e 0 sau daca a iesit din ecran.
			if (shurikens[j].shurikenX < - shurikenWidth * 1.41f * shurikens[j].shurikenScale || shurikens[j].shurikenScale <= 0) {
				shurikens.erase(shurikens.begin() + j);
			}
			else {
				// Desenarea shurikenului la locatia respectiva,
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(shurikens[j].shurikenX, shurikens[j].shurikenY);
				modelMatrix *= Transform2D::Rotate(angularStep);
				modelMatrix *= Transform2D::Scale(shurikens[j].shurikenScale, shurikens[j].shurikenScale);
				RenderMesh2D(meshes["shuriken"], shaders["VertexColor"], modelMatrix);
			}
		}

		// BALON
		// Vectorul de baloane este creat asemanator ca cel de la shurikenuri.
		if (balloons.size() == 0) {
			releaseTimeGlobalBalloons = 0;
			balloonVectorSize = rand() % balloonVectorMaxSize + 5;

			for (int i = 0; i < balloonVectorSize; i++) {
				
				balloon b;
				b.balloonX = rand() % (resolution.x - 402) + 401;
				b.balloonY = -balloonRadiusY;
				b.colorRed = rand() % 2;
				b.upSpeed = rand() % 50 + 100 + balloonUpSpeedAdd;
				b.sideSpeed = rand() % 50 + 50;
				b.direction = rand() % 2;
				b.releaseTime = rand() % 1000 + 200;

				balloons.push_back(b);
			}
		}

		for (int j = 0; j < balloons.size(); j++) {
			// Fiecare balon incepe sa se miste la un moment de timp aleator.
			if (releaseTimeGlobalBalloons >= balloons[j].releaseTime) {
				balloons[j].released = true;

				// Balonul se poate misca si pe Ox.
				// Daca e pe cale sa iasa din cadru spre dreapta sau se apropie prea mult de acr ii schimb directia.
				if (balloons[j].direction == 0 && balloons[j].balloonX >= 400 && balloons[j].balloonX <= resolution.x - balloonRadiusX) {
					balloons[j].balloonX -= deltaTimeSeconds * balloons[j].sideSpeed;

					if (balloons[j].balloonX <= 400) {
						balloons[j].direction = 1;
						balloons[j].balloonX += deltaTimeSeconds * balloons[j].sideSpeed;
					}
				}
				else if (balloons[j].direction != 0 && balloons[j].balloonX >= 400 && balloons[j].balloonX <= resolution.x - balloonRadiusX) {
					balloons[j].balloonX += deltaTimeSeconds * balloons[j].sideSpeed;

					if (balloons[j].balloonX >= resolution.x - balloonRadiusX) {
						balloons[j].direction = 0;
						balloons[j].balloonX -= deltaTimeSeconds * balloons[j].sideSpeed;
					}
				}

				balloons[j].balloonY += deltaTimeSeconds * balloons[j].upSpeed;
			}

			// Daca balonul a fost lovit atunci acesta o sa scaleze pana la 0 pe o directie serpuitoare aleatoare spre sus. 
			// (ma gandesc ca atunci cand ar pierde aer forta aerului l-ar deplasa in sus)
			if (balloons[j].hit) {
				balloons[j].balloonScale -= deltaTimeSeconds;
				balloons[j].sideSpeed += deltaTimeSeconds * 500;
				balloons[j].upSpeed += deltaTimeSeconds * 1000;
				if (releaseTimeGlobalBalloons % 10 == 0) {
					balloons[j].direction = rand() % 2;
				}
			}

			// Daca balonul a iesit din ecran sau a scalat la 0 e sters.
			if (balloons[j].balloonY > resolution.y + balloonRadiusY + 90 || balloons[j].balloonScale <= 0) {
				balloons.erase(balloons.begin() + j);
			}
			else if (balloons[j].colorRed) {
				// Desenarea baloanelor rosii.
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(balloons[j].balloonX, balloons[j].balloonY);
				if (balloons[j].hit) {
					modelMatrix *= Transform2D::Scale(balloons[j].balloonScale, balloons[j].balloonScale);
				}
				RenderMesh2D(meshes["balloon"], shaders["VertexColor"], modelMatrix);
				RenderMesh2D(meshes["triangle_balloon"], shaders["VertexColor"], modelMatrix);
				RenderMesh2D(meshes["line"], shaders["VertexColor"], modelMatrix);
			}
			else if (!balloons[j].colorRed) {
				// Desenarea baloanelor galbene.
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(balloons[j].balloonX, balloons[j].balloonY);
				if (balloons[j].hit) {
					modelMatrix *= Transform2D::Scale(balloons[j].balloonScale, balloons[j].balloonScale);
				}
				RenderMesh2D(meshes["balloon_yellow"], shaders["VertexColor"], modelMatrix);
				RenderMesh2D(meshes["triangle_balloon_yellow"], shaders["VertexColor"], modelMatrix);
				RenderMesh2D(meshes["line_yellow"], shaders["VertexColor"], modelMatrix);
			}

		}

		// ARC
		// Desenarea arcului.
		// Acesta se roteste dupa pozitia mouse-ului la fiecare moment de timp.
		// Se poate deplasa in sus sau in jos cu butoanele "W" sau "S".
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(bowX, bowY);
		modelMatrix *= Transform2D::Rotate(rotateBow);
		RenderMesh2D(meshes["bow"], shaders["VertexColor"], modelMatrix);

		// Daca sageata se afla la pozitia arcului si se tine apasat pe click stanga,
		// apare un power bar care o sa creasca viteza sagetii.
		// Sageata o sa fie trasa atunci cand nu mai este apasat butonul click stanga.(release button)
		if (powerBarAppearance && !releaseArrow) {
			if (barScaleX < 1) {
				barScaleX += deltaTimeSeconds;
			}

			// Desenarea power barului se face sub arc.
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(bowX, bowY - bowRadius - 10);
			modelMatrix *= Transform2D::Scale(barScaleX, barScaleY);
			RenderMesh2D(meshes["powerBar"], shaders["VertexColor"], modelMatrix);
		}

		// SAGEATA
		modelMatrix = glm::mat3(1);
		if (releaseArrow == true) {
			miliseconds += 1;

			// Bloc care se executa daca sageata nu a fost distrusa de un shuriken.
			if (!brokenArrow) {
				// Pe sageata se aplica forta de gravitatie dupa formulele din fizica de mai jos. (aruncarea pe oblica)
				Vx = cos(initialAngle) * initialSpeed * powerArrow;
				Vy = sin(initialAngle) * initialSpeed * powerArrow;

				arrowXfinal = arrowX + temp * Vx;
				arrowYfinal = arrowY + temp * Vy - ((g * temp * temp) / 2);
				temp += 0.1;

				rotateArrow = atan((Vy - g * temp) / Vx);

				modelMatrix *= Transform2D::Translate(arrowXfinal, arrowYfinal);
				modelMatrix *= Transform2D::Rotate(rotateArrow);

				// Se testeaza coliziunea cu fiecare shuriken la fiecare moment de timp.
				// Daca a fost nimerit un shuriken atunci atat sageata cat si shurikenul
				// sunt distruse si se adauga la scor un punct.
				for (int i = 0; i < shurikens.size(); i++) {
					if (CheckCollisionPointCircle(arrowXfinal + cos(rotateArrow) * (arrowLength / 2 + arrowTipWidth), arrowYfinal + sin(rotateArrow) * (arrowLength / 2 + arrowTipWidth),
						shurikens[i].shurikenX, shurikens[i].shurikenY, shurikenWidth * 1.41f * shurikens[i].shurikenScale) && shurikens[i].released && !shurikens[i].hit && !brokenArrow) {

						brokenArrow = true;
						shurikens[i].hit = true;
						score++;
					}
				}

				// Daca sageata loveste un balon, cel din urma va fi distrus si se vor adauga 10
				// puncte daca balonul e rosu sau se vor lua 5 puncte daca acesta e galben.
				for (int i = 0; i < balloons.size(); i++) {
					if (CheckCollisionPointElipse(arrowXfinal + cos(rotateArrow) * (arrowLength / 2 + arrowTipWidth), arrowYfinal + sin(rotateArrow) * (arrowLength / 2 + arrowTipWidth),
						balloons[i].balloonX, balloons[i].balloonY, balloonRadiusX, balloonRadiusY) && balloons[i].released && !balloons[i].hit && !brokenArrow) {

						if (balloons[i].colorRed) {
							score += 10;
						}
						else if (!balloons[i].colorRed && score - 5 >= 0) {
								score -= 5;
						}

						balloons[i].hit = true;
					}
				}

				// Daca sageata nu a fost distrusa atunci este desenata la pozitia aferenta.
				if (!brokenArrow) {
					RenderMesh2D(meshes["rectangle"], shaders["VertexColor"], modelMatrix);
					RenderMesh2D(meshes["triangle_arrow"], shaders["VertexColor"], modelMatrix);
				}
			}

			// Se spawneaza o sageata dupa 300 de incrementari pentru "miliseconds" dupa ce s-a tras cu o sageata.
			if (miliseconds == 300) {
				releaseArrow = false;
				brokenArrow = false;
				barScaleX = 0;
				miliseconds = 0;
				temp = 0;
			}
		}
		else {
			// Daca nu s-a tras inca cu sageata atunci o sa fie desenata in mijlocul arcului, cu rotatia acestuia.
			modelMatrix *= Transform2D::Translate(bowX, bowY);
			modelMatrix *= Transform2D::Rotate(rotateBow);
			modelMatrix *= Transform2D::Translate(50, 0);
			arrowX = bowX + cos(rotateBow) * arrowLength / 2;
			arrowY = bowY + sin(rotateBow) * arrowLength / 2;
			rotateArrow = rotateBow;
			initialAngle = rotateArrow;

			RenderMesh2D(meshes["rectangle"], shaders["VertexColor"], modelMatrix);
			RenderMesh2D(meshes["triangle_arrow"], shaders["VertexColor"], modelMatrix);
		}
	}
}

void Tema1::FrameEnd()
{
	drawHud();
	//DrawCoordinatSystem();
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	glm::ivec2 resolution = window->GetResolution();

	// Miscarea arcuui in sus si jos se face cu tastele "W" respectiv cu "S".
	if (window->KeyHold(GLFW_KEY_W) && bowY <= resolution.y - 100) {
		bowY += deltaTime * 200;
		rotateBow = atan((resolution.y - mouseYglobal - bowY) / (mouseXglobal - bowX));
	}

	if (window->KeyHold(GLFW_KEY_S) && bowY >= 100) {
		bowY -= deltaTime * 200;
		rotateBow = atan((resolution.y - mouseYglobal - bowY) / (mouseXglobal - bowX));
	}
}

void Tema1::OnKeyPress(int key, int mods)
{
	// add key press event
	// Pentru a pune jocul pe pauza se apasa tasta "P"
	if (key == GLFW_KEY_P) {
		newGamesContinueColor = glm::vec3(0, 0, 0);
		newGamesNewGameColor = glm::vec3(0, 0, 0);
		newGamesExitGameColor = glm::vec3(0.75, 0, 0);
		pauseGameScreen = true;
		gameStart = false;
	}
}

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
	glm::ivec2 resolution = window->GetResolution();
	// In timpul jocului arcul si sageata vor "pointa" spre mouse.
	if (gameStart) {
		mouseXglobal = mouseX;
		mouseYglobal = mouseY;

		// Coordonatele mouse-ului au originea in stanga sus a ecranului.
		rotateBow = atan((resolution.y - mouseY - bowY) / (mouseX - bowX));
	}
	// Daca se da hover peste scris atunci se schimba culoarea pentru a sti ce buton o sa fie apasat. 
	// Ecran new game.
	else if (newGameScreen) {
		if (mouseX < resolution.x / 2 + textWidthNewGame / 2 && mouseX > resolution.x / 2 - textWidthNewGame / 2
			&& mouseY > resolution.y / 2 - 100 && mouseY < resolution.y / 2 - 100 + textHightNewGame) {

			newGamesNewGameColor = glm::vec3(0.75, 0.75, 0.75);
		}
		else if (mouseX < resolution.x / 2 + textWidthExitGame / 2 && mouseX > resolution.x / 2 - textWidthExitGame / 2
			&& mouseY > resolution.y / 2 - 50 && mouseY < resolution.y / 2 - 50 + textHightExitGame) {

			newGamesExitGameColor = glm::vec3(0.5, 0, 0);
		}
		else {
			newGamesNewGameColor = glm::vec3(0, 0, 0);
			newGamesExitGameColor = glm::vec3(0.75, 0, 0);
		}
	}
	// Ecran game over.
	else if (gameOverScreen && youDiedColor >= 0.75) {
		if (mouseX < resolution.x / 2 + textWidthNewGame / 2 && mouseX > resolution.x / 2 - textWidthNewGame / 2
			&& mouseY > resolution.y / 2 - 100 && mouseY < resolution.y / 2 - 100 + textHightNewGame) {

			exitGamesNewGameColor = glm::vec3(0.5, 0, 0);
		}
		else if (mouseX < resolution.x / 2 + textWidthExitGame / 2 && mouseX > resolution.x / 2 - textWidthExitGame / 2
			&& mouseY > resolution.y / 2 - 50 && mouseY < resolution.y / 2 - 50 + textHightExitGame) {

			exitGamesExitGameColor = glm::vec3(0.5, 0, 0);
		}
		else {
			exitGamesNewGameColor = glm::vec3(0.75, 0, 0);
			exitGamesExitGameColor = glm::vec3(0.75, 0, 0);
		}
	}
	// Ecran pause game.
	else if (pauseGameScreen) {
		if (mouseX < resolution.x / 2 + textWidthNewGame / 2 && mouseX > resolution.x / 2 - textWidthNewGame / 2
			&& mouseY > resolution.y / 2 - 150 && mouseY < resolution.y / 2 - 150 + textHightNewGame) {

			newGamesContinueColor = glm::vec3(0.75, 0.75, 0.75);
		}
		else if (mouseX < resolution.x / 2 + textWidthNewGame / 2 && mouseX > resolution.x / 2 - textWidthNewGame / 2
			&& mouseY > resolution.y / 2 - 100 && mouseY < resolution.y / 2 - 100 + textHightNewGame) {

			newGamesNewGameColor = glm::vec3(0.75, 0.75, 0.75);
		}
		else if (mouseX < resolution.x / 2 + textWidthExitGame / 2 && mouseX > resolution.x / 2 - textWidthExitGame / 2
			&& mouseY > resolution.y / 2 - 50 && mouseY < resolution.y / 2 - 50 + textHightExitGame) {

			newGamesExitGameColor = glm::vec3(0.5, 0, 0);
		}
		else {
			newGamesContinueColor = glm::vec3(0, 0, 0);
			newGamesNewGameColor = glm::vec3(0, 0, 0);
			newGamesExitGameColor = glm::vec3(0.75, 0, 0);
		}
	}
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	glm::ivec2 resolution = window->GetResolution();
	// Ecran new game.
	if (newGameScreen) {
		// Daca se da click pe new game atunci se reseteaza variabilele si vectorii.
		if (mouseX < resolution.x / 2 + textWidthNewGame / 2 && mouseX > resolution.x / 2 - textWidthNewGame / 2
			&& mouseY > resolution.y / 2 - 100 && mouseY < resolution.y / 2 - 100 + textHightNewGame) {
			stageCount = 1;
			waveCount = 0;
			shurikenScaleAdd = 0;
			balloonUpSpeedAdd = 0;
			shurikenXSpeedAdd = 0;
			shurikenVectorMaxSize = 5;

			mouseClickNewGame = true;
			newGameScreen = false;
			gameStart = true;
		}
		// Daca se da click pe exit game se iese din joc.
		else if (mouseX < resolution.x / 2 + textWidthExitGame / 2 && mouseX > resolution.x / 2 - textWidthExitGame / 2
			&& mouseY > resolution.y / 2 - 50 && mouseY < resolution.y / 2 - 50 + textHightExitGame) {

			exit(-1);
		}
	}
	// Ecran game over.
	else if (gameOverScreen && youDiedColor >= 0.75) {
		// Daca se da click pe new game atunci se reseteaza variabilele si vectorii.
		if (mouseX < resolution.x / 2 + textWidthNewGame / 2 && mouseX > resolution.x / 2 - textWidthNewGame / 2
			&& mouseY > resolution.y / 2 - 100 && mouseY < resolution.y / 2 - 100 + textHightNewGame) {

			stageCount = 1;
			waveCount = 0;
			shurikenScaleAdd = 0;
			balloonUpSpeedAdd = 0;
			shurikenXSpeedAdd = 0;
			shurikenVectorMaxSize = 5;
			youDiedColor = 0;

			mouseClickNewGame = true;
			gameOverScreen = false;
			gameStart = true;
			lifeCount = 3;
			score = 0;
			balloons.clear();
			shurikens.clear();
		}
		// Daca se da click pe exit game se iese din joc.
		else if (mouseX < resolution.x / 2 + textWidthExitGame / 2 && mouseX > resolution.x / 2 - textWidthExitGame / 2
			&& mouseY > resolution.y / 2 - 50 && mouseY < resolution.y / 2 - 50 + textHightExitGame) {

			exit(-1);
		}
	}
	// Ecran pause game.
	else if (pauseGameScreen) {
		// Daca se doreste continuarea jocului atunci se schimba ecranul si se continua jocul de unde a ramas.
		if (mouseX < resolution.x / 2 + textWidthNewGame / 2 && mouseX > resolution.x / 2 - textWidthNewGame / 2
			&& mouseY > resolution.y / 2 - 150 && mouseY < resolution.y / 2 - 150 + textHightNewGame) {

			pauseGameScreen = false;
			mouseClickNewGame = true;
			gameStart = true;
		}
		// Daca se da click pe new game atunci se reseteaza variabilele si vectorii.
		else if (mouseX < resolution.x / 2 + textWidthNewGame / 2 && mouseX > resolution.x / 2 - textWidthNewGame / 2
			&& mouseY > resolution.y / 2 - 100 && mouseY < resolution.y / 2 - 100 + textHightNewGame) {

			stageCount = 1;
			waveCount = 0;
			shurikenScaleAdd = 0;
			balloonUpSpeedAdd = 0;
			shurikenXSpeedAdd = 0;
			shurikenVectorMaxSize = 5;

			pauseGameScreen = false;
			mouseClickNewGame = true;
			gameStart = true;
			lifeCount = 3;
			score = 0;
			balloons.clear();
			shurikens.clear();
		}
		// Daca se da click pe exit game se iese din joc.
		else if (mouseX < resolution.x / 2 + textWidthExitGame / 2 && mouseX > resolution.x / 2 - textWidthExitGame / 2
			&& mouseY > resolution.y / 2 - 50 && mouseY < resolution.y / 2 - 50 + textHightExitGame) {

			exit(-1);
		}
	}
	else if (gameStart) {
		powerBarAppearance = true;
	}
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
	// Cand se incepe un joc nou click facut pe butonul anterior(New Game), nu o sa fie luat in considerare
	// Prin urmare nu o sa fie lansata o sageata.
	// Daca suntem deja in timpul jocului atunci se retine puterea care o sa fie adaugata la sageata, si se lanseaza.
	// De asemenea nu mai apare power barul cand nu mai este apasat pe click stanga.
	if (gameStart && !mouseClickNewGame) {
			releaseArrow = true;
			powerBarAppearance = false;
			powerArrow = barScaleX + 1;
	}
	else if (gameStart && mouseClickNewGame) {
		mouseClickNewGame = false;
	}
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
	// Cand se mareste rezolutia se schimba si cat arata camera.
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	// Se da spatiul in care o safie randat textul.
	Text = new TextRenderer(resolution.x, resolution.y);
	Text->Load("Source/TextRenderer/Fonts/Arial.ttf", 18);

	// Shurikenurile se vor deplasa mereu la marginea ecranului
	for (int i = 0; i < shurikens.size(); i++) {
		if (releaseTimeGlobalShurikens < shurikens[i].releaseTime) {
			shurikens[i].shurikenX = resolution.x + shurikenWidth * 1.41f + 5;
			shurikens[i].shurikenY = rand() % (resolution.y - 200) + 100;
		}
	}

	// Baloanele care nu au se misca inca se reaseaza pe toata rezolutia ecranului.
	for (int i = 0; i < balloons.size(); i++) {
		if (releaseTimeGlobalBalloons < balloons[i].releaseTime) {
			balloons[i].balloonX = rand() % (resolution.x - 402) + 401;
		}
	}
}
