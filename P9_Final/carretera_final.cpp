/*!
	\file		carretera_texturas.cpp
	\brief		Practica 8 Iluminacion
	\author		Jorge Iranzo' <jorirsan@upv.es>
	\date		2021-2022
 */


#define PROYECTO "Practica P9 Conduccion - Creado por Jorge Iranzo Sanchez"
 //#define _USE_MATH_DEFINES
#include <iostream>		
#include <sstream>
#include <cmath>
#include <ctime>
#include <freeglut.h>
#include <Utilidades.h>
#include <irrKlang.h>

#pragma comment(lib, "irrKlang.lib")

using namespace irrklang;	

ISoundEngine* engine;
ISound* cancionfondo;


static const int tasaFPS = 60;

//Constantes para guardar la hora actual del sistema
static float tHora;
static float tMinuto;
static float tSegundos;

//Inicializacion de la posicion de la camara
static float X = 0;
static float Y = 1;
static float Z = 0;

//Variables de generacion de la carretera
static float amplitudCarretera = 8;
static float periodo = 90;
static int nQuads = 90;
static int ancho = 4;
int l = ancho / 2;
static int distancia = 1;
static float threshold = 10;

//Variables de control de camara
float ratioGiro = 2;
static float angulo = 90.0; //IMPORTANTE: Se genera la carretera hacia el eje +Z . Inicialmente desde 0.0.0 la tangente forma 90 grados con +X.
static float velocidad = 0.0;
static int mirar = 1;		//Constante para controlar la distancia entre la camara y el punto que esta mirando. Se ha escogido de forma arbitraria

//VARIABLES ANIMACION
//Angulo de giro de la estructura estrella. 
static float anguloEstrella;
float velocidadEstrella = 10.0;

//Ratio de desplazamiento de textura para el panel flechas.
static float desplazamientoTextura;
float ratioAnimacion = 0.2f;

//Ratio de desplazamiento de la estructura panel flechas para que haga el efecto que esta flotando en el espacio. 
static float movimientoPanel;
float amplitudAnimacion = 10.f;


//Flags de control y valores por defecto
//Vista normal = 1/ Vista pajaro = 0
static int modoVista = 1;
static int noche = 0;
static int niebla = 0;
static int HUD = 1;
static int modoSimple = 0;
static int mensajes = 0;
static int cielo = 3;
static int sonido = 1;
static int dibujaEjes = 0;

//Lista de variables de textura
GLuint textura[16];

//Lista de variables de textura
int totalcanciones = 2;
static int idcancionActual = 0;
static float auxVolumen = 0.f;


/*		Vista de pajaro
		  Carretera
				^ Z+
			   (|
				|)
			   (|
X+    <---------|-------->     -X


*/

/* A partir del metodo descrito en Utilidades.h, esta implementacion de quadtex no tiene el calculo de normales. 
   Asi si a estos quads se le aplica luz, se aplicara a FRONT y BACK.
   Esto nos sirve para los decoracion de la carretera en detalles(); */
void quadtexAlter(GLfloat v0[3], GLfloat v1[3], GLfloat v2[3], GLfloat v3[3],
	GLfloat smin, GLfloat smax, GLfloat tmin, GLfloat tmax,
	int M, int N)
	// Dibuja un cuadrilatero con resolucion MxN con normales y coordenadas de textura
{
	if (M < 1) M = 1; if (N < 1) N = 1;	// Resolucion minima
	GLfloat ai[3], ci[3], bj[3], dj[3], p0[3], p1[3];
	// calculo de la normal (v1-v0)x(v3-v0) unitaria 
	GLfloat v01[] = { v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
	GLfloat v03[] = { v3[0] - v0[0], v3[1] - v0[1], v3[2] - v0[2] };
	GLfloat normal[] = { v01[1] * v03[2] - v01[2] * v03[1] ,
						 v01[2] * v03[0] - v01[0] * v03[2] ,
						 v01[0] * v03[1] - v01[1] * v03[0] };
	//float norma = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);			//* Se comenta
	//glNormal3f(normal[0] / norma, normal[1] / norma, normal[2] / norma);									//* Se comenta
	// ai: punto sobre el segmento v0v1, bj: v1v2, ci: v3v2, dj: v0v3
	for (int i = 0; i < M; i++) {
		// puntos sobre segmentos a y c
		for (int k = 0; k < 3; k++) {
			ai[k] = v0[k] + i * (v1[k] - v0[k]) / M;
			ci[k] = v3[k] + i * (v2[k] - v3[k]) / M;
		}
		// strip vertical. i=s, j=t
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= N; j++) {
			for (int k = 0; k < 3; k++) {
				// puntos sobre los segmentos b y d
				bj[k] = v1[k] + j * (v2[k] - v1[k]) / N;
				dj[k] = v0[k] + j * (v3[k] - v0[k]) / N;

				// p0= ai + j/N (ci-ai)
				p0[k] = ai[k] + j * (ci[k] - ai[k]) / N;
				// p1= p0 + 1/M (bj-dj)
				p1[k] = p0[k] + (bj[k] - dj[k]) / M;
			}
			// punto izquierdo
			glTexCoord2f(smin + (smax - smin) * i / M, tmin + (tmax - tmin) * j / N);  // s,t
			glVertex3f(p0[0], p0[1], p0[2]);
			// punto derecho
			glTexCoord2f(smin + (smax - smin) * (i + 1) / M, tmin + (tmax - tmin) * j / N);
			glVertex3f(p1[0], p1[1], p1[2]);
		}
		glEnd();
	}
}





