#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <thread>

float yCamPos = 20.0f;
GLfloat lightAmbient[] = { 0.5,0.5,0.5,1 };
GLfloat lightDiffuse[] = { 1,1,1,1 };
GLfloat lightPosition[] = { 0,3,0 };
GLuint texture[2];
GLuint cube;

float cR = 0; float cG = 0; float cB = 0; //centre line color
float eR = 0; float eG = 0; float eB = 0; //edge line color
float bR = 34; float bG = 139; float bB = 34; //default block color

bool gameOver = false;
bool gameStart = false;
int historyScore = 0;

enum Direction {
	D_LEFT, D_RIGHT, D_UP, D_DOWN
};

class Fruit {
public:
	Fruit(int pX, int pZ) {
		x = pX;
		z = pZ;
		angle = 0.0f;
	};
	void draw() {
		angle += 2.0f;
		glPushMatrix();
		glTranslatef(-x, 0, z);
		glRotatef(angle, 0, 1, 0);
		glColor3f(1.0, 0.0, 0.0);
		glutSolidCube(0.6);
		glColor3f(0.0, 0.0, 0.0);
		glutWireCube(0.62);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		float gC = 0.5;
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
	};
	float getX() { return x; };
	float getZ() { return z; };
	void setX(float pX) { x = pX; };
	void setZ(float pZ) { z = pZ; };
	float getAngle() { return angle; };
private:
	float x, z, angle;
};

Fruit* fruit = new Fruit(1, 1);

class Block { //Blocks that make up the snake object
public:
	Block(float pX, float pY, float pZ, float pSize = 1.0f) {
		x = pX;
		y = pY;
		z = pZ;
		size = pSize;
		r = bR; g = bG; b = bB; //rgb colors are set to their default
	};
	void setX(float pX) { x = pX; };
	void setY(float pY) { y = pY; };
	void setZ(float pZ) { z = pZ; };
	float getX() { return x; };
	float getY() { return y; };
	float getZ() { return z; };
	void setColor(float pR, float pG, float pB) { r = pR;g = pG;b = pB; };
	void draw() {
		glPushMatrix();
		glTranslatef(-x, y, z);
		if (r > 1 || g > 1 || b > 1) {
			glColor3ub(r, g, b);
		}
		else {
			glColor3f(r, g, b);
		}
		glCallList(cube);
		glPopMatrix();
	};

private:
	float x, y, z, size, r, g, b;
};

class Snake { //snake movement and collision
public:
	std::vector<Block*> blocks; //Snake is an array of blocks

	Snake(float startX, float startZ, int blockCount) {
		for (float i = 0; i < blockCount; i++) {
			Block* temp = new Block(startX + i, 0, startZ);
			if (i == 0)
				temp->setColor(50, 205, 50);
			blocks.push_back(temp);
		}
		isUpdated = false;
		currentDirection = D_LEFT;
		score = 0;
	};
	void draw() { //loops through block array and calls draw functions

		char scr[60];
		char scre[60];
		char achieve[60] = "";
		char scoremessage[60];
		char historymessage[60];
		strcpy_s(scr, "S C O R E : ");
		sprintf_s(scre, "%d", score);
		sprintf_s(scoremessage, "%d", historyScore);
		strcat_s(scr, scre);
		printTxt(1.0f, 9.0f, scr);

		if (score > 4 && score < 6)
		{
			strcpy_s(achieve, "You snake have reached: 5!");
			printTxt(5.0f, 9.0f, achieve);
		}

		else if (score > 9 && score < 11)
		{
			strcpy_s(achieve, "Amazing!! Your score is 10!");
			printTxt(5.0f, 9.0f, achieve);
		}

		else if (score > 14 && score < 16)
		{
			strcpy_s(achieve, "Briliant. You have just reached to 15!!");
			printTxt(5.0f, 9.0f, achieve);
		}

		else if (score > 19 && score < 21)
		{
			strcpy_s(achieve, "GodHand. You just pass the score of 20!");
			printTxt(5.0f, 9.0f, achieve);
		}

		else if (score > 25)
		{
			strcpy_s(achieve, "This is amazing!. Your current score is: ");
			sprintf_s(scre, "%d!!!", score);
			strcat_s(achieve, scre);
			printTxt(5.0f, 9.0f, achieve);
		}

		if (gameOver == true) {
			strcpy_s(scr, "G A M E   O V E R   P R E S S   F1   T O   R E S T A R T");
			printTxt(3.0f, 9.0f, scr);
			strcpy_s(historymessage, "Your score was: ");
			strcat_s(historymessage, scoremessage);
			printTxt(1.0f, 8.5f, historymessage);
			removeBlock();
		}

		if (gameStart == false) {
			strcpy_s(scr, "P R E S S   F2   T O   S T A R T");
			printTxt(4.0f, 9.0f, scr);
		}

		for (int i = 0; i <= blocks.size() - 1; i++) {
			Block* temp = blocks.at(i);
			temp->draw();
		}
	};

