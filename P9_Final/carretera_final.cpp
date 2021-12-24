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
static float amplitud = 8;
static float periodo = 90;
static int nQuads = 90;
static int ancho = 4;
int l = ancho / 2;
static int distancia = 1;
static float threshold = 10;

//Variables de control de camara
static float ratioGiro = 5;
static float angulo = 90.0; //IMPORTANTE: Se genera la carretera hacia el eje +Z . Inicialmente desde 0.0.0 la tangente forma 90 grados con +X.
static float velocidad = 0.0;
static int mirar = 1;		//Constante para controlar la distancia entre la camara y el punto que esta mirando. Se ha escogido de forma arbitraria

//Angulo de giro estrella. Se declara para que sea dependiente de la velocidad de proceso del computador. Giro de 30 grados de base
static float anguloEstrella;
static float velocidadEstrella = 10.0;

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

//Lista de variables de textura
GLuint textura[12];

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

// *Esta implementacion de quadtex no tiene el calculo de normales. ASi si a estos quads se le aplica luz, se aplicara a los dos lados
// Se utiliza para los detalles de la carretera.
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
	//float norma = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);			//*
	//glNormal3f(normal[0] / norma, normal[1] / norma, normal[2] / norma);									//*
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
	std::cout << "C/c: Cambia la visibilidad de elementos solidarios a la c�mara -HUD-. El HUD contiene informacion acerca de \nla velocidad actual, el norte (eje -Z) y otros modos implementados \n";
	std::cout << "---------------------\n";
	std::cout << "MEJORAS/CONTROLES AVANZADOS:\n";
	std::cout << "T/t: Activa/desactiva mensajes de ayuda que indican que modos estan activos. ( Estos mensajes no consideramos que son parte del HUD,\n por lo que C/c no afecta a este opcion ) \n";
	std::cout << "V/v: Activa/desactiva una vista de aguila para ver el circuito desde arriba. Se desactiva el movimiento mediante flechas en este modo \n";
	std::cout << "W/w: Cambia la textura de fondo del cielo entre una seleccion de imagenes \n";
	std::cout << "M/m: Activa/desactiva la musica y los sonidos \n";
	std::cout << "K/k: Si estan activos los sonidos (M/m) cambia la cancion actual entre una seleccion de canciones \n";
	std::cout << "R/r: Reseta y pone los valores predeterminados de los modos descritos previamente (visual y sonoro) \n";

}

float funcionCarretera(float punto) {

	return amplitud * sin(punto * 2 * PI / periodo);

}


float normal(float d) {

	return  1 / (sqrtf(1 + powf(d, 2)));

}


float derivada(float punto) {

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
	float d = derivada(distanciaAnuncio);
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
		float d = derivada(punto);
		float n = normal(d);

		float x1 = funcionCarretera(siguiente);
		float d1 = derivada(siguiente);
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
	float d = derivada(distanciaAnuncio);
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
	else gluDisk(estrCirc, 2.0, 6, 10, 1);



	glDisable(GL_BLEND);
	glPopMatrix();

}
void detalles( int inicial,int final ) {
	glEnable(GL_BLEND);
	for (int i = inicial; i <= final; i++) {
		float detras = 0;

		float inicioPeriodo = ((int)Z - (int)Z % (int)periodo);
		float punto = inicioPeriodo + (i - 1) * distancia;
		if (punto < Z) { punto += periodo; }
		float siguiente = punto + distancia;

		float x = funcionCarretera(punto);
		float d = derivada(punto);
		float n = normal(d);

		float x1 = funcionCarretera(siguiente);
		float d1 = derivada(siguiente);
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
	//Externo
	glPushMatrix();
	glDepthMask(GL_FALSE);
	if (noche) quadtexAlter(v3, v2, v2YBorde, v3YBorde, 0, 1, 0, 1, 7, 7);
		else quadtexAlter(  v3, v2, v2YBorde, v3YBorde, 0, 1, 0, 1, 1, 1);
	glDepthMask(GL_TRUE);
	glPopMatrix();

	//Interno
	glPushMatrix();
	glDepthMask(GL_FALSE);
	if (noche) quadtexAlter(v0, v1, v1YBorde, v0YBorde, 0, 1, 0, 1, 7, 7);
	else quadtexAlter(v0, v1, v1YBorde, v0YBorde, 0, 1, 0, 1, 1, 1);
	glDepthMask(GL_TRUE);
	glPopMatrix();

	
	
	}
	glDisable(GL_BLEND);
}