void texturas() {

	//Carretera
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textura[1]);
	glBindTexture(GL_TEXTURE_2D, textura[1]);
	loadImageFile((char*)"./texturas/mario_kart_road.png");

	//Anuncio
	glGenTextures(1, &textura[2]);
	glBindTexture(GL_TEXTURE_2D, textura[2]);
	loadImageFile((char*)"./texturas/anuncio_kirby.png");

	//Fondo 1
	glGenTextures(1, &textura[3]);
	glBindTexture(GL_TEXTURE_2D, textura[3]);
	loadImageFile((char*)"./texturas/rainbow_road_fondo.jpg");

	//Fondo 2
	glGenTextures(1, &textura[4]);
	glBindTexture(GL_TEXTURE_2D, textura[4]);
	loadImageFile((char*)"./texturas/agujero_fondo.jpg");

	glGenTextures(1, &textura[5]);
	glBindTexture(GL_TEXTURE_2D, textura[5]);
	loadImageFile((char*)"./texturas/estrellas_fondo.png");

	glGenTextures(1, &textura[6]);
	glBindTexture(GL_TEXTURE_2D, textura[6]);
	loadImageFile((char*)"./texturas/luna_fondo_2.jpg");

	glGenTextures(1, &textura[7]);
	glBindTexture(GL_TEXTURE_2D, textura[7]);
	loadImageFile((char*)"./texturas/postes.png");

	glGenTextures(1, &textura[8]);
	glBindTexture(GL_TEXTURE_2D, textura[8]);
	loadImageFile((char*)"./texturas/rainbow_road_circle_star.png");

	glGenTextures(1, &textura[9]);
	glBindTexture(GL_TEXTURE_2D, textura[9]);
	loadImageFile((char*)"./texturas/borde_carretera.png");


	glGenTextures(1, &textura[10]);
	glBindTexture(GL_TEXTURE_2D, textura[10]);
	loadImageFile((char*)"./texturas/textura_jupiter.png");

	glGenTextures(1, &textura[11]);
	glBindTexture(GL_TEXTURE_2D, textura[11]);
	loadImageFile((char*)"./texturas/flechas_textura.png");

	glGenTextures(1, &textura[12]);
	glBindTexture(GL_TEXTURE_2D, textura[12]);
	loadImageFile((char*)"./texturas/logo_textura.png");

	glGenTextures(1, &textura[13]);
	glBindTexture(GL_TEXTURE_2D, textura[13]);
	loadImageFile((char*)"./texturas/detalles_borde_final.png");

	glGenTextures(1, &textura[14]);
	glBindTexture(GL_TEXTURE_2D, textura[14]);
	loadImageFile((char*)"./texturas/detalles_borde_final_esfera.png");

	glGenTextures(1, &textura[15]);
	glBindTexture(GL_TEXTURE_2D, textura[15]);
	loadImageFile((char*)"./texturas/poste_flechas.jpg");
}


void init()
{
	//Inicializacion de metodos que cargan lista para luego poder llamarlas en display o otros metodos
	 // Fondo Blanco

	glEnable(GL_DEPTH_TEST);		//Test de profundidad
	glEnable(GL_NORMALIZE);			//Normalizacion vectores iluminacion
	texturas();						//Carga todas las texturas. NO se puede poner en el display
									//porque si no estaria cargando las texturas cada frame y va lentisimo
	engine = createIrrKlangDevice();   //Creamos el controlador de sonido
	//Cancion Base
	cancionfondo = engine->play2D("./sonidos/Rainbow_Road_DS_Song_00.mp3", true, false, true);
	cancionfondo->setVolume(0.3);

	std::cout << PROYECTO << "\n---------------------\nBienvenido a la Senda Arcoiris!\n\n";
	std::cout << "CONTROLES BASICOS:\n";
	std::cout << "Flecha izquierda / derecha: giro del vehiculo \n";
	std::cout << "Flecha arriba/abajo: aumento/disminucion de la velocidad \n";
	std::cout << "S/s: Activa/desactiva un modelo simple en alambrico sin luces ni texturas\nSi esta activo N/n se puede visualizar la resolucion adicional de los poligonos para conseguir luces mas realistas \n";
	std::cout << "L/l: Cambia entre modo diurno/nocturno \n";
	std::cout << "N/n: Cambia el estado de la niebla (on/off). Varia si L/l esta activado \n";
	std::cout << "C/c: Cambia la visibilidad de elementos solidarios a la cámara -HUD-. El HUD contiene informacion acerca de \nla velocidad actual, el norte (eje -Z) y otros modos implementados \n";
	std::cout << "---------------------\n";
	std::cout << "MEJORAS/CONTROLES AVANZADOS:\n";
	std::cout << "T/t: Activa/desactiva mensajes de ayuda que indican que modos estan activos. ( Se ha considerado que estos mensajes no forman del HUD,\npor lo que C/c no afecta a este opcion ) \n";
	std::cout << "V/v: Activa/desactiva una vista de aguila para ver el circuito desde arriba. Se desactiva el movimiento mediante flechas en este modo \n";
	std::cout << "W/w: Cambia la textura de fondo del cielo entre una seleccion de imagenes \n";
	std::cout << "M/m: Activa/desactiva la musica y los sonidos \n";
	std::cout << "K/k: Si estan activos los sonidos (M/m) cambia la cancion actual entre una seleccion de canciones \n";
	std::cout << "R/r: Reseta y pone los valores predeterminados de los modos descritos previamente (visual y sonoro) \n";
	std::cout << "E/e: Dibuja/Desdibuja unos ejes en (0,0,0) \n";
	std::cout << "J/j: Guarda una captura del estado actual del programa en el directorio donde esta el .exe \n";

}