	void *font = GLUT_BITMAP_HELVETICA_18;

	void printTxt(float x, float y, char *String) {
		char *c;

		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);

		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 10, 0, 10);

		glColor3f(9.0f, 0.7f, 0.4f);
		glRasterPos2f(x, y);
		glDisable(GL_LIGHTING);

		for (c = String; *c != '\0'; c++)
			glutBitmapCharacter(font, *c);

		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

	}

	void move(Direction direction) {
		Block* snakeHead = blocks.at(0);
		if (direction == D_LEFT && currentDirection != D_RIGHT) {
			pushSnake();
			snakeHead->setX(snakeHead->getX() - 1.0f);
			currentDirection = direction;
			isUpdated = true;
		}
		else if (direction == D_RIGHT && currentDirection != D_LEFT) {
			pushSnake();
			snakeHead->setX(snakeHead->getX() + 1.0f);
			currentDirection = direction;
			isUpdated = true;
		}
		else if (direction == D_UP && currentDirection != D_DOWN) {
			pushSnake();
			snakeHead->setZ(snakeHead->getZ() + 1.0f);
			currentDirection = direction;
			isUpdated = true;
		}
		else if (direction == D_DOWN && currentDirection != D_UP) {
			pushSnake();
			snakeHead->setZ(snakeHead->getZ() - 1.0f);
			currentDirection = direction;
			isUpdated = true;
		}
	};

	void pushSnake() { //advances all other blocks
		for (int i = blocks.size() - 1; i >= 1; i--) {
			Block* temp = blocks.at(i);
			Block* prevBlock = blocks.at(i - 1);
			temp->setX(prevBlock->getX());
			temp->setZ(prevBlock->getZ());
		}
	};

	void update() { //called at every frame
		if (gameOver == false && gameStart == true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (!isUpdated)
				move(currentDirection);
			isUpdated = false;
			collisionDetection();
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			score = 0;
		}
	};

	void collisionDetection() { //checks if snake head hit fruit or it's own body
		float x = blocks.at(0)->getX();
		float z = blocks.at(0)->getZ();
		if (fruit->getX() == x && fruit->getZ() == z) { //check for fruit collision
			score++; //increment score
			historyScore++;
			printf("Score = %i\n", score);
			bool repeat = false;
			do { //makes sure that fruit doesn't spawn underneath body
				repeat = false;
				fruit->setX(rand() % 11 - 5);
				fruit->setZ(rand() % 11 - 5);
				for (int i = 0; i <= blocks.size() - 1;i++) {
					x = blocks.at(i)->getX();
					z = blocks.at(i)->getZ();
					if (fruit->getX() == x && fruit->getZ() == z)
						repeat = true;
				}
			} while (repeat);
			addBlock();
		}
		for (int i = 1; i <= blocks.size() - 1; i++) { //check for collision with own body
			if (blocks.at(i)->getX() == blocks.at(0)->getX() && blocks.at(i)->getZ() == blocks.at(0)->getZ()) {
				printf("collision\n");
				gameOver = true;
			}
		}


		for (int i = -5; i <= 5; i++) {
			if (i == blocks.at(0)->getX() && 6 == blocks.at(0)->getZ()) {
				printf("collision\n");
				printf("%d\n", blocks.size());
				gameOver = true;
			}

			if (i == blocks.at(0)->getX() && -6 == blocks.at(0)->getZ()) {
				printf("collision\n");
				gameOver = true;
			}

			if (6 == blocks.at(0)->getX() && i == blocks.at(0)->getZ()) {
				printf("collision\n");
				gameOver = true;
			}

			if (-6 == blocks.at(0)->getX() && i == blocks.at(0)->getZ()) {
				printf("collision\n");
				gameOver = true;
			}
		}

	};

	void addBlock() { //adds a new block to the snake
		Block* newBlock = new Block(-100, 0, -100);
		blocks.push_back(newBlock);
	};

	void resetSnakePosition() { //reset the snakes head position
		blocks.at(0)->setX(0);
		blocks.at(0)->setZ(0);
	}

	void removeBlock() { //removes blocks from the snake
		if (blocks.size() >= 2) {
			for (int i = 1; i < blocks.size() - 2; i++) {
				blocks.pop_back();
			}
		}
	};

	Direction getDirection() { return currentDirection; };