void panelflechas(float distanciaFlechas) {
	
	
		float detras = 0;

		float inicioPeriodo = ((int)Z - (int)Z % (int)periodo);
		float punto = inicioPeriodo + ((nQuads/2) - 5) * distancia;
		if (punto < Z) { punto += periodo; }
		float siguiente = punto + distancia;

		float x = funcionCarretera(punto);
		float d = derivada(punto);
		float n = normal(d);

		float x1 = funcionCarretera(siguiente);
		float d1 = derivada(siguiente);
		float n1 = normal(d1);

		GLfloat v3[3] = { x + (n * l) , 0.0 , punto + (-1 * d * n * l) };

		GLfloat v2[3] = { x1 + (n1 * l) , 0.0 , siguiente + (-1 * d1 * n1 * l) };

		float altura = 0.5f;
		GLfloat v3YBorde[3] = { v3[0] , altura , v3[2] };
		GLfloat v2YBorde[3] = { v2[0] , altura , v2[2] };

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, textura[11]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if (noche) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPushMatrix();
	glDepthMask(GL_FALSE);
	quadtex(v3, v2, v2YBorde, v3YBorde, 0, 1, 0, 1, 1, 1);
	glDepthMask(GL_TRUE);
	glPopMatrix();
	glDisable(GL_BLEND);
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
	float distanciaAnuncio =  (periodo / 4.f) -1;
	if (inicioPeriodo + distanciaAnuncio < Z) { detras = 1; }
	distanciaAnuncio += inicioPeriodo + (periodo * detras);

	float x = funcionCarretera(distanciaAnuncio);

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
	glTranslatef(x-10, -2, distanciaAnuncio );
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
	char separador[] = "-----------------";
	char ayuda[] = "Mensajes de ayuda";
	char hud[] = "HUD: ON";
	char textNoche[] = "Modo Noche: ON";
	char textNiebla[] = "Niebla: ON";
	char textAguila[] = "Aguila: ON";
	char textAlambrico[] = "Alambrico: ON";
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	texto(0, 3, ayuda, ROJO, GLUT_BITMAP_9_BY_15, false);
	texto(0, 18, separador, ROJO, GLUT_BITMAP_9_BY_15,false);
	if(HUD) texto(0, 48, hud, ROJO, GLUT_BITMAP_9_BY_15, false);
	}
}
void mostrarHUD() {
	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	if(HUD){
	
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);	//(-1,-1) es el punto mas abajo a la izquierda del viewport, (1,1) el maximo a la derecha y (0,0) el centro
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	
	// Habilitamos blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Z-Buffer Readonly
	glDepthMask(GL_FALSE);
	
	// Control Velocidad
	glPushMatrix();
	glColor4f(1, 0, 0.0, 0.7);
	glBegin(GL_POLYGON);
	glVertex2f(0.90, -0);
	glVertex2f(0.90, -1.00);
	glVertex2f(1.00, -1.00);
	glVertex2f(1.00, -0);

	glEnd();
	glPopMatrix();

	// Z-Buffer a estado normal
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	}
		glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glPopAttrib();
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
	//Solo se cambia de canci�n si hay musica
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

	
	ambiente();
	anuncio(2.f); //El anuncio se genera a mitad de ampitud de onda
	circuito();
	fondo();
	ejes();
	circuito();
	planeta(1.f);
	
	
	detalles(1, nQuads / 2.5);
	estrella(1.f);
	detalles(nQuads / 2, nQuads);
	panelflechas(2.f);
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
	anguloEstrella += velocidadEstrella * tiempo_transcurrido / 1000.0f;

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
	//Inicializacion, buffer, tama�o ventana inicial, llamada a init, etc...
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