float funcionCarretera(float punto) {

	return amplitudCarretera * sin(punto * 2 * PI / periodo);

}


float normal(float d) {

	return  1 / (sqrtf(1 + powf(d, 2)));

}


float derivada(float punto,float amplitud) {

	return(((2 * PI * amplitud) / periodo) * cos(punto * 2 * PI / periodo));
}

void anuncio(float distancia) {
	glPushMatrix();
	glColor3f(0, 0, 0);
	float detras = 0;
	float inicioPeriodo = (int)Z - (int)Z % (int)periodo;
	float distanciaAnuncio = periodo / distancia;

	if (inicioPeriodo + distanciaAnuncio < Z) { detras = 1; }
	distanciaAnuncio += inicioPeriodo + (periodo * detras);

	float x = funcionCarretera(distanciaAnuncio);
	float d = derivada(distanciaAnuncio, amplitudCarretera);
	float n = normal(d);

	GLfloat v0[3] = { x - (n * l), 0.0 , distanciaAnuncio - (-1 * d * n * l) };
	GLfloat v3[3] = { x + (n * l) , 0.0 , distanciaAnuncio + (-1 * d * n * l) };
	GLfloat a0[3] = { v0[0], 2, v0[2] };
	GLfloat a1[3] = { v0[0], 3 ,v0[2] };
	GLfloat a2[3] = { v3[0], 3, v3[2] };
	GLfloat a3[3] = { v3[0], 2, v3[2] };

	glBindTexture(GL_TEXTURE_2D, textura[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//alternativamente GL_REPLACE o se puede usar el canal alfa con GL_BLEND
	
	if (noche) {
		GLfloat lP[] = { x, 2.5, distanciaAnuncio - 4, 1.0 };
		GLfloat lD[] = { 0.8, 0.8, 0.8, 1.0 };
		GLfloat lM[] = { 0.0, 0.0, 1.0 };
		glLightfv(GL_LIGHT6, GL_POSITION, lP);
		glLightfv(GL_LIGHT6, GL_DIFFUSE, lD);
		glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, lM);
		glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, 40.0);
		glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, 25.0);
		glLightf(GL_LIGHT6, GL_QUADRATIC_ATTENUATION, 0.02);
		
		glEnable(GL_LIGHT6);
	}

	else {
		glDisable(GL_LIGHT6);
	}

	quadtex(a3, a0, a1, a2, 0, 1, 0, 1, 10, 10);

	//Dibujamos los postes
	GLUquadric* poste = gluNewQuadric();
	gluQuadricTexture(poste, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, textura[7]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPushMatrix();
	glTranslatef(a0[0] + 0.12, 0, a0[2]);
	glRotatef(90, -1, 0, 0);
	gluCylinder(poste, 0.15, 0.15, 3, 5, 1);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(a3[0] - 0.12, 0, a3[2]);
	glRotatef(90, -1, 0, 0);
	gluCylinder(poste, 0.15, 0.15, 3, 5, 1);
	glPopMatrix();
	

	glPopMatrix();

}