private:
	Direction currentDirection;
	bool isUpdated;
	int score;
};

Snake* snake = new Snake(0, 0, 3);

void buildDLs() { //Build display lists
	cube = glGenLists(1);
	glNewList(cube, GL_COMPILE);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(0.5, 0.5, 0.5); //back face
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glNormal3f(0, 0, -1);
	glVertex3f(0.5, 0.5, -0.5); //front face
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glNormal3f(0, -1, 0); //bottom face
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glNormal3f(0, 1, 0); //top face
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glNormal3f(-1, 0, 0); //right face
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glNormal3f(1, 0, 0); //left face
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glEnd();
	glEndList();
}

void drawGrid(void) { //values for grid
	glColor3ub(222, 184, 135);
	glLineWidth(1);
	glBegin(GL_LINES);
	for (float i = -6; i <= 5; i++) {
		glVertex3f(-5 - 0.5, 0 - 0.5f, i + 0.5f);
		glVertex3f(6 - 0.5, 0 - 0.5f, i + 0.5f);
	}
	for (float i = -5; i <= 6; i++) {
		glVertex3f(i - 0.5, -0.5f, -6 - 0.5f);
		glVertex3f(i - 0.5, -0.5f, 6 - 0.5f);
	}
	glEnd();
	glLineWidth(1);
	glColor3ub(245, 222, 179);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-5 - 0.5, -0.51, -6.5);
	glVertex3f(-5 - 0.5, -0.51, 6.5);
	glVertex3f(6 - 0.5, -0.51, 6.5);
	glVertex3f(6 - 0.5, -0.51, -6.5);
	glEnd();

	glColor3ub(160, 82, 45);
	for (int i = -5; i <= 5; i++) {
		glPushMatrix();
		glTranslatef(-6, 0, i);
		glCallList(cube);

		glColor3f(cR, cG, cB);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f(0, 0.51, -0.5);
		glVertex3f(0, 0.51, 0.5);
		glEnd();
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f(0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(-0.5, 0.51, 0.5);
		glEnd();
		glColor3ub(160, 82, 45);


		glPopMatrix();
		glPushMatrix();
		glTranslatef(6, 0, i);
		glCallList(cube);

		glColor3f(cR, cG, cB);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f(0, 0.51, -0.5);
		glVertex3f(0, 0.51, 0.5);
		glEnd();
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f(0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(-0.5, 0.51, 0.5);
		glEnd();
		glColor3ub(160, 82, 45);

		glPopMatrix();
		glPushMatrix();
		glTranslatef(i, 0, -6);
		glCallList(cube);

		glColor3f(cR, cG, cB);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f(-0.5, 0.51, 0);
		glVertex3f(0.5, 0.51, 0);
		glEnd();
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f(-0.5, 0.51, 0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, -0.5);
		glEnd();
		glColor3ub(160, 82, 45);

		glPopMatrix();
		glPushMatrix();
		glTranslatef(i, 0, 6);
		glCallList(cube);

		glColor3f(cR, cG, cB);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f(-0.5, 0.51, 0);
		glVertex3f(0.5, 0.51, 0);
		glEnd();
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f(-0.5, 0.51, 0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, -0.5);
		glEnd();
		glColor3ub(160, 82, 45);

		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(6, 0, 6); // top left
	glCallList(cube);
	glColor3f(cR, cG, cB);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f(-0.5, 0.51, 0);
	glVertex3f(0, 0.51, 0);
	glVertex3f(0, 0.51, 0);
	glVertex3f(0, 0.51, -0.5);
	glEnd();
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f(-0.5, 0.51, 0.5);
	glVertex3f(0.5, 0.51, 0.5);
	glVertex3f(0.5, 0.51, 0.5);
	glVertex3f(0.5, 0.51, -0.5);
	glVertex3f(-0.51, 0.51, -0.51);
	glVertex3f(-0.51, -0.51, -0.51);
	glEnd();
	glColor3ub(160, 82, 45);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-6, 0, 6); // top right
	glCallList(cube);
	glColor3f(cR, cG, cB);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f(0.5, 0.51, 0);
	glVertex3f(0, 0.51, 0);
	glVertex3f(0, 0.51, 0);
	glVertex3f(0, 0.51, -0.5);
	glEnd();
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f(-0.5, 0.51, 0.5);
	glVertex3f(0.5, 0.51, 0.5);
	glVertex3f(-0.5, 0.51, 0.5);
	glVertex3f(-0.5, 0.51, -0.5);
	glVertex3f(0.51, 0.51, -0.51);
	glVertex3f(0.51, -0.51, -0.51);
	glEnd();
	glColor3ub(160, 82, 45);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-6, 0, -6); // bottom right
	glCallList(cube);
	glColor3f(cR, cG, cB);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f(0.5, 0.51, 0);
	glVertex3f(0, 0.51, 0);
	glVertex3f(0, 0.51, 0);
	glVertex3f(0, 0.51, 0.5);
	glEnd();
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f(-0.5, 0.51, -0.5);
	glVertex3f(0.5, 0.51, -0.5);
	glVertex3f(-0.5, 0.51, 0.5);
	glVertex3f(-0.5, 0.51, -0.5);
	glVertex3f(0.51, 0.51, 0.51);
	glVertex3f(0.51, -0.51, 0.51);
	glEnd();
	glColor3ub(160, 82, 45);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(6, 0, -6); // bottom left
	glCallList(cube);
	glColor3f(cR, cG, cB);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f(-0.5, 0.51, 0);
	glVertex3f(0, 0.51, 0);
	glVertex3f(0, 0.51, 0);
	glVertex3f(0, 0.51, 0.5);
	glEnd();
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f(-0.5, 0.51, -0.5);
	glVertex3f(0.5, 0.51, -0.5);
	glVertex3f(0.5, 0.51, 0.5);
	glVertex3f(0.5, 0.51, -0.5);
	glVertex3f(-0.51, 0.51, 0.51);
	glVertex3f(-0.51, -0.51, 0.51);
	glEnd();
	glColor3ub(160, 82, 45);
	glPopMatrix();
}

