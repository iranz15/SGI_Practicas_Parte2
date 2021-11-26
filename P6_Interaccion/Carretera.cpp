/*!
	\file		Reloj_3D.cpp
	\brief		Practica 5 Reloj_3D_Digital
	\author		Jorge Iranzo' <jorirsan@upv.es>
	\date		2021-2022
 */


#define PROYECTO "Reloj3D - Jorge Iranzo"
 //#define _USE_MATH_DEFINES

#include <iostream>		
#include <sstream>
#include <cmath>
#include <ctime>
#include <freeglut.h>
#include <Utilidades.h>



static const int tasaFPS = 60;
static enum { SOLIDO, ALAMBRICO } modoVisu;


//Constantes para guardar la hora actual del sistema
static float tHora;
static float tMinuto;
static float tSegundos;


static float velocidadEsfera[] = { 100.0, 60.0 };

//Inicializacion de la posicion de la camara
static float X = 0;
static float Y = 1;
static float Z = -10;

//Variables de generacion de la carretera
static float amplitud = 4;
static float periodo = 50 ;
static int nQuads = 200;
static int ancho = 10;
static int distancia = 10;

//Variables de control de camara
static float ratioGiro = 5;
static float angulo = 90.0; //IMPORTANTE: Se genera la carretera hacia el eje +Z . Inicialmente desde 0.0.0 la tangente forma 90 grados con +X.
static float velocidad = 0.0;
static int mirar  =  6;		//Constante para controlar la distancia entre la camara y el punto que esta mirando. Se ha escogido de forma arbitraria


/*		Vista de pajaro
		  Carretera
				^ Z+
			   (|
				|)
			   (|
X+    <---------|-------->     -X


*/





void init()
{
	//Inicializacion de metodos que cargan lista para luego poder llamarlas en display o otros metodos
	glClearColor(1.0, 1.0, 1.0, 1.0); // Fondo Blanco
	glEnable(GL_DEPTH_TEST);		//Test de profundidad

}



float normal(int d) {

	return  1 / (sqrtf(1 + pow(d, 2)));

}


float derivada(int punto) {

	return( ( (2 * PI * amplitud) / periodo )* cos( punto * 2 * PI / periodo));
}


void circuito() {

	

	glColor3f(0.0, 0.0, 0.0);
	int l = ancho / 2;
	for (int i = 1; i <= nQuads; i++) {

		int punto = Z + (i - 1) * distancia;

		float d = derivada(punto);
		float n = normal(d);

		float d1 = derivada(punto + distancia);
		float n1 = normal(d1);

		GLfloat v0[3] = { punto - (n * l), 0.0 , punto - (-d * n * l) };
		GLfloat v3[3] = { punto + (n * l) , 0.0 , punto + (-d * n * l) };

		GLfloat v1[3] = { distancia + punto - (n1 * l), 0.0 , distancia + punto - (-d1 * n1 * l) };
		GLfloat v2[3] = { distancia + punto + (n1 * l) , 0.0 , distancia + punto + (-d1 * n1 * l) };


		glPolygonMode(GL_FRONT, GL_LINE);

		quad(v0, v1, v2, v3, 10, 5);

	}
	
	
}


void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (modoVisu == ALAMBRICO) 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	else 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//El seno y cosen dan los vectores unitarios
	gluLookAt(X, Y, Z, X + mirar * cos(angulo * PI / 180), Y, Z + mirar * sin(angulo * PI / 180), 0, 1, 0);


	

	ejes();
	glColor3f(1, 0.3, 0.3);
	//glutSolidCone(0.4, 2, 30, 5);
	circuito();

	

	glutSwapBuffers();


}



void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float razon = (float)w / h;
	gluPerspective(45, razon, 1, 100);
}



#pragma warning(disable : 4996)
void onTimer(int valor) {
	static float antes = 0;
	float ahora, tiempo_transcurrido;

	ahora = glutGet(GLUT_ELAPSED_TIME);
	tiempo_transcurrido = ahora - antes; //en milisegundos

	antes = ahora;
	//Extraccion mediante la libreria time del tiempo actual y grados que se han de rotar dependiendo del tiempo transcurrido
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	tHora = timeinfo->tm_hour;
	tMinuto = timeinfo->tm_min;
	tSegundos = timeinfo->tm_sec;
	Z += sin(angulo * PI / 180) * velocidad * tiempo_transcurrido / 1000.0f;
	X += cos(angulo * PI / 180) * velocidad * tiempo_transcurrido / 1000.0f;
	
	stringstream titulo;
	titulo << fixed;
	titulo.precision(1); // De esta forma no aparecen errores de coma flotante en la ventana
	titulo << velocidad << "m/s"; 
	glutSetWindowTitle(titulo.str().c_str());

	glutTimerFunc(1000 / tasaFPS, onTimer, tasaFPS);
	glutPostRedisplay();
}

void onKey(unsigned char tecla, int x, int y) { 

	switch (tecla) {

		case 'a':
			modoVisu = ALAMBRICO;
			break;
		case 's':
			modoVisu = SOLIDO;
			break;
		case 27:
			exit(0);

	}

	glutPostRedisplay();
}



void onSpecialKey(int specialKey, int x, int y) { 

	switch (specialKey) {
		


		case GLUT_KEY_UP:
			velocidad += 0.1;
			break;
		case GLUT_KEY_DOWN:
			if (velocidad <= 0.1) //Para evitar 
				velocidad = 0;
			else velocidad -= 0.1;
			break;
		case GLUT_KEY_LEFT:
			angulo -= ratioGiro;
			break;
		case GLUT_KEY_RIGHT:
			angulo += ratioGiro;
			break;


	}

	glutPostRedisplay();
}



void main(int argc, char** argv) {
	//Inicializacion, buffer, tama�o ventana inicial, llamada a init, etc...
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow(PROYECTO);
	init();
	std::cout << PROYECTO << " \nA conducir!" << std::endl;
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKey);
	glutSpecialFunc(onSpecialKey);
	//glutIdleFunc(onIdle);
	glutTimerFunc(1000 / tasaFPS, onTimer, tasaFPS);
	glutMainLoop();
}