void circuito() {
	glPushMatrix();
	if (noche & !modoSimple) {

		//Emisivo por defecto
		//Ambiental por defecto
		GLfloat cD[] = { 0.8,0.8,0.8,1.0 };
		GLfloat cS[] = { 0.3,0.3,0.3,1.0 };
		GLfloat s = 3.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cD);
		glMaterialfv(GL_FRONT, GL_SPECULAR, cS);
		glMaterialf(GL_FRONT, GL_SHININESS, s);
		
	}
	else { glColor3f(0, 0, 0); }
	for (int i = 1; i <= nQuads; i++) {
		float detras = 0;
		
		float inicioPeriodo = ((int)Z - (int)Z % (int)periodo) ;
		float punto = inicioPeriodo + (i - 1) * distancia ;
		if (punto< Z) { punto += periodo; }
		float siguiente = punto + distancia;

		float x = funcionCarretera(punto);
		float d = derivada(punto, amplitudCarretera);
		float n = normal(d);

		float x1 = funcionCarretera(siguiente);
		float d1 = derivada(siguiente, amplitudCarretera);
		float n1 = normal(d1);

		GLfloat v0[3] = { x - (n * l), 0.0 , punto - (-1 * d * n * l) };
		GLfloat v3[3] = { x + (n * l) , 0.0 , punto + (-1 * d * n * l) };

		GLfloat v1[3] = { x1 - (n1 * l), 0.0 , siguiente - (-1 * d1 * n1 * l) };
		GLfloat v2[3] = { x1 + (n1 * l) , 0.0 , siguiente + (-1 * d1 * n1 * l) };

		
		
		glBindTexture(GL_TEXTURE_2D, textura[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if(noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		
		//GL_MODULATE/ GL_REPLACE o se puede usar el canal alfa con GL_BLEND
		if (!noche) { quadtex(v0, v1, v2, v3, 0, -1, 0, -1, 1, 1); }
		else { 
			if (punto <= Z+7) { quadtex(v0, v1, v2, v3, 0, -1, 0, -1, 30, 110); }
			else { quadtex(v0, v1, v2, v3, 0, -1, 0, -1, 2, 2); }
		}
		
		
	}
	glPopMatrix();
}
void estrella(float distancia) {
	float detras = 0;
	float inicioPeriodo = (int)Z - (int)Z % (int)periodo;
	float distanciaAnuncio = periodo / distancia;

	if (inicioPeriodo + distanciaAnuncio < Z) { detras = 1; }
	distanciaAnuncio += inicioPeriodo + (periodo * detras);

	float x = funcionCarretera(distanciaAnuncio);
	float d = derivada(distanciaAnuncio, amplitudCarretera);
	float n = normal(d);
	GLfloat v0[3] = { x - (n * l), 0.0 , distanciaAnuncio - (-1 * d * n * l) };
	GLfloat v3[3] = { x + (n * l) , 0.0 , distanciaAnuncio + (-1 * d * n * l) };

	GLfloat a0[3] = { v0[0], 2, v0[2] };
	GLfloat a1[3] = { v0[0], 3 ,v0[2] };
	GLfloat a2[3] = { v3[0], 3, v3[2] };
	GLfloat a3[3] = { v3[0], 2, v3[2] };

	//Con quadtex ocurre que al pasar por la estrella, el hud desaparace durant un momento,
	// por lo que se ha optado por una geometria que por el centro no halla espacio de dibujo
	//quadtex(a3, a0, a1, a2, 0, 1, 0, 1, 10, 10); 
	glPushMatrix();
	GLUquadric* estrCirc = gluNewQuadric();
	gluQuadricTexture(estrCirc, GL_TRUE);
	gluQuadricOrientation(estrCirc, GLU_INSIDE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, textura[8]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTranslatef(x, 0, distanciaAnuncio);
	glRotatef(30, 0.0, 1.0, 0.0);
	glRotatef(anguloEstrella, 0, 0, -1);
	if (noche) gluDisk(estrCirc, 2.0, 6, 20, 8);
	else gluDisk(estrCirc, 2.0, 6, 14, 1);



	glDisable(GL_BLEND);
	glPopMatrix();

}


void detalles( int inicial,int final ) {
	glEnable(GL_BLEND);
	GLUquadric* detallesFinales = gluNewQuadric();
	gluQuadricTexture(detallesFinales, GL_TRUE);
	GLUquadric* detallesfinalesEsfera = gluNewQuadric();
	gluQuadricTexture(detallesfinalesEsfera, GL_TRUE);
	for (int i = inicial; i <= final; i++) {
		float detras = 0;

		float inicioPeriodo = ((int)Z - (int)Z % (int)periodo);
		float punto = inicioPeriodo + (i - 1) * distancia;
		if (punto < Z) { punto += periodo; }
		float siguiente = punto + distancia;

		float x = funcionCarretera(punto);
		float d = derivada(punto, amplitudCarretera);
		float n = normal(d);

		float x1 = funcionCarretera(siguiente);
		float d1 = derivada(siguiente, amplitudCarretera);
		float n1 = normal(d1);

		GLfloat v0[3] = { x - (n * l), 0.0 , punto - (-1 * d * n * l) };
		GLfloat v3[3] = { x + (n * l) , 0.0 , punto + (-1 * d * n * l) };

		GLfloat v1[3] = { x1 - (n1 * l), 0.0 , siguiente - (-1 * d1 * n1 * l) };
		GLfloat v2[3] = { x1 + (n1 * l) , 0.0 , siguiente + (-1 * d1 * n1 * l) };

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, textura[9]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		float altura = 0.5f;
		GLfloat v0YBorde[3] = { v0[0], altura , v0[2] };
		GLfloat v3YBorde[3] = { v3[0] , altura , v3[2] };

		GLfloat v1YBorde[3] = { v1[0], altura , v1[2] };
		GLfloat v2YBorde[3] = { v2[0] , altura , v2[2] };

		//Borde Derecha
		glPushMatrix();
		glDepthMask(GL_FALSE);
		if (noche) quadtexAlter(v3, v2, v2YBorde, v3YBorde, 0, 1, 0, 1, 7, 7);
		else quadtexAlter(v3, v2, v2YBorde, v3YBorde, 0, 1, 0, 1, 1, 1);
		glDepthMask(GL_TRUE);
		glPopMatrix();

		//Borde Izquierda
		glPushMatrix();
		glDepthMask(GL_FALSE);
		if (noche) quadtexAlter(v0, v1, v1YBorde, v0YBorde, 0, 1, 0, 1, 7, 7);
		else quadtexAlter(v0, v1, v1YBorde, v0YBorde, 0, 1, 0, 1, 1, 1);
		glDepthMask(GL_TRUE);
		glPopMatrix();

		//Retoques de principio y final de carretera

		if (i == inicial || i == final) {
			
			
		glBindTexture(GL_TEXTURE_2D, textura[13]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		//Detalles final recorrido borde izquierda
		glPushMatrix();
		
		if (i == inicial) glTranslatef(v3[0] , 0, v3[2]);
		else glTranslatef(v2[0], 0, v2[2]);
		glRotatef(90, -1, 0, 0);
		gluCylinder(detallesFinales, 0.05, 0.05, altura, 5, 1);
		glPopMatrix();

		//Detalles final recorrido borde derecha
		glPushMatrix();
		if (i == inicial) glTranslatef(v0[0], 0, v0[2]);
		else glTranslatef(v1[0], 0, v1[2]);
		glRotatef(90, -1, 0, 0);
		gluCylinder(detallesFinales, 0.05, 0.05, altura, 5, 1);
		glPopMatrix();

		
		glBindTexture(GL_TEXTURE_2D, textura[14]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		//Detalles final recorrido borde izquierda Bola
		glPushMatrix();
		if (i == inicial) glTranslatef(v3[0], altura, v3[2]);
		else glTranslatef(v2[0], altura, v2[2]);
		//glRotatef(90, -1, 0, 0);
		gluSphere(detallesfinalesEsfera, 0.1, 5, 5);
		glPopMatrix();

		//Detalles final recorrido borde derecha Bola
		glPushMatrix();
		if (i == inicial) glTranslatef(v0[0], altura, v0[2]);
		else glTranslatef(v1[0], altura, v1[2]);
		//glRotatef(90,-1, 0,0);
		gluSphere(detallesfinalesEsfera, 0.1, 5, 5);
		glPopMatrix();



		}
	
	
	}
	glDisable(GL_BLEND);
}



void panelflechas() {
	
	
	float detras = 0;

	float inicioPeriodo = ((int)Z - (int)Z % (int)periodo);
	float punto = inicioPeriodo + (nQuads * 0.85 * distancia);
	if (punto < Z+4) { punto += periodo; }
	float siguiente = punto + distancia;

	float x = funcionCarretera(punto);
	float d = derivada(punto, amplitudCarretera);
	float n = normal(d);

	float x1 = funcionCarretera(siguiente);
	float d1 = derivada(siguiente, amplitudCarretera);
	float n1 = normal(d1);
	float offsetX = 3.f;
	float offsetZ = 3.f; 
	float altura = 2.f;
	GLfloat v0[3] = { x - (n * l) , 0.0 , punto - (-1 * d * n * l)  };
	GLfloat v1[3] = { x1 - (n1 * l) + offsetX  , 0.0 , siguiente - (-1 * d1 * n1 * l) + offsetZ };
		

	GLfloat v0YBorde[3] = { v0[0], altura , v0[2] };
	GLfloat v1YBorde[3] = { v1[0], altura , v1[2] };


	glPushMatrix(); 
	//Animamos la estructura del panel para que parezca flotar
	glTranslatef(0, derivada(movimientoPanel, amplitudAnimacion) , 0);

	glBindTexture(GL_TEXTURE_2D, textura[11]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPushMatrix();
	glTranslatef(-4, 2, 0);
	//Movemos la textura del panel para que parezca estar encendido
	quadtexAlter(v0, v1, v1YBorde, v0YBorde, 0.f + desplazamientoTextura, -1.f + desplazamientoTextura, 0, -1 , 2, 2);
	glPopMatrix();


	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textura[15]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	GLUquadricObj* base = gluNewQuadric();
	gluQuadricDrawStyle(base, GLU_FILL);
	gluQuadricTexture(base, TRUE);
	gluQuadricNormals(base, GLU_SMOOTH);
	GLUquadricObj* baseEsfera = gluNewQuadric();
	gluQuadricDrawStyle(baseEsfera, GLU_FILL);
	gluQuadricTexture(baseEsfera, TRUE);
	gluQuadricNormals(baseEsfera, GLU_SMOOTH);

	glPushMatrix();
	glTranslatef(-4, 0, 0);
	glTranslatef(v1[0], 0, v1[2]);
	glRotatef(90, -1, 0, 0);
	gluCylinder(base, 0.2, 0.2, altura*2.f,5,5);
	glScalef(0.5, 0.5, 0.5);
	gluSphere(base, 1, 5, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-4, 0, 0);
	glTranslatef(v0[0] , 0, v0[2] );
	glRotatef(90, -1, 0, 0);
	gluCylinder(base, 0.2, 0.2, altura * 2.f, 5, 5);
	glScalef(0.5, 0.5, 0.5);
	gluSphere(base, 1, 5,5);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();

}

void fondo() {
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textura[cielo]);

	if (noche) {
		GLfloat cD[] = { 0.0,0.0,0.0,1.0 };
		GLfloat cS[] = { 0.0,0.0,0.0,1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cD);
		glMaterialfv(GL_FRONT, GL_SPECULAR, cS);
		if(cielo == 4){
		GLfloat cE[] = { 0.1,0.1,0.1,1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, cE);
		}
	}
	float nquads = 10;
	float alpha = 2 * PI / nquads;
	float r = 100;
	float x = r * cos(0); //0 porque en la priemra tiracion del bloque se incilizada en alpha * 1 = alpha (1)
	float z = r * sin(0); //(0)
	int altura = 200;
	GLfloat cil0[3] = { x + X,-(altura / 2), z + Z };
	GLfloat cil1[3];
	GLfloat cil2[3];
	GLfloat cil3[3] = { x + X, altura / 2,z + Z };
	for (int i = 1; i <= nquads; i++) {
		cil1[0] = r * (float)cos(i * alpha) + X;
		cil1[1] = cil0[1];
		cil1[2] = r * (float)sin(i * alpha) + Z;

		cil2[0] = r * (float)cos(i * alpha) + X;
		cil2[1] = cil3[1];
		cil2[2] = r * (float)sin(i * alpha) + Z;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glColor3f(0, 0, 0);
		quadtex(cil0, cil1, cil2, cil3, 0.25+(i *alpha) / (2 * PI),0.25 + ((i + 1.f) * alpha) / (2 * PI), 0, 1,4,4);
		for (int j = 0; j < 3; j++) {
			cil0[j] = cil1[j];
			cil3[j] = cil2[j];
		}
	}
	glPopMatrix();

	
}

void planeta(float distancia) {

	glPushMatrix();
	GLfloat planoS[4] = { 1,0,0,0 };
	GLfloat planoT[4] = { 0,0,1,0 };

	float detras = 0;
	float inicioPeriodo = (int)Z - (int)Z % (int)periodo;
	float distanciaPlaneta =  (periodo / 4.f) -1;
	if (inicioPeriodo + distanciaPlaneta < Z) { detras = 1; }
	distanciaPlaneta += inicioPeriodo + (periodo * detras);

	float x = funcionCarretera(distanciaPlaneta);

	glBindTexture(GL_TEXTURE_2D, textura[10]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_BORDER_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_BORDER_EXT);

	if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, planoS);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, planoT);

	GLUquadricObj* sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	glTranslatef(x-10, -2, distanciaPlaneta);
	glScalef(3,3,3);
	glRotatef((-anguloEstrella)/2, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	gluSphere(sphere, 1.0, 20, 20);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();


}

void luzfoco() {

	if (noche ) {
		if(modoVista){
		//glPushMatrix();
		GLfloat focoP[] = { 0, 0.7, 0, 1.0 };
		GLfloat focoA[] = { 0.2, 0.2, 0.2, 1.0 };
		GLfloat focoD[] = { 1.00, 1.00, 1.00, 1.0 };
		GLfloat focoS[] = { 0.3, 0.3, 0.3, 1.0 };
		GLfloat focoM[] = { 0.0, -0.5, -0.7 };

		glLightfv(GL_LIGHT1, GL_AMBIENT, focoA);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, focoD);
		glLightfv(GL_LIGHT1, GL_SPECULAR, focoS);
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20.0);
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 25.0);

		glLightfv(GL_LIGHT1, GL_POSITION, focoP);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, focoM);
		//glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.1);   //Atenuacuion para que la luz no ilumine el fondo

		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);
		//glPopMatrix();
		}
		else { glDisable(GL_LIGHT1); }
		
	}
	else {
		glDisable(GL_LIGHTING);
	}
	
}