void drawSnakeLines() { //hardcoded values for snake lines
	Block* block = snake->blocks.at(0);
	Block* blockAfter = NULL;
	Block* blockBefore = NULL;
	glColor3f(cR, cG, cB);
	float c_lw = 1; //centre line width
	float e_lw = 1; //edge line width
	glLineWidth(1);
	if (snake->getDirection() == D_UP || snake->getDirection() == D_DOWN) {
		glPushMatrix();
		glTranslatef(-block->getX(), block->getY(), block->getZ());
		glBegin(GL_LINES);
		glVertex3f(0, 0.51, -0.5);
		glVertex3f(0, 0.51, 0.5);
		glColor3f(eR, eG, eB);
		glVertex3f(0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(-0.5, 0.51, 0.5);
		if (snake->getDirection() == D_UP) {
			glVertex3f(0.5, 0.51, 0.5);
			glVertex3f(-0.5, 0.51, 0.5);
		}
		else if (snake->getDirection() == D_DOWN) {
			glVertex3f(0.5, 0.51, -0.5);
			glVertex3f(-0.5, 0.51, -0.5);
		}
		glEnd();
		glPopMatrix();
	}
	else {
		glPushMatrix();
		glTranslatef(-block->getX(), block->getY(), block->getZ());
		glBegin(GL_LINES);
		glVertex3f(-0.5, 0.51, 0);
		glVertex3f(0.5, 0.51, 0);
		glColor3f(eR, eG, eB);
		glVertex3f(-0.5, 0.51, 0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, -0.5);
		if (snake->getDirection() == D_LEFT) {
			glVertex3f(0.5, 0.51, -0.5);
			glVertex3f(0.5, 0.51, 0.5);
		}
		else if (snake->getDirection() == D_RIGHT) {
			glVertex3f(-0.5, 0.51, -0.5);
			glVertex3f(-0.5, 0.51, 0.5);
		}
		glEnd();
		glPopMatrix();
	}
	for (int i = 1; i <= snake->blocks.size() - 2; i++) {
		block = snake->blocks.at(i);
		blockAfter = snake->blocks.at(i + 1);
		blockBefore = snake->blocks.at(i - 1);
		float x = block->getX();
		float z = block->getZ();
		float aX = blockAfter->getX();
		float aZ = blockAfter->getZ();
		float bX = blockBefore->getX();
		float bZ = blockBefore->getZ();
		glPushMatrix();
		glTranslatef(-x, block->getY(), z);
		if (z != bZ && z != aZ) { //up or down
			glLineWidth(c_lw);
			glBegin(GL_LINES);
			glColor3f(cR, cG, cB);
			glVertex3f(0, 0.51, -0.5); //centre line
			glVertex3f(0, 0.51, 0.5);
			glEnd();
			glLineWidth(e_lw);
			glBegin(GL_LINES);
			glColor3f(eR, eG, eB);
			glVertex3f(-0.5, 0.51, -0.5); //edges below
			glVertex3f(-0.5, 0.51, 0.5);
			glVertex3f(0.5, 0.51, -0.5);
			glVertex3f(0.5, 0.51, 0.5);
			glEnd();
			glPopMatrix();
		}
		else if (x != bX && x != aX) { //left or right
			glLineWidth(c_lw);
			glBegin(GL_LINES);
			glColor3f(cR, cG, cB);
			glVertex3f(-0.5, 0.51, 0); //centre line
			glVertex3f(0.5, 0.51, 0);
			glEnd();
			glLineWidth(e_lw);
			glBegin(GL_LINES);
			glColor3f(eR, eG, eB);
			glVertex3f(-0.5, 0.51, -0.5); //edges below
			glVertex3f(0.5, 0.51, -0.5);
			glVertex3f(-0.5, 0.51, 0.5);
			glVertex3f(0.5, 0.51, 0.5);
			glEnd();
		}
		else if (x > bX && z < aZ) {  //b_right && a_up
			glLineWidth(c_lw);
			glBegin(GL_LINES);
			glColor3f(cR, cG, cB);
			glVertex3f(0, 0.51, 0);
			glVertex3f(0.5, 0.51, 0);
			glVertex3f(0, 0.51, 0.5);
			glVertex3f(0, 0.51, 0);
			glEnd();
			glLineWidth(e_lw);
			glBegin(GL_LINES);
			glColor3f(eR, eG, eB);
			glVertex3f(-0.5, 0.51, -0.5);
			glVertex3f(0.5, 0.51, -0.5);
			glVertex3f(-0.5, 0.51, 0.5);
			glVertex3f(-0.5, 0.51, -0.5);
			glEnd();
		}
		else if (x < bX && z < aZ) { //b_left && a_up
			glLineWidth(c_lw);
			glBegin(GL_LINES);
			glColor3f(cR, cG, cB);
			glVertex3f(0, 0.51, 0);
			glVertex3f(-0.5, 0.51, 0);
			glVertex3f(0, 0.51, 0.5);
			glVertex3f(0, 0.51, 0);
			glEnd();
			glLineWidth(e_lw);
			glBegin(GL_LINES);
			glColor3f(eR, eG, eB);
			glVertex3f(0.5, 0.51, -0.5);
			glVertex3f(-0.5, 0.51, -0.5);
			glVertex3f(0.5, 0.51, 0.5);
			glVertex3f(0.5, 0.51, -0.5);
			glEnd();
		}
		else if (x > bX && z > aZ) { //b_right && a_down
			glLineWidth(c_lw);
			glBegin(GL_LINES);
			glColor3f(cR, cG, cB);
			glVertex3f(0, 0.51, 0);
			glVertex3f(0.5, 0.51, 0);
			glVertex3f(0, 0.51, -0.5);
			glVertex3f(0, 0.51, 0);
			glEnd();
			glLineWidth(e_lw);
			glBegin(GL_LINES);
			glColor3f(eR, eG, eB);
			glVertex3f(-0.5, 0.51, 0.5);
			glVertex3f(0.5, 0.51, 0.5);
			glVertex3f(-0.5, 0.51, -0.5);
			glVertex3f(-0.5, 0.51, 0.5);
			glEnd();
		}
		else if (x < bX && z > aZ) { //b_left && a_down
			glLineWidth(c_lw);
			glBegin(GL_LINES);
			glColor3f(cR, cG, cB);
			glVertex3f(0, 0.51, 0);
			glVertex3f(-0.5, 0.51, 0);
			glVertex3f(0, 0.51, -0.5);
			glVertex3f(0, 0.51, 0);
			glEnd();
			glLineWidth(e_lw);
			glBegin(GL_LINES);
			glColor3f(eR, eG, eB);
			glVertex3f(0.5, 0.51, 0.5);
			glVertex3f(-0.5, 0.51, 0.5);
			glVertex3f(0.5, 0.51, -0.5);
			glVertex3f(0.5, 0.51, 0.5);
			glEnd();
		}
		else if (x < aX && z < bZ) {  //a_right && b_up
			glLineWidth(c_lw);
			glBegin(GL_LINES);
			glColor3f(cR, cG, cB);
			glVertex3f(0, 0.51, 0);
			glVertex3f(-0.5, 0.51, 0);
			glVertex3f(0, 0.51, 0.5);
			glVertex3f(0, 0.51, 0);
			glEnd();
			glLineWidth(e_lw);
			glBegin(GL_LINES);
			glColor3f(eR, eG, eB);
			glVertex3f(0.5, 0.51, -0.5);
			glVertex3f(-0.5, 0.51, -0.5);
			glVertex3f(0.5, 0.51, 0.5);
			glVertex3f(0.5, 0.51, -0.5);
			glEnd();
		}
		else if (x > aX && z < bZ) { //a_left && b_up
			glLineWidth(c_lw);
			glBegin(GL_LINES);
			glColor3f(cR, cG, cB);
			glVertex3f(0, 0.51, 0);
			glVertex3f(0.5, 0.51, 0);
			glVertex3f(0, 0.51, 0.5);
			glVertex3f(0, 0.51, 0);
			glEnd();
			glLineWidth(e_lw);
			glBegin(GL_LINES);
			glColor3f(eR, eG, eB);
			glVertex3f(-0.5, 0.51, -0.5);
			glVertex3f(0.5, 0.51, -0.5);
			glVertex3f(-0.5, 0.51, 0.5);
			glVertex3f(-0.5, 0.51, -0.5);
			glEnd();
		}
		else if (x < aX && z > bZ) { //a_right && b_down
			glLineWidth(c_lw);
			glBegin(GL_LINES);
			glColor3f(cR, cG, cB);
			glVertex3f(0, 0.51, 0);
			glVertex3f(-0.5, 0.51, 0);
			glVertex3f(0, 0.51, -0.5);
			glVertex3f(0, 0.51, 0);
			glEnd();
			glLineWidth(e_lw);
			glBegin(GL_LINES);
			glColor3f(eR, eG, eB);
			glVertex3f(0.5, 0.51, 0.5);
			glVertex3f(-0.5, 0.51, 0.5);
			glVertex3f(0.5, 0.51, -0.5);
			glVertex3f(0.5, 0.51, 0.5);
			glEnd();
		}
		else if (x > aX && z > bZ) { //a_left && b_down
			glLineWidth(c_lw);
			glBegin(GL_LINES);
			glColor3f(cR, cG, cB);
			glVertex3f(0, 0.51, 0);
			glVertex3f(0.5, 0.51, 0);
			glVertex3f(0, 0.51, -0.5);
			glVertex3f(0, 0.51, 0);
			glEnd();
			glLineWidth(e_lw);
			glBegin(GL_LINES);
			glColor3f(eR, eG, eB);
			glVertex3f(-0.5, 0.51, 0.5);
			glVertex3f(0.5, 0.51, 0.5);
			glVertex3f(-0.5, 0.51, -0.5);
			glVertex3f(-0.5, 0.51, 0.5);
			glEnd();
		}
		glPopMatrix();
	}
	block = snake->blocks.at(snake->blocks.size() - 1); //last block
	blockBefore = snake->blocks.at(snake->blocks.size() - 2);
	glPushMatrix();
	glTranslatef(-block->getX(), block->getY(), block->getZ());
	if (block->getX() > blockBefore->getX()) { //left
		glLineWidth(c_lw);
		glBegin(GL_LINES);
		glColor3f(cR, cG, cB);
		glVertex3f(0, 0.51, 0);
		glVertex3f(0.5, 0.51, 0);
		glEnd();
		glLineWidth(e_lw);
		glBegin(GL_LINES);
		glColor3f(eR, eG, eB);
		glVertex3f(-0.5, 0.51, 0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, -0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(-0.5, 0.51, 0.5);
		glEnd();
	}
	else if (block->getX() < blockBefore->getX()) { //right
		glLineWidth(c_lw);
		glBegin(GL_LINES);
		glColor3f(cR, cG, cB);
		glVertex3f(-0.5, 0.51, 0);
		glVertex3f(0, 0.51, 0);
		glEnd();
		glLineWidth(e_lw);
		glBegin(GL_LINES);
		glColor3f(eR, eG, eB);
		glVertex3f(-0.5, 0.51, 0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glEnd();
	}
	else if (block->getZ() < blockBefore->getZ()) { //up
		glLineWidth(c_lw);
		glBegin(GL_LINES);
		glColor3f(cR, cG, cB);
		glVertex3f(0, 0.51, 0);
		glVertex3f(0, 0.51, 0.5);
		glEnd();
		glLineWidth(e_lw);
		glBegin(GL_LINES);
		glColor3f(eR, eG, eB);
		glVertex3f(0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(-0.5, 0.51, 0.5);
		glVertex3f(0.5, 0.51, -0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glEnd();
	}
	else if (block->getZ() > blockBefore->getZ()) { //down
		glLineWidth(c_lw);
		glBegin(GL_LINES);
		glColor3f(cR, cG, cB);
		glVertex3f(0, 0.51, -0.5);
		glVertex3f(0, 0.51, 0);
		glEnd();
		glLineWidth(e_lw);
		glBegin(GL_LINES);
		glColor3f(eR, eG, eB);
		glVertex3f(0.5, 0.51, -0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, -0.5);
		glVertex3f(-0.5, 0.51, 0.5);
		glVertex3f(0.5, 0.51, 0.5);
		glVertex3f(-0.5, 0.51, 0.5);
		glEnd();
	}
	glPopMatrix();
	glLineWidth(1);
}

void draw(void) {

	snake->update(); //update snake position
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(
		-5, yCamPos, -12, //eye
		0, 0, 0,  //centre
		0, 1, 0   //up
	);
	drawGrid();
	snake->draw();
	drawSnakeLines();
	fruit->draw();
	glutSwapBuffers();
}

void resize(int w, int h) { //resize of window
	if (h == 0)
		h = 1;
	float ratio = w * 1.0f / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(50, ratio, 1, 200);
	glMatrixMode(GL_MODELVIEW);
}

void keyEvents(unsigned char key, int x, int y) { //keys are pressed
	switch (key) {
	case 27: //escape key
		exit(0);
		break;
	}
}

void specialKeys(int key, int x, int y) { //keys are pressed
	switch (key) {
	case GLUT_KEY_F1:
		if (gameOver == true) {
			gameOver = false;
			historyScore = 0;
			snake->resetSnakePosition();
		}
		break;
	case GLUT_KEY_F2:
		if (gameStart == false) {
			gameStart = true;
		}
		break;
	case GLUT_KEY_LEFT:
		if (gameStart == true) {
			if (gameOver == false) {
				snake->move(D_LEFT);
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		if (gameStart == true) {
			if (gameOver == false) {
				snake->move(D_RIGHT);
			}
		}
		break;
	case GLUT_KEY_UP:
		if (gameStart == true) {
			if (gameOver == false) {
				snake->move(D_UP);
			}
		}
		break;
	case GLUT_KEY_DOWN:
		if (gameStart == true) {
			if (gameOver == false) {
				snake->move(D_DOWN);
			}
		}
		break;
	}
}

void loadGLTexture(std::string filePath, unsigned int textureNum) { //loads textures
	glBindTexture(GL_TEXTURE_2D, texture[textureNum]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void initGL(void) {
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
}

void mouseEvents(int button, int dir, int x, int y) { //mouse events
	switch (button) {
	case 3: //scroll up and zoom in
		printf("zoom in\n");
		yCamPos -= 0.5;
		break;
	case 4: //scroll down and zoom out
		printf("zoom out\n");
		yCamPos += 0.5;
		break;
	}
}

int main(int argc, char **argv) {
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitWindowPosition(-1, -1);
	glutInitWindowSize(1000, 1000);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Voxel Snake");

	initGL();
	buildDLs();

	glutDisplayFunc(draw);
	glutIdleFunc(draw);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyEvents);
	glutSpecialFunc(specialKeys);
	glutMouseFunc(mouseEvents);

	glutMainLoop();

	return 1;
}