/* PROG 49635 MID TERM TAKE HOME PROJECT */
/* OPTION B ( Path Following ) */
/* Joo Hyun Park and Kenji Casibang */


#define _CRT_SECURE_NO_DEPRECATE
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <GL/glu.h>
#include <GL/glut.h>

/*Window Screen sizes*/
const char * Window_Title = "3D Midterm RollerCoaster";
const int Window_Width = 1280;
const int Window_Height = 720;

/*FOV*/
const float Camera_Fovy = 90.0f;

/*Initial values of the land. Everything is 0 and scale is 1*/
float Rotate_Land[3] = { 0.0, 0.0, 0.0 };
float Translate_Land[3] = { 0.0, 0.0, 0.0 };
float Scale_Land[3] = { 1.0, 1.0, 1.0 };

char fileName[] = "3D Midterm RollerCoaster";

struct point {
	double x;
	double y;
	double z;
};

struct spline {
	int numControlPoints;
	struct point *points;
};

/* the array of spline */
struct spline *Splines;

/* total number of splines */
int Total_Num_Splines;

int Num_of_Spline_Points;
int NTB = 0;
const double SECTION_GAP = 0.6;
const int Speed_of_Rollercoaster = 800;

/* Information of Spline*/
point * Spline_Points;
point * Spline_Bi_Normals;
point * Spline_Tangents;
point * Spline_Normals;

/*Disply lists IDs*/
GLuint Track_ID;

/*Catmull-Rome spline formula to calculate the spline point*/

double Spline_Axis_Point(double p1, double p2, double p3, double p4, double u) {
	return (2 * p2) + (-p1 + p3) * u + (2 * p1 - 5 * p2 + 4 * p3 - p4) * pow(u, 2) + (-p1 + 3 * p2 - 3 * p3 + p4) * pow(u, 3);
}
double Spline_Axis_Tangent(double p1, double p2, double p3, double p4, double u) {
	return  (-p1 + p3) + (2 * p1 - 5 * p2 + 4 * p3 - p4) * (2 * u) + (-p1 + 3 * p2 - 3 * p3 + p4) * (3 * pow(u, 2));
}
point Calculate_Spline(point p1, point p2, point p3, point p4, double u) {
	point out;
	out.x = 0.5 * Spline_Axis_Point(p1.x, p2.x, p3.x, p4.x, u);
	out.z = 0.5 * Spline_Axis_Point(p1.y, p2.y, p3.y, p4.y, u);
	out.y = 0.5 * Spline_Axis_Point(p1.z, p2.z, p3.z, p4.z, u);
	return out;
}
point SplineTangentCalc(point p1, point p2, point p3, point p4, double u) {
	point out;
	out.x = 0.5 * Spline_Axis_Tangent(p1.x, p2.x, p3.x, p4.x, u);
	out.z = 0.5 * Spline_Axis_Tangent(p1.y, p2.y, p3.y, p4.y, u);
	out.y = 0.5 * Spline_Axis_Tangent(p1.z, p2.z, p3.z, p4.z, u);
	return out;
}

/*Vector functions*/

point Add_Vectors(point a, point b) {
	point r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	return r;
}

point Multiply_Scalar(double s, point a) {
	point r;
	r.x = a.x * s;
	r.y = a.y * s;
	r.z = a.z * s;
	return r;
}

point Cross_Product(point a, point b) {
	point r;
	r.x = a.y * b.z - a.z * b.y;
	r.y = a.z * b.x - a.x * b.z;
	r.z = a.x * b.y - a.y * b.x;
	return r;
}

point Normalize(point p) {
	double length = sqrt(pow(p.x, 2) + pow(p.y, 2) + pow(p.z, 2));
	point s;
	s.x = p.x / length;
	s.y = p.y / length;
	s.z = p.z / length;
	return s;
}