void luces() {


	if (noche) {

		GLfloat lunaP[] = { 0.0, 10.0, 0.0, 0.0 };
		GLfloat lunaA[] = { 0.05, 0.05, 0.05, 1.0 };
		GLfloat lunaD[] = { 0.05, 0.05, 0.05, 1.0 };
		GLfloat lunaS[] = { 0.0, 0.0, 0.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, lunaP);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lunaA);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lunaD);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lunaS);
		glEnable(GL_LIGHT0);

		float inicioPeriodo = (int)Z - (int)Z % (int)periodo;

		for (int i = 2; i <= 5; i++) {
			GLenum GL_LIGHTi = GL_LIGHT0 + i;

			/*Posicion depende del circuito
			Ponemos las farolas equitativamente respecto la funcion
			*/
			float detras = 0;
				 // Casting para poder sacar bien el inico del periodo
			float distanciaFarolas = (periodo / 4.f) * ((float)i - 2.f);

			//if (i == 5) { printf("%.6f\n", distanciaFarolas );  }
			if (inicioPeriodo + distanciaFarolas < Z) { detras = 1; }			//Si detras de la camara, generamos en el siguiente periodo y no se genera detras
			GLfloat lP[] = { funcionCarretera(inicioPeriodo + distanciaFarolas + (periodo * detras)), 4.0, inicioPeriodo + distanciaFarolas + (periodo * detras), 1.0 };
			//Ambiental 0 por defecto
			GLfloat lD[] = { 0.5, 0.5, 0.2, 1.0 };
			GLfloat lM[] = { 0.0, -1.0, 0.0 };

			glLightfv(GL_LIGHTi, GL_POSITION, lP);
			glLightfv(GL_LIGHTi, GL_DIFFUSE, lD);
			glLightfv(GL_LIGHTi, GL_SPOT_DIRECTION, lM);
			glLightf(GL_LIGHTi, GL_SPOT_CUTOFF, 45.0);
			glLightf(GL_LIGHTi, GL_SPOT_EXPONENT, 10.0);
			glEnable(GL_LIGHTi);

		}

		glEnable(GL_LIGHTING);

	}
	else {

		glDisable(GL_LIGHT0);
		for (int i = 2; i <= 5; i++) {
			GLenum GL_LIGHTi = GL_LIGHT0 + i;
			glDisable(GL_LIGHTi);
		}
		glDisable(GL_LIGHTING);

	}
}


