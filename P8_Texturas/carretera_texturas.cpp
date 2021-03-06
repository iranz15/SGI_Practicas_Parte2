/*!
	\file		carretera_texturas.cpp
	\brief		Practica 8 Iluminacion
	\author		Jorge Iranzo' <jorirsan@upv.es>
	\date		2021-2022
 */


#define PROYECTO " - Jorge Iranzo"
 //#define _USE_MATH_DEFINES

#include <iostream>		
#include <sstream>
#include <cmath>
#include <ctime>
#include <freeglut.h>
#include <Utilidades.h>



static const int tasaFPS = 60;
static float coef[16];    // Matriz MODELVIEW

//Constantes para guardar la hora actual del sistema
static float tHora;
static float tMinuto;
static float tSegundos;


static float velocidadEsfera[] = { 100.0, 60.0 };

//Inicializacion de la posicion de la camara
static float X = 0;
static float Y = 1;
static float Z = 0;

//Variables de generacion de la carretera
static float amplitud = 8;
static float periodo = 70;
static int nQuads = 70;
static int ancho = 4;
static int distancia = 1;
static float threshold = 10;

//Variables de control de camara
static float ratioGiro = 5;
static float angulo = 90.0; //IMPORTANTE: Se genera la carretera hacia el eje +Z . Inicialmente desde 0.0.0 la tangente forma 90 grados con +X.
static float velocidad = 0.0;
static int mirar = 1;		//Constante para controlar la distancia entre la camara y el punto que esta mirando. Se ha escogido de forma arbitraria


//Flags de control
//Vista normal = 1/ Vista pajaro = 0
static int vista = 1;
static int noche = 0;
static int niebla = 0;
static int HUD = 1;
static int modoSimple = 0;
static int cielo = 3;

//Lista de variables de textura
GLuint textura[10];


/*		Vista de pajaro
		  Carretera
				^ Z+
			   (|
				|)
			   (|
X+    <---------|-------->     -X


*/



void texturas() {

	//Carretera
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textura[1]);
	glBindTexture(GL_TEXTURE_2D, textura[1]);
	loadImageFile((char*)"mario_kart_road.png");

	//Anuncio
	glGenTextures(1, &textura[2]);
	glBindTexture(GL_TEXTURE_2D, textura[2]);
	loadImageFile((char*)"anuncio_kirby.png");

	//Fondo 1
	glGenTextures(1, &textura[3]);
	glBindTexture(GL_TEXTURE_2D, textura[3]);
	loadImageFile((char*)"rainbow_road_fondo.jpg");

	//Fondo 2
	glGenTextures(1, &textura[4]);
	glBindTexture(GL_TEXTURE_2D, textura[4]);
	loadImageFile((char*)"agujero_fondo.jpg");

	glGenTextures(1, &textura[5]);
	glBindTexture(GL_TEXTURE_2D, textura[5]);
	loadImageFile((char*)"estrellas_fondo.png");


	glGenTextures(1, &textura[6]);
	glBindTexture(GL_TEXTURE_2D, textura[6]);
	loadImageFile((char*)"luna_fondo_2.jpg");
}


void init()
{
	//Inicializacion de metodos que cargan lista para luego poder llamarlas en display o otros metodos
	 // Fondo Blanco

	glEnable(GL_DEPTH_TEST);		//Test de profundidad
	glEnable(GL_NORMALIZE);			//Normalizacion vectores iluminacion
	texturas();						//Carga todas las texturas. NO se puede poner en el display
									//porque si no estaria cargando las texturas cada frame y va lentisimo
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
	float detras = 0;
	int l = ancho / 2;
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
	GLfloat a1[3] = { v0[0], 3 , v0[2] };
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
		glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, 50.0);
		glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, 25.0);
		glEnable(GL_LIGHT6);
	}

	else {
		glDisable(GL_LIGHT6);
	}

	quadtex(a3, a0, a1, a2, 0, 1, 0, 1, 10, 10);


	
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
	int l = ancho / 2;
	for (int i = 1; i <= nQuads; i++) {
		float detras = 0;
		
		float inicioPeriodo = ((int)Z - (int)Z % (int)periodo) ;
		float punto = inicioPeriodo + (i - 1) * distancia ;
		if (punto <= Z) { punto += periodo; }
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
		glColor3f(0, 0, 1);
		quadtex(cil0, cil1, cil2, cil3, 0.25+(i *alpha) / (2 * PI),0.25 + ((i + 1.f) * alpha) / (2 * PI), 0, 1,4,4);
		for (int j = 0; j < 3; j++) {
			cil0[j] = cil1[j];
			cil3[j] = cil2[j];
		}
	}
	glPopMatrix();
}

void luzfoco() {

	//Cheatsheet luces: https://i.gyazo.com/08d9da01d2b54086b40d41097cb25e75.png

	if (noche) {
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
	else {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHTING);
	}
}




void luces() {

	//Cheatsheet luces: https://i.gyazo.com/08d9da01d2b54086b40d41097cb25e75.png

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
		GLfloat nieblaA[] = { 0.6875, 0.8046875, 0.9765625, 1.0 };
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


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (noche && !modoSimple) glClearColor(0.0, 0.0, 0.0, 1.0); //Fondo negro
	else glClearColor(1.0, 1.0, 1.0, 1.0);		 //Fondo blanco

	if (vista) luzfoco();						 //Vista de pajaro = No se activa el foco

	if (vista) gluLookAt(X, Y, Z, X + mirar * cos(angulo * PI / 180), Y, Z + mirar * sin(angulo * PI / 180), 0, vista, 1 - vista);
	else gluLookAt(X, Y + 50, Z, X + mirar * cos(angulo * PI / 180), Y, Z + mirar * sin(angulo * PI / 180), 0, vista, 1 - vista);

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
	fondo();
	ejes();
	circuito();
	anuncio(2.f); 		//El anuncio se genera a mitad de ampitud de onda
	luces();
	glutSwapBuffers();


}


void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float razon = (float)w / h;
	gluPerspective(45, razon, 1, 200);
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
	glutSetWindowTitle(titulo.str().c_str());

	glutTimerFunc(1000 / tasaFPS, onTimer, tasaFPS);
	glutPostRedisplay();
}

void onKey(unsigned char tecla, int x, int y) {

	switch (tecla) {

	case 'S':
	case 's':
		if (modoSimple) modoSimple = 0;
		else modoSimple = 1;
		break;
	case 'V':
	case 'v':
		if (vista)
			vista = 0;
		else vista = 1;
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
	//Inicializacion, buffer, tama?o ventana inicial, llamada a init, etc...
	FreeImage_Initialise();
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
	FreeImage_DeInitialise();
}


