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

//Listas estrellas david
static int id;
static int estrella;
static int estrellas;

//Lista Marcador
static int lMarcadores;

//Listas para guardar agujas
static int lHora;
static int lMinuto;
static int lSegundos;

//Angulos de giro de agujas del reloj
static float aConstante = 0.5; //60 minutos por 12 horas = 720  => 360/720=0.5
static float aHora = 30; // 360/12 horas
static float aMinuto = 6; // 360/60 mins/secs
static float aSegundos = 6;

//Constantes para guardar la hora actual del sistema
static float tHora;
static float tMinuto;
static float tSegundos;

//Angulo de giro estrella. Se declara para que sea dependiente de la velocidad de proceso del computador. Giro de 30 grados de base
static float anguloEstrella;
static float velocidadEstrella = 30.0;

//Lo mismo para la esferas, primer componente velocidad de giro en x y segundo en y (grados por segundo)
static float anguloEsferaX;
static float anguloEsferaY;
static float velocidadEsfera[] = { 100.0, 60.0 };

//Inicializacion de la posicion de la camara
static int camaraX = 0;
static int camaraY = 0;
static int camaraZ = 5;


//Practica 2
void estrella_basica() {

	estrella = glGenLists(1);
	glNewList(estrella, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	float rp = 0.7; //radio grande
	float rg = 1.0;  //radio pequeño

	//Se dibuja cada triangulo en sentido horario
	//Sin y Cos estan en rad
	//2 PI es la longitud de un circulo en rad, queremos los TRES vertices del triangulo
	//Triangulo positivo
	for (int i = 0; i < 4; i++) {
		double angulo = i * 2 * PI / 3;
		glVertex3f(rp * sin(angulo), rp * cos(angulo), 0.0);
		glVertex3f(rg * sin(angulo), rg * cos(angulo), 0.0);
	}
	glEnd();

	//Triangulo negativo
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < 4; i++) {
		double angulo = i * 2 * PI / 3;
		glVertex3f(-rp * sin(angulo), -rp * cos(angulo), 0.0);
		glVertex3f(-rg * sin(angulo), -rg * cos(angulo), 0.0);
	}
	glEnd();
	glEndList();
}


//Practica 4
void estrella_compleja() {

	for (int i = 0; i < 6; i++) {
		glColor3f(0 + (i * 0.1), 0 + (i * 0.1), 0.3);
		glPushMatrix();
		glRotatef(30 * i, 0, 1, 0);
		glCallList(estrella);
		glPopMatrix();
	}

}

void esfera(double colorR, double colorG, double colorB, int r, int sl, int st) {

	glColor3f(colorR, colorG, colorB);
	glutWireSphere(r, sl, st);

}

void agujas() {

	//Aguja Hora
	lHora = glGenLists(1);
	glNewList(lHora, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(-0.1, 0.0, -0.1);
	glVertex3f(0.1, 0.0, -0.1);
	glVertex3f(0.0, 0.5, -0.1);
	glEnd();
	glEndList();



	//Aguja Minuto
	lMinuto = glGenLists(1);
	glNewList(lMinuto, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(-0.1, 0.0, -0.2);
	glVertex3f(0.1, 0.0, -0.2);
	glVertex3f(0.0, 1.0, -0.2);
	glEnd();
	glEndList();



	//Aguja Segundos
	lSegundos = glGenLists(1);
	glNewList(lSegundos, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(-0.05, 0.0, 0.0);
	glVertex3f(0.05, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glEnd();
	glEndList();
}

void marcadores() {

	lMarcadores = glGenLists(1);
	glNewList(lMarcadores, GL_COMPILE);


	for (int i = 0; i < 60; i++) {
		glBegin(GL_LINES);
		//Marcador rojo mas grueso cada 5 minutos/1 hora
		if (i % 5 == 0) {
			glColor3f(1, 0.3, 0.3);
			glLineWidth(5);
		}
		//Marcador azul mas fino de minutos (60 en una hora), segundos(60 en un minuto)  y periodos de 12 minutos (5 en cada hora)
		else {
			glColor3f(0.1, 1.0, 1);
			glLineWidth(1);
		}

		double angulo = i * 2 * PI / 60;
		glVertex3f(0.8 * sin(angulo), 0.8 * cos(angulo), 0.1);
		glVertex3f(0.9 * sin(angulo), 0.9 * cos(angulo), 0.1);
		glEnd();
	}
	glLineWidth(1);

	glEndList();
}

void muestraFPS()
// Calcula la frecuencia y la muestra en el título de la ventana 
// cada segundo 
{
	int ahora, tiempo_transcurrido;
	static int antes = 0;
	static int FPS = 0;
	stringstream titulo;

	//Cuenta de llamadas a esta función en el último segundo 
	FPS++;
	ahora = glutGet(GLUT_ELAPSED_TIME);   //Tiempo transcurrido desde el inicio 
	tiempo_transcurrido = ahora - antes;   //Tiempo transcurrido desde antes 
	if (tiempo_transcurrido > 1000) {    //Acaba de rebasar el segundo 
		titulo << "FPS: " << FPS;   //Se muestra una vez por segundo 
		glutSetWindowTitle(titulo.str().c_str());
		antes = ahora;     //Ahora ya es antes  
		FPS = 0;      //Reset del conteo 
	}
}




void init()
{
	//Inicializacion de metodos que cargan lista para luego poder llamarlas en display o otros metodos
	glClearColor(1.0, 1.0, 1.0, 1.0); // Fondo Blanco
	glEnable(GL_DEPTH_TEST);		//Test de profundidad

}

void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camaraX, camaraY, camaraZ, 0, 0, 0, 0, 1, 0); //Siempre mira al origen y la vertical es Y

	

	muestraFPS();

	glutSwapBuffers();


}


void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float razon = (float)w / h;

	//Ahora la razon se ocupa correctamente de la isometria.

	//Para que la esfera toque el extremo superior e inferior tenemos que hacer que el fov siempre este correcto cada vez que se hace reshape
	//Sacamos la distancia entre la posicion de la camara al punto origen 
	//Como es una piramide, la dividimos en dos y por trigonometria extraemos sin(a/2)=altura/distanciaOrig
	//Sabemos que la altura es 1 porque es el tamaño que se pide del circulo.
	//Extraemos el angulo alpha mediante la inversa (arcsin) y dejamos el resultado en grados

	double distanciaOrig = sqrt(pow(camaraX, 2) + pow(camaraY, 2) + pow(camaraZ, 2));
	double alpha = asin(1 / distanciaOrig) * 2 * (180 / PI);
	gluPerspective(alpha, razon, 1, 15);
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
	anguloEstrella += velocidadEstrella * tiempo_transcurrido / 1000.0f;
	anguloEsferaX += velocidadEsfera[0] * tiempo_transcurrido / 1000.0f;
	anguloEsferaY += velocidadEsfera[1] * tiempo_transcurrido / 1000.0f;
	//xd
	glutTimerFunc(1000 / tasaFPS, onTimer, tasaFPS);
	glutPostRedisplay();
}




void main(int argc, char** argv) {
	//Inicializacion, buffer, tamaño ventana inicial, llamada a init, etc...
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow(PROYECTO);
	init();
	std::cout << PROYECTO << " \nParametrizando el espacio-tiempo..." << std::endl;
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//glutIdleFunc(onIdle);
	glutTimerFunc(1000 / tasaFPS, onTimer, tasaFPS);
	glutMainLoop();
}