void ambiente() {
	glPushMatrix();
	if (niebla && !noche) {
		
		glEnable(GL_FOG);
		GLfloat nieblaD[] = { 0.6875, 0.8046875, 0.9765625, 1.0 };
		glFogfv(GL_FOG_COLOR, nieblaD);
		glFogf(GL_FOG_DENSITY, 0.25);
	}
	else if (niebla && noche) {
	
		glEnable(GL_FOG);
		GLfloat nieblaN[] = { 0.0, 0.0, 0.02, 1.0 };
		glFogfv(GL_FOG_COLOR, nieblaN);
		glFogf(GL_FOG_DENSITY, 0.15);
	
	}
	else { glDisable(GL_FOG);
	}
	glPopMatrix();
}

void generartexto() {
	if (mensajes) {
		GLfloat color [3];
	if (modoSimple)  std::copy(NEGRO, NEGRO + 4, color);
	else std::copy(BLANCO, BLANCO + 4, color);
	char separador[] = "-----------------";
	char ayuda[] = "Mensajes de ayuda";

	char* textNocheC;
	string textNoche("Modo Noche:");
	if (noche) textNoche.append(" ON");
	else textNoche.append(" OFF");
	textNocheC = &textNoche[0];

	char* textHudC;
	string textHud("HUD:");
	if (HUD) textHud.append(" ON");
	else textHud.append(" OFF");
	textHudC = &textHud[0];

	char* textNieblaC;
	string textNiebla("Niebla:");
	if (niebla) textNiebla.append(" ON");
	else textNiebla.append(" OFF");
	textNieblaC = &textNiebla[0];

	char* textVistaC;
	string textVista("Vista de Aguila:");
	if (!modoVista) textVista.append(" ON");
	else textVista.append(" OFF");
	textVistaC = &textVista[0];

	char* textAlambricoC;
	string textAlambrico("Modo Alambrico:");
	if (modoSimple) textAlambrico.append(" ON");
	else textAlambrico.append(" OFF");
	textAlambricoC = &textAlambrico[0];

	char* textMusicaC;
	string textMusica("Sonido:");
	if (sonido) textMusica.append(" ON");
	else textMusica.append(" OFF");
	textMusicaC = &textMusica[0];

	//char textAlambrico[] = "Alambrico: ON";

	texto(0, 5, ayuda, color, GLUT_BITMAP_9_BY_15, false);
	texto(0, 18, separador, color, GLUT_BITMAP_9_BY_15,false);
	texto(0, 26, textNocheC, color, GLUT_BITMAP_9_BY_15, false);
	texto(0, 40, textHudC, color, GLUT_BITMAP_9_BY_15, false);
	texto(0, 52, textNieblaC, color, GLUT_BITMAP_9_BY_15, false);
	texto(0, 64, textVistaC, color, GLUT_BITMAP_9_BY_15, false);
	texto(0, 76, textMusicaC, color, GLUT_BITMAP_9_BY_15, false);
	texto(0, 88, textAlambricoC, color, GLUT_BITMAP_9_BY_15, false);
	}
}
void mostrarHUD() {
	
	if(HUD){
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1,-1,100);	//(-1,-1) es el punto mas abajo a la izquierda del viewport, (1,1) el maximo a la derecha y (0,0) el centro
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	//Esfera
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textura[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTranslatef(-0.8, 0.8, 0.0);
	GLUquadricObj* test = gluNewQuadric();
	gluQuadricDrawStyle(test, GLU_FILL);
	gluQuadricTexture(test, TRUE);
	gluQuadricNormals(test, GLU_SMOOTH);
	glRotatef(anguloEstrella, 0, 1, 0);
	//glColor4f(0.0, 0.0, 0.0, 1.0);
	gluSphere(test, 0.2, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, textura[12]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	GLfloat v0[3] = { 0.2, 0.85, 0.0 };
	GLfloat v1[3] = { 1.0, 0.85, 0.0 };
	GLfloat v2[3] = { 1.0, 1.0, 0.0 };
	GLfloat v3[3] = { 0.2, 1.0, 0.0 };
	quadtex( v0, v1, v2, v3, 0, 1, 0, 1, 0, 0);
	glDisable(GL_BLEND);
	glPopMatrix();


	//Contador velocidad
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	// Habilitamos blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Z-Buffer Readonly
	glDepthMask(GL_FALSE);
	
	// Control Velocidad
	glPushMatrix();

	if (velocidad <= 10) glColor4f(0, 1, 0.3, 0.7);
	if (velocidad > 10 && velocidad < 20) glColor4f(1, 1, 0.0, 0.7);
	if (velocidad >= 20 ) glColor4f(1, 0.0, 0.0, 0.7);

	float aux = -1 + (velocidad / 30);
	if (aux > 0.8) aux = 0.8;
	glBegin(GL_POLYGON);
	glVertex2f(0.90, aux);
	glVertex2f(0.90, -1.00);
	glVertex2f(1.00, -1.00);
	glVertex2f(1.00, aux);

	glEnd();
	glPopMatrix();

	// Z-Buffer a estado normal
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	}
	

	
}
void activarMusica() {

	if(sonido) {
		sonido = 0;
		auxVolumen = cancionfondo->getVolume();  //Nos guardamos el volumen de la cancion ...
		cancionfondo->setVolume(0.0); 
		/*En vez de llamar a engine->stopAllSounds(); silenciamos la musica
		para que si la volvemos a activar, la cancion no vuelva a empezar desde el principio*/
	}
	else if (!sonido) {
		sonido = 1;
		cancionfondo->setVolume(auxVolumen);
		
	}

}

void cambiarMusica() {
	//Solo se cambia de canción si hay musica
	if (sonido) {
		engine->stopAllSounds();
		cancionfondo->drop();	//Libera memoria
		idcancionActual++;
		if (idcancionActual == totalcanciones) { idcancionActual = 0; }
			switch (idcancionActual) {
			case 0:
				cancionfondo = engine->play2D("./sonidos/Rainbow_Road_DS_Song_00.mp3", true, false, true);
				cancionfondo->setVolume(0.3);
				break;
			case 1:
				cancionfondo = engine->play2D("./sonidos/Kirby_Water_Song_Air_Ride_01.mp3", true, false, true);
				cancionfondo->setVolume(0.5);
				break;
			}
		
		}

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (noche && !modoSimple) glClearColor(0.0, 0.0, 0.0, 1.0); //Fondo negro
	else glClearColor(1.0, 1.0, 1.0, 1.0);		 //Fondo blanco



	luzfoco();									//Aqui para que sea adyacente a la camara

	if (modoVista) { gluLookAt(X, Y, Z, X + mirar * cos(angulo * PI / 180), Y, Z + mirar * sin(angulo * PI / 180), 0, modoVista, 1 - modoVista);}
	else gluLookAt(X, Y + 50, Z-30, X + mirar * cos(angulo * PI / 180), Y, Z + mirar * sin(angulo * PI / 180)+30 , 0, modoVista, 1 - modoVista);

	if (modoSimple){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_TEXTURE_2D);				 // esto asegura que se cargan las tecturas otra vez si se ponen modo simple y luego se pasa al texturado
		
	}

	if (dibujaEjes) {
		glPushMatrix();
		glScalef(8, 8, 8);
		ejes();
		glPopMatrix();
	}
	ambiente();
	anuncio(2.f); //El anuncio se genera a mitad de ampitud de onda
	circuito();
	fondo();
	circuito();
	planeta(1.f);
	panelflechas();
	
	detalles(1, nQuads / 3.2);
	estrella(1.f);
	detalles(nQuads / 2, nQuads);

	luces();
	
	mostrarHUD();
	generartexto();
	
	glutSwapBuffers();


}