/*Calculate the splines.*/
void Spline_Generate(spline * splines) {

	int Num_Of_Control_Points = splines[0].numControlPoints;
	Num_of_Spline_Points = Num_Of_Control_Points * Speed_of_Rollercoaster;
	double uInc = 1.0 / Speed_of_Rollercoaster;

	Spline_Normals = new point[Num_of_Spline_Points];
	Spline_Points = new point[Num_of_Spline_Points];
	Spline_Bi_Normals = new point[Num_of_Spline_Points];
	Spline_Tangents = new point[Num_of_Spline_Points];

	int splineIndex = 0;
	for (int i = 0; i < Num_Of_Control_Points - 3; i++) {
		for (double u = 0.0f; u < 1.0; u += uInc) {
			point p0, p1, p2, p3;
			p0 = splines[0].points[i];
			p1 = splines[0].points[i + 1];
			p2 = splines[0].points[i + 2];
			p3 = splines[0].points[i + 3];

			//Calculate spline points//
			point Spline_Point = Calculate_Spline(p0, p1, p2, p3, u);
			point Spline_Tangent = SplineTangentCalc(p0, p1, p2, p3, u);
			Spline_Tangents[splineIndex] = Spline_Tangent;
			Spline_Tangent = Normalize(Spline_Tangent);

			//Spline points//
			point Normal_Spline;
			point Bi_Normal_Spline;

			if (splineIndex == 0) {
				point V0;
				V0.x = 0.0; V0.y = 0.0; V0.z = -1.0;
				Normal_Spline = Normalize(Cross_Product(Spline_Tangent, V0));
				Bi_Normal_Spline = Normalize(Cross_Product(Spline_Tangent, Normal_Spline));
			}
			else {
				point previousBinormal = Spline_Bi_Normals[splineIndex - 1];
				Normal_Spline = Normalize(Cross_Product(previousBinormal, Spline_Tangent));
				Bi_Normal_Spline = Normalize(Cross_Product(Spline_Tangent, Normal_Spline));
			}

			/*Update the arrays*/
			Spline_Points[splineIndex] = Spline_Point;
			Spline_Normals[splineIndex] = Normal_Spline;
			Spline_Bi_Normals[splineIndex] = Bi_Normal_Spline;
			splineIndex++;
		}
	}

	Num_of_Spline_Points = splineIndex;
}

int Spline_Load(char *argv) {
	char *File_Name = (char *)malloc(128 * sizeof(char));
	FILE *File_List;
	FILE *Spline_File;
	int iType, i = 0, j, iLength;

	/* Open and scan the file */
	File_List = fopen(argv, "r");
	if (File_List == NULL) {
		printf("can't open file\n");
		exit(1);
	}

	fscanf(File_List, "%d", &Total_Num_Splines);

	Splines = (struct spline *)malloc(Total_Num_Splines * sizeof(struct spline));

	//Read the file and calculate splines//
	for (j = 0; j < Total_Num_Splines; j++) {
		i = 0;
		fscanf(File_List, "%s", File_Name);
		Spline_File = fopen(File_Name, "r");

		if (Spline_File == NULL) {
			printf("can't open file\n");
			exit(1);
		}

		fscanf(Spline_File, "%d %d", &iLength, &iType);
		Splines[j].points = (struct point *)malloc(iLength * sizeof(struct point));
		Splines[j].numControlPoints = iLength;

		//save the data to the spline struct// 
		while (fscanf(Spline_File, "%lf %lf %lf",
			&Splines[j].points[i].x,
			&Splines[j].points[i].y,
			&Splines[j].points[i].z) != EOF) {
			i++;
		}
	}

	free(File_Name);

	return 0;
}

//Drawing splines functions//

void Draw_Left_Trail(point p0, point p1, point p2, point p3, point p4, point p5, point p6, point p7) {
	//Top//
	glBegin(GL_QUADS);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p6.x, p6.y, p6.z);
	glVertex3f(p5.x, p5.y, p5.z);
	glEnd();

	//Right//
	glBegin(GL_QUADS);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p4.x, p4.y, p4.z);
	glVertex3f(p0.x, p0.y, p0.z);
	glEnd();

	//Left//
	glBegin(GL_QUADS);
	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p6.x, p6.y, p6.z);
	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glEnd();

	//Bottom//
	glBegin(GL_QUADS);
	glVertex3f(p0.x, p0.y, p0.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p4.x, p4.y, p4.z);
	glEnd();

}

void Draw_Cross_Section(point p0, point tangent, point normal, point binormal, double sectionGap, double Section_Size = 0.03) {
	glColor3f(0.0, 1.0, 1.0);

	
	point v1 = Add_Vectors(p0, Multiply_Scalar(Section_Size, normal));
	point v3 = Add_Vectors(p0, Multiply_Scalar(sectionGap * 0.9, binormal));
	point v2 = Add_Vectors(v3, Multiply_Scalar(Section_Size, normal));

	//Front//
	glBegin(GL_QUADS);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
	glVertex3f(p0.x, p0.y, p0.z);
	glEnd();
}

void drawBisection(int Spline_Index, double Spline_Scale, double Interval_Section = 0.5, int uInterval = 1, bool Cross_Section = false) {
	
	glColor3f(1.0, 0.0, 1.0);

	point Biscection_Tangent = Normalize(Spline_Tangents[Spline_Index]);
	point points1[4];
	points1[2] = Spline_Points[Spline_Index];
	points1[1] = Add_Vectors(Spline_Points[Spline_Index], Multiply_Scalar(Spline_Scale, Spline_Bi_Normals[Spline_Index]));
	points1[0] = Add_Vectors(points1[1], Multiply_Scalar(-1 * Spline_Scale, Spline_Normals[Spline_Index]));
	points1[3] = Add_Vectors(Spline_Points[Spline_Index], Multiply_Scalar(-Spline_Scale, Spline_Normals[Spline_Index]));

	Biscection_Tangent = Normalize(Spline_Tangents[Spline_Index + uInterval]);
	point points2[4];
	points2[2] = Spline_Points[Spline_Index + uInterval];
	points2[1] = Add_Vectors(Spline_Points[Spline_Index + uInterval], Multiply_Scalar(Spline_Scale, Spline_Bi_Normals[Spline_Index + uInterval]));
	points2[0] = Add_Vectors(points2[1], Multiply_Scalar(-1 * Spline_Scale, Spline_Normals[Spline_Index + uInterval]));
	points2[3] = Add_Vectors(Spline_Points[Spline_Index + uInterval], Multiply_Scalar(-Spline_Scale, Spline_Normals[Spline_Index + uInterval]));

	//Draw left section//
	Draw_Left_Trail(points1[0], points1[1], points1[2], points1[3], points2[0], points2[1], points2[2], points2[3]);

	//DrawCross section//
	if (Cross_Section) {
		Draw_Cross_Section(points1[0], Biscection_Tangent, Spline_Normals[Spline_Index], Spline_Bi_Normals[Spline_Index], Interval_Section);
	}

	glColor3f(1.0, 0.0, 1.0);
	for (point & v : points1) {
		v = Add_Vectors(v, Multiply_Scalar(Interval_Section, Spline_Bi_Normals[Spline_Index]));
	}
	for (point & v : points2) {
		v = Add_Vectors(v, Multiply_Scalar(Interval_Section, Spline_Bi_Normals[Spline_Index + uInterval]));
	}

	//Draw right section//
	Draw_Left_Trail(points1[0], points1[1], points1[2], points1[3], points2[0], points2[1], points2[2], points2[3]);
}

void drawSpline() {
	Track_ID = glGenLists(1);
	glNewList(Track_ID, GL_COMPILE);

	//Draw Bisection//
	for (int i = 0; i < Num_of_Spline_Points - 10; i += 10) {

		double a = 0.07;

		drawBisection(i, a, SECTION_GAP, 10, i % 100 == 0);
	}

	glEndList();
}

/*Follow the track as first person perspective*/
void NTV_View(point Spline_Point, point tangent, point normal, point binormal) {

	point eye = Add_Vectors(Spline_Point, Multiply_Scalar(0.2, normal));
	eye = Add_Vectors(eye, Multiply_Scalar(SECTION_GAP / 2, binormal));

	point up = normal;
	point center = Add_Vectors(eye, tangent);
	
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	/*Update the camera*/
	if (NTB >= Num_of_Spline_Points)
		NTB = 0;
	NTV_View(Spline_Points[NTB], Spline_Tangents[NTB], Spline_Normals[NTB], Spline_Bi_Normals[NTB]);
	NTB++;

	/*Transform to the current world state */
	glTranslatef(Translate_Land[0], Translate_Land[1], Translate_Land[2]);
	glRotatef(Rotate_Land[0], 1.0, 0.0, 0.0);
	glRotatef(Rotate_Land[1], 0.0, 1.0, 0.0);
	glRotatef(Rotate_Land[2], 0.0, 0.0, 1.0);
	glScalef(Scale_Land[0], Scale_Land[1], Scale_Land[2]);

	//drawSpline();
	glCallList(Track_ID);

	glutSwapBuffers();
}

//Update screen
void doIdle()
{
	glutPostRedisplay();
}

void Init_RollerCoaster()
{
	//Spline initiate
	Spline_Generate(Splines);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	drawSpline();
	glEnable(GL_DEPTH_TEST);

}

//change the aspect depends on window size
void Window_Reshape(int w, int h) {
	GLfloat Window_Aspect = (GLfloat)w / (GLfloat)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(Camera_Fovy, Window_Aspect, 0.01, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, _TCHAR* argv[])
{

	if (argc < 2)
	{
		printf("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}

	//Load splines file
	Spline_Load(argv[1]);

	glutInit(&argc, (char**)argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);

	//Create Window
	glutInitWindowSize(Window_Width, Window_Height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(fileName);

	//Glut Display
	glutDisplayFunc(display);
	glutIdleFunc(doIdle);
	glutReshapeFunc(Window_Reshape);

	Init_RollerCoaster();

	glutMainLoop();

	return 0;
}