void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float razon = (float)w / h;
	gluPerspective(45, razon, 1, 300);

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

	// Control de animaciones para que sean 
	// temporalmente coherente con la velocidad del procesador carga del sistema.
	anguloEstrella += velocidadEstrella * tiempo_transcurrido / 1000.0f;
	desplazamientoTextura += ratioAnimacion *tiempo_transcurrido / 1000.0f;
	movimientoPanel += amplitudAnimacion * tiempo_transcurrido / 1000.0f;

	stringstream titulo;
	titulo << fixed;
	titulo.precision(1); // De esta forma no aparecen errores de coma flotante en la ventana
	titulo << velocidad << "m/s";
	glutSetWindowTitle(titulo.str().c_str());
	glutSetWindowTitle(titulo.str().c_str());
	
	glutTimerFunc(1000 / tasaFPS, onTimer, tasaFPS);
	glutPostRedisplay();
}


void reset() {
	modoVista = 1;
	noche = 0;
	niebla = 0;
	HUD = 1;
	modoSimple = 0;
	mensajes = 0;
	cielo = 3;
	sonido = 1;
	idcancionActual = totalcanciones - 1;
	cambiarMusica();
}
void onKey(unsigned char tecla, int x, int y) {

	switch (tecla) {

	case 'T':
	case 't':
		if (mensajes) mensajes = 0;
		else mensajes = 1;
		break;
	case 'S':
	case 's':
		if (modoSimple) modoSimple = 0;
		else modoSimple = 1;
		break;
	case 'V':
	case 'v':
		if (modoVista)
			modoVista = 0;
		else modoVista = 1;
		break;
	case 'L':
	case 'l':
		if (noche)
			noche = 0;
		else noche = 1;
		break;
	case 'N':
	case 'n':
		if (niebla)
			niebla = 0;
		else niebla = 1;
		break;
	case 'M':
	case 'm':
		activarMusica();
		break;
	case 'K':
	case 'k':
		cambiarMusica();
		break;
	case 'C':
	case 'c':
		if (HUD)
			HUD = 0;
		else HUD = 1;
		break;
	case 'W':
	case 'w':
		switch (cielo) {
			case 6:  cielo = 3; break;
			default: cielo++;
		}
		break;
	case 'R':
	case 'r':
		reset();
		break;
	case 'E':
	case 'e':
		if (dibujaEjes)
			dibujaEjes = 0;
		else dibujaEjes = 1;
		break;
	case 'J':
	case 'j':
		saveScreenshot((char*)"captura_carretera_1.jpg", 900, 900);
		engine->play2D("./sonidos/sonido_camara.mp3");
		break;
	
	case 27:
		exit(0);

	}

	glutPostRedisplay();
}



void onSpecialKey(int specialKey, int x, int y) {
	if(modoVista){
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
	}
	glutPostRedisplay();
}



void main(int argc, char** argv) {
	//Inicializacion, buffer, tamaño ventana inicial, llamada a init, etc...
	FreeImage_Initialise();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(900, 900);
	glutInitWindowPosition(100, 100);

	glutCreateWindow(PROYECTO);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKey);
	glutSpecialFunc(onSpecialKey);
	//glutIdleFunc(onIdle);
	glutTimerFunc(1000 / tasaFPS, onTimer, tasaFPS);
	glutMainLoop();
	FreeImage_DeInitialise();
}


