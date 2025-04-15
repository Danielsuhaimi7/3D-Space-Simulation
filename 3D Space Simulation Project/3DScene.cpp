#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
// Define GL_BGR if it's not defined
#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

// Mars variables
GLfloat marsScaleX = 1.2f;
GLfloat marsScaleY = 1.5f;
GLfloat marsScaleZ = 1.2f;
GLfloat marsRotationX = 0.0f;
GLfloat marsRotationY = 0.0f;
GLfloat marsRotationZ = 0.0f;
GLfloat marsRotationSpeed = 0.08f;

// Saturn variables
GLfloat saturnPositionX = 10.0f;
GLfloat saturnPositionY = 16.0f;
GLfloat saturnPositionZ = 10.0f;
GLfloat saturnRotationX = 5.0f;
GLfloat saturnRotationY = 0.0f;
GLfloat saturnRotationZ = 0.0f;
GLfloat saturnSpeedY = 0.1f;

// Asteroid variables
GLfloat asteroid1PositionX = 1.5f;
GLfloat asteroid1PositionY = 1.0f;
GLfloat asteroid1PositionZ = -10.5f;
GLfloat asteroid1Speed = 0.006f;

GLfloat asteroid2PositionX = 1.0f;
GLfloat asteroid2PositionY = 1.5f;
GLfloat asteroid2PositionZ = -11.5f;
GLfloat asteroid2Speed = 0.004f;

// Satellite transformation parameters
float satelliteRotationX = 0.0f;
float satelliteRotationY = 0.0f;
float satelliteRotationZ = 0.0f;
float satelliteTranslationX = -1.0f;
float satelliteTranslationY = 1.8f;
float satelliteTranslationZ = -10.0f;
float satelliteScaleX = 0.6f;
float satelliteScaleY = 0.6f;
float satelliteScaleZ = 0.6f;
float prevSatMouseX, prevSatMouseY;

bool isSatDragging = false;
bool enableSatAnimation = true;

// Global variables for spaceship
float spaceshipPositionX = -8.0f;
float spaceshipPositionY = -1.0f;
float spaceshipPositionZ = -4.0f;
float spaceshipRotationAngle = -70.0f;
float spaceshipRotationSpeed = 0.1f;

// General rotation angles
GLfloat angleX = 0.0f;
GLfloat angleY = 0.0f;
GLfloat angleZ = 0.0f;

// General scaling factors
GLfloat scaleX = 0.9f;
GLfloat scaleY = 1.0f;
GLfloat scaleZ = 0.9f;

// Camera position
GLfloat camX = 0.0f;
GLfloat camY = 0.0f;
GLfloat camZ = 10.0f;

// Wireframe mode
bool wireframeMode = false;

// Texture IDs
GLuint marsTexture, saturnRingTexture, saturnSphereTexture;
GLuint galaxyTexture;
GLuint asteroidTexture;
GLuint shimmeringSatellite;
GLuint planeTexture, missileTexture, wingTexture;
GLuint holeringTexture, metalBody;

// Other variables
static float translateZ = 0;
static bool launch1 = false, launch2 = false;
static int first = 0, second = 0;

// Mouse tracking variables
int lastMouseX, lastMouseY;
bool mouseDown = false;


// Starship and Black hole variables
float turretAngle = 0.0f;
float ringAngle = 0.0f;
bool rotateLeft = true;


/////////// LOAD BMP ///////////
int loadBMP(const char* filename, GLuint &texture) {
    FILE* file;
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int width, height;
    unsigned int imageSize;
    unsigned char* data;

    file = fopen(filename, "rb");
    if (!file) {
        printf("Image could not be opened\n");
        return 0;
    }

    if (fread(header, 1, 54, file) != 54) {
        printf("Not a correct BMP file\n");
        return 0;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a correct BMP file\n");
        return 0;
    }

    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);

    if (imageSize == 0) imageSize = width * height * 3;
    if (dataPos == 0) dataPos = 54;

    data = (unsigned char*)malloc(imageSize);
    fread(data, 1, imageSize, file);
    fclose(file);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(data);

    return 1;
}
/////////// LOAD BMP ///////////


/////////// INITIALIZATION ///////////
void init() {
    // Set up perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    // Enable lighting and a light source
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightPosition[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat lightAmbient[] = { 1.0, 0.8, 0.8, 1.0 };
    GLfloat lightDiffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Initialize the render and load textures
    glClearColor(0, 0, 0, 1.0);  // Set clear color to black
    glEnable(GL_DEPTH_TEST);     // Enable depth testing
    glEnable(GL_TEXTURE_2D);     // Enable texture mapping

    glMatrixMode(GL_PROJECTION); // Switch to projection matrix
    glLoadIdentity();            // Reset projection matrix
    gluPerspective(45.0, 1.0, 1.0, 100.0); // Set perspective projection
    glMatrixMode(GL_MODELVIEW);  // Switch back to model view matrix

    // Load texture
    loadBMP("mars.bmp", marsTexture);
    loadBMP("galaxy.bmp", galaxyTexture);
    loadBMP("asteroid.bmp", asteroidTexture);
    loadBMP("shimmeringSatellite.bmp", shimmeringSatellite);
    loadBMP("planetexture.bmp", planeTexture);
    loadBMP("missile.bmp", missileTexture);
    loadBMP("wingtexture.bmp", wingTexture);
    loadBMP("holeringtexture.bmp", holeringTexture);
    loadBMP("metalbody.bmp", metalBody);
    loadBMP("saturn.bmp", saturnSphereTexture);
    loadBMP("ring.bmp", saturnRingTexture);


    // Fog parameters
    GLfloat fogColor[] = {0.5, 0.5, 0.5, 1.0}; // Light grey color
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.04f); // Density of the fog
    glHint(GL_FOG_HINT, GL_NICEST); // Bucest fog calculation
}
/////////// INITIALIZATION ///////////


/////////// Draw Mars ///////////
void drawMars() {
    glPushMatrix();     // Save the current transformation matrix

    // Apply translation
    glTranslatef(-1.7f, -0.8f, -8.3f);              // Translate Mars to its position

    // Apply rotation
    glRotatef(marsRotationX, 1.0f, 0.0f, 0.0f);     // Rotate around x-axis
    glRotatef(marsRotationY, 0.0f, 1.0f, 0.0f);     // Rotate around y-axis
    glRotatef(marsRotationZ, 0.0f, 0.0f, 1.0f);     // Rotate around z-axis

    // Apply scale
    glScalef(marsScaleX, marsScaleY, marsScaleZ);

    GLUquadric *quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, marsTexture);
    gluSphere(quad, 1.0, 40, 40);
    gluDeleteQuadric(quad);

    glPopMatrix();
}
/////////// Draw Mars ///////////



/////////// Draw Mars Animation ///////////
void updateMarsAnimation() {
    marsRotationY += marsRotationSpeed;
    if (marsRotationY > 360.0f) {
        marsRotationY -= 360.0f;    // keep the angle within 0-360 degrees
    }
}
/////////// Draw Mars Animation ///////////



/////////// Draw Sphere Saturn ///////////
void drawSphereSaturn() {

    glPushMatrix();     // Save the current transformation matrix
    // Apply scale
    glScalef(1.0f, 1.3f, 1.0f);

    GLUquadric *quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, saturnSphereTexture);
    gluSphere(quad, 1.0, 40, 40);
    gluDeleteQuadric(quad);

    glPopMatrix();  // Restore the previous transformation matrix
}
/////////// Draw Sphere Saturn ///////////


/////////// Draw Ring Saturn ///////////
void drawRingSaturn() {

    // Apply scale to the ring
    glScalef(1.0f, 1.3f, 1.0f);

    int numSegments = 100;
    GLfloat ringInnerRadius = 1.4f;
    GLfloat ringOuterRadius = 2.0f;
    GLfloat angleStep = 2.0f * M_PI / numSegments; // Use M_PI for better precision

    glBindTexture(GL_TEXTURE_2D, saturnRingTexture);  // Bind the texture for the ring

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSegments; ++i) {
        GLfloat angle = i * angleStep;
        GLfloat x = cos(angle);
        GLfloat y = sin(angle);

        glTexCoord2f(0.5f * (1.0f + x), 0.5f * (1.0f + y)); glVertex3f(ringInnerRadius * x, 0.0f, ringInnerRadius * y);
        glTexCoord2f(0.5f * (1.0f + x), 0.5f * (1.0f + y)); glVertex3f(ringOuterRadius * x, 0.0f, ringOuterRadius * y);
    }
    glEnd();
}

/////////// Draw Ring Saturn ///////////


/////////// Draw Saturn ///////////
void drawSaturn() {
    glPushMatrix();  // Save the current matrix state

    // Apply translation once
    glTranslatef(1.0f, 5.0f, -15.0f);

    // Apply rotation around Y-axis
    glRotatef(saturnRotationY, 0.0f, 1.0f, 0.0f);  // Rotate Saturn around the Y-axis

    drawSphereSaturn();  // Draw Saturn's sphere
    drawRingSaturn();    // Draw Saturn's ring

    glPopMatrix();  // Restore the previous matrix state
}
/////////// Draw Saturn ///////////


/////////// Update Animation ///////////
void updateSaturnAnimation() {
    saturnRotationY += saturnSpeedY;  // Adjust rotation speed as needed

    // Ensure rotation stays within 0-360 degrees
    if (saturnRotationY > 360.0f) saturnRotationY -= 360.0f;
}
/////////// Update Animation ///////////


/////////// Draw Asteroid1 ///////////
void drawAsteroid1() {
    glPushMatrix();

    // Position the asteroid
    glTranslatef(asteroid1PositionX, asteroid1PositionY, asteroid1PositionZ);
    // Scale the asteroid
    glScalef(0.5f, 1.0f, 2.0f);

    glBindTexture(GL_TEXTURE_2D, asteroidTexture);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, 0.3, 20, 20);   // Adjust size to be proportional to Mars
    gluDeleteQuadric(quad);

    glPopMatrix();
}
/////////// Draw Asteroid2 ///////////



/////////// Asteroid1 Animation ///////////
void updateAsteroid1Animation() {
    asteroid1PositionZ += asteroid1Speed;  // Move towards the camera
}
/////////// Asteroid1 Animation ///////////



/////////// Draw Asteroid2 ///////////
void drawAsteroid2() {
    glPushMatrix();

    // Position the asteroid
    glTranslatef(asteroid2PositionX, asteroid2PositionY, asteroid2PositionZ);
    // Scale the asteroid
    glScalef(0.5f, 0.5f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, asteroidTexture);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, 0.3, 20, 20);   // Adjust size to be proportional to Mars
    gluDeleteQuadric(quad);

    glPopMatrix();
}
/////////// Draw Asteroid2 ///////////



/////////// Asteroid2 Animation ///////////
void updateAsteroid2Animation() {
    asteroid2PositionZ += asteroid2Speed;  // Move towards the camera
}
/////////// Asteroid2 Animation ///////////



/////////// Draw Solar Panels ///////////
void drawSolarPanels() {
    glColor3f(0.75, 0.75, 0.75); // Silver color
    glBindTexture(GL_TEXTURE_2D, shimmeringSatellite);

    // Right solar panel
    glPushMatrix();
    glTranslatef(1.5, 0.0, 0.0);
    glScalef(3.0, 0.1, 1.0);

    if (wireframeMode) {
        glutWireCube(1.0);
    } else {
        glBegin(GL_QUADS);

        // Front face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5,  0.5);
        glTexCoord2f(3.0, 0.0); glVertex3f( 0.5, -0.5,  0.5);
        glTexCoord2f(3.0, 3.0); glVertex3f( 0.5,  0.5,  0.5);
        glTexCoord2f(0.0, 3.0); glVertex3f(-0.5,  0.5,  0.5);

        // Back face
        glTexCoord2f(3.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(3.0, 3.0); glVertex3f(-0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 3.0); glVertex3f( 0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f( 0.5, -0.5, -0.5);

        // Top face
        glTexCoord2f(0.0, 3.0); glVertex3f(-0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5,  0.5,  0.5);
        glTexCoord2f(3.0, 0.0); glVertex3f( 0.5,  0.5,  0.5);
        glTexCoord2f(3.0, 3.0); glVertex3f( 0.5,  0.5, -0.5);

        // Bottom face
        glTexCoord2f(3.0, 3.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(0.0, 3.0); glVertex3f( 0.5, -0.5, -0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f( 0.5, -0.5,  0.5);
        glTexCoord2f(3.0, 0.0); glVertex3f(-0.5, -0.5,  0.5);

        // Right face
        glTexCoord2f(3.0, 0.0); glVertex3f( 0.5, -0.5, -0.5);
        glTexCoord2f(3.0, 3.0); glVertex3f( 0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 3.0); glVertex3f( 0.5,  0.5,  0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f( 0.5, -0.5,  0.5);

        // Left face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(3.0, 0.0); glVertex3f(-0.5, -0.5,  0.5);
        glTexCoord2f(3.0, 3.0); glVertex3f(-0.5,  0.5,  0.5);
        glTexCoord2f(0.0, 3.0); glVertex3f(-0.5,  0.5, -0.5);

        glEnd();
    }

    glPopMatrix();

    // Left solar panel
    glPushMatrix();
    glTranslatef(-1.5, 0.0, 0.0);
    glScalef(3.0, 0.1, 1.0);

    if (wireframeMode) {
        glutWireCube(1.0);
    } else {
        glBegin(GL_QUADS);

        // Front face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5,  0.5);
        glTexCoord2f(3.0, 0.0); glVertex3f( 0.5, -0.5,  0.5);
        glTexCoord2f(3.0, 3.0); glVertex3f( 0.5,  0.5,  0.5);
        glTexCoord2f(0.0, 3.0); glVertex3f(-0.5,  0.5,  0.5);

        // Back face
        glTexCoord2f(3.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(3.0, 3.0); glVertex3f(-0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 3.0); glVertex3f( 0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f( 0.5, -0.5, -0.5);

        // Top face
        glTexCoord2f(0.0, 3.0); glVertex3f(-0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5,  0.5,  0.5);
        glTexCoord2f(3.0, 0.0); glVertex3f( 0.5,  0.5,  0.5);
        glTexCoord2f(3.0, 3.0); glVertex3f( 0.5,  0.5, -0.5);

        // Bottom face
        glTexCoord2f(3.0, 3.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(0.0, 3.0); glVertex3f( 0.5, -0.5, -0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f( 0.5, -0.5,  0.5);
        glTexCoord2f(3.0, 0.0); glVertex3f(-0.5, -0.5,  0.5);

        // Right face
        glTexCoord2f(3.0, 0.0); glVertex3f( 0.5, -0.5, -0.5);
        glTexCoord2f(3.0, 3.0); glVertex3f( 0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 3.0); glVertex3f( 0.5,  0.5,  0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f( 0.5, -0.5,  0.5);

        // Left face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(3.0, 0.0); glVertex3f(-0.5, -0.5,  0.5);
        glTexCoord2f(3.0, 3.0); glVertex3f(-0.5,  0.5,  0.5);
        glTexCoord2f(0.0, 3.0); glVertex3f(-0.5,  0.5, -0.5);

        glEnd();
    }

    glPopMatrix();
}
/////////// Draw Solar Panels ///////////


/////////// Draw Antennas ///////////
void drawAntennas() {
    glColor3f(1.0, 1.0, 1.0); // White color

    // Antenna 1
    glPushMatrix();
    glTranslatef(0.0, 0.5, 0.0);
    glRotatef(45, 1.0, 0.0, 0.0);
    glScalef(0.1, 1.0, 0.1);

    if (wireframeMode) {
        glutWireCube(1.0);
    } else {
        glBegin(GL_QUADS);

        // Front face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, 0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);

        // Back face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, -0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, -0.5);

        // Left face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, -0.5);

        // Right face
        glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, 0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(0.5, 0.5, -0.5);

        // Top face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, 0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, 0.5, -0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);

        // Bottom face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, -0.5, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, -0.5, 0.5);

        glEnd();
    }

    glPopMatrix();

    // Antenna 2
    glPushMatrix();
    glTranslatef(0.0, -0.5, 0.0);
    glRotatef(-45, 1.0, 0.0, 0.0);
    glScalef(0.1, 1.0, 0.1);

    if (wireframeMode) {
        glutWireCube(1.0);
    } else {
         glBegin(GL_QUADS);

        // Front face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, 0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);

        // Back face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, -0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, -0.5);

        // Left face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, -0.5);

        // Right face
        glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, 0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(0.5, 0.5, -0.5);

        // Top face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, 0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, 0.5, -0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);

        // Bottom face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, -0.5, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, -0.5, 0.5);

        glEnd();
    }

    glPopMatrix();
}
/////////// Draw Antennas ///////////


/////////// Draw SatBody ///////////
void drawSatelliteBody() {
    glColor3f(0.5, 0.5, 0.5); // Grey color

    // Body of the satellite (cube)
    glPushMatrix();
    glScalef(1.0, 0.5, 0.5);

    if (wireframeMode) {
        glutWireCube(1.0);
    } else {
        glBegin(GL_QUADS);

        // Front face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5,  0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f( 0.5, -0.5,  0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f( 0.5,  0.5,  0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5,  0.5,  0.5);

        // Back face
        glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(-0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f( 0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f( 0.5, -0.5, -0.5);

        // Top face
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5,  0.5,  0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f( 0.5,  0.5,  0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f( 0.5,  0.5, -0.5);

        // Bottom face
        glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f( 0.5, -0.5, -0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f( 0.5, -0.5,  0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5,  0.5);

        // Right face
        glTexCoord2f(1.0, 0.0); glVertex3f( 0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f( 0.5,  0.5, -0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f( 0.5,  0.5,  0.5);
        glTexCoord2f(0.0, 0.0); glVertex3f( 0.5, -0.5,  0.5);

        // Left face
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5,  0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(-0.5,  0.5,  0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5,  0.5, -0.5);

        glEnd();
    }

    glPopMatrix();
}
/////////// Draw SatBody ///////////


/////////// Draw Sat ///////////
void drawSatellite() {

    // Define transformation variables
    float scaleX = 0.8f, scaleY = 0.8f, scaleZ = 0.8f;

    glPushMatrix();
    glTranslatef(satelliteTranslationX, satelliteTranslationY, satelliteTranslationZ);

    // Apply rotation for animation
    glRotatef(satelliteRotationX, 1.0f, 0.0f, 0.0f); // Rotate around X-axis
    glRotatef(satelliteRotationY, 0.0f, 2.0f, 0.6f); // Rotate around Y-axis
    glRotatef(satelliteRotationZ, 0.0f, 0.0f, 1.0f); // Rotate around Z-axis

    // Apply scaling
    glScalef(satelliteScaleX, satelliteScaleY, satelliteScaleZ);

    // Draw the components of the satellite
    drawSatelliteBody();
    drawSolarPanels();
    drawAntennas();

    glPopMatrix();
}
/////////// Draw Sat ///////////


/////////// Draw Wing ///////////
void wing() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wingTexture);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glTexCoord2f(0.0, 0.0); glVertex3f(0, 4, 0);
    glTexCoord2f(1.0, 0.0); glVertex3f(7, 1, 12);
    glTexCoord2f(1.0, 1.0); glVertex3f(9, 1, 12);
    glTexCoord2f(0.0, 1.0); glVertex3f(8, 4, 0);

    glTexCoord2f(0.0, 0.0); glVertex3f(0, 0, 0);
    glTexCoord2f(1.0, 0.0); glVertex3f(7, 0, 12);
    glTexCoord2f(1.0, 1.0); glVertex3f(9, 0, 12);
    glTexCoord2f(0.0, 1.0); glVertex3f(8, 0, 0);

    glTexCoord2f(0.0, 0.0); glVertex3f(0, 4, 0);
    glTexCoord2f(1.0, 0.0); glVertex3f(0, 0, 0);
    glTexCoord2f(1.0, 1.0); glVertex3f(7, 0, 12);
    glTexCoord2f(0.0, 1.0); glVertex3f(7, 1, 12);

    glTexCoord2f(0.0, 0.0); glVertex3f(7, 1, 12);
    glTexCoord2f(1.0, 0.0); glVertex3f(7, 0, 12);
    glTexCoord2f(1.0, 1.0); glVertex3f(9, 0, 12);
    glTexCoord2f(0.0, 1.0); glVertex3f(9, 1, 12);

    glTexCoord2f(0.0, 0.0); glVertex3f(8, 0, 0);
    glTexCoord2f(1.0, 0.0); glVertex3f(8, 4, 0);
    glTexCoord2f(1.0, 1.0); glVertex3f(9, 1, 12);
    glTexCoord2f(0.0, 1.0); glVertex3f(9, 0, 12);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
/////////// Draw Wing ///////////


/////////// Draw Left and Right Wing ///////////
void LeftNRightWing() {
    glPushMatrix();
    glTranslatef(0, -0.8, 0);
    glRotatef(-90, 0, 1, 0);
    glScalef(0.5, 0.4, 1);
    wing();

    glTranslatef(10, 0, 0);
    glScalef(0.8, 1, 0.5);
    wing();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -0.8, 0);
    glRotatef(-90, 0, 1, 0);
    glScalef(0.5, 0.4, -1);
    wing();

    glTranslatef(10, 0, 0);
    glScalef(0.8, 1, 0.5);
    wing();
    glPopMatrix();
}
/////////// Draw Left and Right Wing ///////////


/////////// Draw Top Wing ///////////
void TopWing() {
    glPushMatrix();
    glTranslatef(0.6, 0.5, 7.5);
    glRotatef(-90, 0, 1, 0);
    glScalef(0.3, 0.3, 0.3);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wingTexture);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glTexCoord2f(0.0, 0.0); glVertex3f(2, 7, 2);
    glTexCoord2f(1.0, 0.0); glVertex3f(0, 0, 4);
    glTexCoord2f(1.0, 1.0); glVertex3f(8, 0, 4);
    glTexCoord2f(0.0, 1.0); glVertex3f(6, 7, 2);

    glTexCoord2f(0.0, 0.0); glVertex3f(2, 7, 2);
    glTexCoord2f(1.0, 0.0); glVertex3f(0, 0, 0);
    glTexCoord2f(1.0, 1.0); glVertex3f(8, 0, 0);
    glTexCoord2f(0.0, 1.0); glVertex3f(6, 7, 2);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(1, 1, 1);
    glTexCoord2f(0.0, 0.0); glVertex3f(2, 7, 2);
    glTexCoord2f(1.0, 0.0); glVertex3f(0, 0, 0);
    glTexCoord2f(1.0, 1.0); glVertex3f(0, 0, 4);

    glTexCoord2f(0.0, 0.0); glVertex3f(6, 7, 2);
    glTexCoord2f(1.0, 0.0); glVertex3f(8, 0, 0);
    glTexCoord2f(1.0, 1.0); glVertex3f(8, 0, 4);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
/////////// Draw Top Wing ///////////

/////////// Draw Plane Bpdy ///////////
void PlaneBody() {
    GLUquadricObj* pObj;
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, planeTexture);
    gluQuadricTexture(pObj, GL_TRUE);

    glPushMatrix();
    glColor3f(1, 1, 1);
    gluCylinder(pObj, 1.0f, 1.0f, 10.0f, 26, 13);
    glTranslatef(0, 0, 10);
    gluDisk(pObj, 0, 1.0, 26, 13);
    glColor3f(0.5, 0.5, 0.5);
    gluCylinder(pObj, 0.5f, 0.5f, 0.5f, 26, 13);
    glColor3f(1, 0, 0);
    glTranslatef(0, 0, 0.5);
    gluDisk(pObj, 0, 0.5, 26, 13);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);
    glScalef(1, 1, 3);
    gluSphere(pObj, 1.0f, 26, 26);
    glPopMatrix();

    gluDeleteQuadric(pObj);
    glDisable(GL_TEXTURE_2D);
}
/////////// Draw Plane Bpdy ///////////


/////////// Draw Missiles ///////////
void missiles() {
    GLUquadricObj* pObj;
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
    gluQuadricTexture(pObj, GL_TRUE);  // Enable texture for the quadric

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, missileTexture);  // Bind the missile texture

    glPushMatrix();
    glColor3f(1, 1, 1);

    // Draw the missile nose
    glColor3f(1, 0, 0);
    gluCylinder(pObj, 0.0, 0.5, 2.0, 26, 13); // Nose cone

    glTranslatef(0, 0, 2);
    glColor3f(1, 1, 1);
    gluCylinder(pObj, 0.5, 0.5, 5.0, 26, 13); // Main body of the missile

    glTranslatef(0, 0, 5);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture to avoid affecting other objects
    glColor3f(1, 1, 1);
    gluDisk(pObj, 0, 0.5, 26, 13); // Back of the missile

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(pObj);
}
/////////// Draw Missiles ///////////


/////////// Draw First Set Of Missiles ///////////
void FirstSetOfMissiles() {
    glPushMatrix();
    glTranslatef(-8, -1.2, 0.8);
    glScalef(0.5, 0.5, 0.5);
    missiles();

    glTranslatef(32, 0, 0);
    missiles();
    glPopMatrix();
}
/////////// Draw First Set Of Missiles ///////////


/////////// Draw Second Set Of Missiles ///////////
void SecondSetOfMissiles() {
    glPushMatrix();
    glTranslatef(-6, -1.2, 0);
    glScalef(0.5, 0.5, 0.5);
    missiles();

    glTranslatef(24, 0, 0);
    missiles();
    glPopMatrix();
}
/////////// Draw Second Set Of Missiles ///////////


/////////// Draw Plane ///////////
void drawPlane() {
    GLfloat planeTranslateX = 3.0f;
    GLfloat planeTranslateY = 2.0f;
    GLfloat planeTranslateZ = -10.0f;

    GLfloat planeRotateAngle = 110.0f; // Change the angle to 90 degrees to look left
    GLfloat planeRotateX = -0.2f;
    GLfloat planeRotateY = 1.0f; // Rotate around the Y-axis
    GLfloat planeRotateZ = 0.0f;

    GLfloat planeScaleX = 0.2f;
    GLfloat planeScaleY = 0.2f;
    GLfloat planeScaleZ = 0.2f;

    glPushMatrix(); // Save the current transformation matrix

    // Apply translation
    glTranslatef(planeTranslateX, planeTranslateY, planeTranslateZ);
    // Apply rotation
    glRotatef(planeRotateAngle, planeRotateX, planeRotateY, planeRotateZ);
    // Apply scaling
    glScalef(planeScaleX, planeScaleY, planeScaleZ);

    // Draw plane components
    PlaneBody();
    LeftNRightWing();
    TopWing();

    // Draw missiles
    if (launch1 && translateZ >= -50) {  // Ensure translateZ limit is the same as in the idle function
        glPushMatrix();
        glTranslatef(0, 0, translateZ);
        if (first == 1)
            FirstSetOfMissiles();
        if (second == 1)
            SecondSetOfMissiles();
        glPopMatrix();
    }

    if (first == 0 && second == 0) {
        FirstSetOfMissiles();
        SecondSetOfMissiles();
    }
    else if (first == 1 && second == 0) {
        SecondSetOfMissiles();
    }
    else if (first == 0 && second == 1) {
        FirstSetOfMissiles();
    }

    glEnable(GL_TEXTURE_2D); // Ensure textures are enabled

    glPopMatrix(); // Restore the previous transformation matrix
}
/////////// Draw Plane ///////////


/////////// Draw Background ///////////
void drawStarsBackground() {
    // Enable texturing and bind the skybox texture
    glBindTexture(GL_TEXTURE_2D, galaxyTexture);

    // Draw a cube with each face mapped to the skybox texture
    glBegin(GL_QUADS);

    // Front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, -50.0f, -50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(50.0f, -50.0f, -50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, 50.0f, -50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-50.0f, 50.0f, -50.0f);

    // Back face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(50.0f, -50.0f, 50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-50.0f, -50.0f, 50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-50.0f, 50.0f, 50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(50.0f, 50.0f, 50.0f);

    // Left face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, -50.0f, 50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-50.0f, -50.0f, -50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-50.0f, 50.0f, -50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-50.0f, 50.0f, 50.0f);

    // Right face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(50.0f, -50.0f, -50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(50.0f, -50.0f, 50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, 50.0f, 50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(50.0f, 50.0f, -50.0f);

    // Top face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, 50.0f, -50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(50.0f, 50.0f, -50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, 50.0f, 50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-50.0f, 50.0f, 50.0f);

    // Bottom face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, -50.0f, 50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(50.0f, -50.0f, 50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, -50.0f, -50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-50.0f, -50.0f, -50.0f);

    glEnd();
}
/////////// Draw Background ///////////




/////////// Draw Black Hole ///////////
void drawBlackHole(GLuint eventHorizonTexture, GLuint diskTexture1, GLuint diskTexture2) {

    // Event Horizon
    GLUquadricObj *quadObj;
    quadObj = gluNewQuadric();
    gluQuadricTexture(quadObj, GL_TRUE); // Enable texture coordinates
    gluQuadricNormals(quadObj, GLU_SMOOTH);

    // Accretion Disk parameters
    const int numSegments = 100;
    const float innerRadius = 4.0f;
    const float outerRadius = 12.0f;
    const float diskThickness = 0.2f;

    // Colors for the gradient (from inner to outer)
    float innerColor[3] = {1.0f, 0.5f, 0.0f}; // Orange
    float outerColor[3] = {1.0f, 0.0f, 0.0f}; // Red

    // Draw Event Horizon
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, eventHorizonTexture); // Bind event horizon texture
    glColor3f(1.0f, 1.0f, 1.0f); // White color to show the texture fully
    gluSphere(quadObj, innerRadius, 50, 50);
    glPopMatrix();

    // Draw Accretion Disk
    glPushMatrix();
    glRotatef(0 + (ringAngle * 0.2), 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, diskTexture1); // Bind disk texture

    for (int i = 0; i < numSegments; ++i) {
        float theta1 = (float)i / numSegments * 2.0f * M_PI;
        float theta2 = (float)(i + 1) / numSegments * 2.0f * M_PI;

        glBegin(GL_QUADS);
        // Inner vertex
        glColor3f(innerColor[0], innerColor[1], innerColor[2]);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(innerRadius * cos(theta1), 0.0f, innerRadius * sin(theta1));
        glTexCoord2f(1.0f, 0.0f); glVertex3f(innerRadius * cos(theta2), 0.0f, innerRadius * sin(theta2));

        // Outer vertex
        float interpColor1[3];
        float interpColor2[3];
        for (int j = 0; j < 3; ++j) {
            interpColor1[j] = innerColor[j] + (outerColor[j] - innerColor[j]) * ((innerRadius + diskThickness) / outerRadius);
            interpColor2[j] = innerColor[j] + (outerColor[j] - innerColor[j]) * ((innerRadius + diskThickness) / outerRadius);
        }
        glColor3f(interpColor1[0], interpColor1[1], interpColor1[2]);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(outerRadius * cos(theta2), diskThickness, outerRadius * sin(theta2));
        glColor3f(interpColor2[0], interpColor2[1], interpColor2[2]);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(outerRadius * cos(theta1), diskThickness, outerRadius * sin(theta1));
        glEnd();
    }
    glPopMatrix();

    // Parameters for the smaller ring
    const float innerRadius2 = 4.4f; // Larger than the outer radius of the first disk
    const float outerRadius2 = 7.0f; // Larger than the inner radius of the second disk
    const float diskThickness2 = 0.2f;

    // Colors for the smaller ring gradient (from inner to outer)
    float innerColor2[3] = {1.0f, 0.2f, 0.2f}; // Brighter red inner color
    float outerColor2[3] = {1.0f, 0.1f, 0.1f}; // Brighter red outer color

    // Draw smaller accretion disk
    glPushMatrix();
    glRotatef(-70 + (ringAngle * 5), 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, diskTexture2); // Bind second disk texture

    for (int i = 0; i < numSegments; ++i) {
        float theta1 = (float)i / numSegments * 2.0f * M_PI;
        float theta2 = (float)(i + 1) / numSegments * 2.0f * M_PI;

        glBegin(GL_QUADS);
        // Inner vertex
        glColor3f(innerColor2[0], innerColor2[1], innerColor2[2]);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(innerRadius2 * cos(theta1), 0.0f, innerRadius2 * sin(theta1));
        glTexCoord2f(1.0f, 0.0f); glVertex3f(innerRadius2 * cos(theta2), 0.0f, innerRadius2 * sin(theta2));

        // Outer vertex
        float interpColor1[3];
        float interpColor2[3];
        for (int j = 0; j < 3; ++j) {
            interpColor1[j] = innerColor2[j] + (outerColor2[j] - innerColor2[j]) * ((innerRadius2 + diskThickness2) / outerRadius2);
            interpColor2[j] = innerColor2[j] + (outerColor2[j] - innerColor2[j]) * ((innerRadius2 + diskThickness2) / outerRadius2);
        }
        glColor3f(interpColor1[0], interpColor1[1], interpColor1[2]);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(outerRadius2 * cos(theta2), diskThickness2, outerRadius2 * sin(theta2));
        glColor3f(interpColor2[0], interpColor2[1], interpColor2[2]);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(outerRadius2 * cos(theta1), diskThickness2, outerRadius2 * sin(theta1));
        glEnd();
    }
    glPopMatrix();

    gluDeleteQuadric(quadObj);
}
/////////// Draw Black Hole ///////////


/////////// Draw Side Wings ///////////
void drawRightAngledTriangularPrism() {

    glBindTexture(GL_TEXTURE_2D, missileTexture);

    glBegin(GL_QUADS);
    // Side face 1
    glTexCoord2f(0.0f, 0.0f); glVertex3f(7.0f, 0.0f, 5.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(7.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(7.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(7.0f, 1.0f, 5.0f);

    // Side face 2
    glTexCoord2f(0.0f, 0.0f); glVertex3f(9.0f, 0.0f, 5.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(7.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(7.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(9.0f, 1.0f, 5.0f);

    // Side face 3
    glTexCoord2f(0.0f, 0.0f); glVertex3f(7.0f, 0.0f, 5.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(9.0f, 0.0f, 5.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(9.0f, 1.0f, 5.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(7.0f, 1.0f, 5.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    // Top face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(7.0f, 1.0f, 5.0f);
    glTexCoord2f(0.5f, 1.0f); glVertex3f(7.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(9.0f, 1.0f, 5.0f);

    // Bottom face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(7.0f, 0.0f, 5.0f);
    glTexCoord2f(0.5f, 1.0f); glVertex3f(7.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(9.0f, 0.0f, 5.0f);
    glEnd();
}
/////////// Draw Side Wings ///////////


/////////// Draw Pyramid ///////////
void drawPyramid() {
    glBindTexture(GL_TEXTURE_2D, missileTexture);

    glBegin(GL_TRIANGLES);
    // Front face
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);

    // Right face
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);

    // Back face
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);

    // Left face
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
    glEnd();

    glBegin(GL_QUADS);
    // Bottom face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glEnd();
}
/////////// Draw Pyramid ///////////


/////////// Draw Cube ///////////
void drawCubeWithTexture(){
    glBegin(GL_QUADS);
    // Set texture coordinates and vertices for the cube
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, -1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, -1.0);

    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0,  1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0,  1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  1.0);

    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0, -1.0,  1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0,  1.0);

    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0,  1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0,  1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  1.0);

    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0,  1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0,  1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0,  1.0);

    glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0,  1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f( 1.0, -1.0,  1.0);
    glEnd();
}
/////////// Draw Cube ///////////


/////////// Draw Spaceship ///////////
void drawSpaceShip() {
       // Apply translation
    glTranslatef(spaceshipPositionX, spaceshipPositionY, spaceshipPositionZ);

    GLUquadricObj *pObj;
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);

    glRotatef(-180, 0, 1, 0);

    // Main Body
    glPushMatrix();
    glTranslatef(0, -2, -14);
    glScalef(6, 1.5, 12);
    glBindTexture(GL_TEXTURE_2D, planeTexture);
    glColor3f(1, 1, 1); // Color white for texture application

    drawCubeWithTexture();
    glPopMatrix();

    // Office Platform
    glPushMatrix();
    glColor3f(1, 1, 1);
    glTranslatef(0, 0.5, -6);
    glScalef(4.0, 1, 4);
    drawCubeWithTexture();
    glPopMatrix();

    // Control Tower
    glPushMatrix();
    glTranslatef(-3.0, 2.0, -12);
    glScalef(0.5, 2.0, 0.5);
    drawCubeWithTexture();

    glPopMatrix();

    // Antennas on Control Tower
    glPushMatrix();
    glColor3f(0.8, 0.8, 0.8);
    glTranslatef(-3.0, 3, -12);
    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();
        glTranslatef(i * 0.3, 0, 0);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(pObj, 0.05f, 0.05f, 3.5f, 26, 13);
        glPopMatrix();
    }
    glPopMatrix();

    // Glass View
    glPushMatrix();
    glTranslatef(0, 2, -5);
    glScalef(1.5, 1.0, 2.5);
    glBindTexture(GL_TEXTURE_2D, missileTexture);
    drawCubeWithTexture();
    glPopMatrix();

    // Side Engines
    glPushMatrix();
    glColor3f(0.4, 0.4, 0.4);
    for (int i = -1; i <= 1; i += 2) {
        glPushMatrix();
        glTranslatef(i * 3.0, -2, -2);
        gluCylinder(pObj, 0.5f, 0.5f, 4.0f, 26, 13);
        glTranslatef(0, 0, 4);
        gluDisk(pObj, 0, 0.5, 26, 13);
        glPopMatrix();
    }
    glPopMatrix();

    // Main Rear Engine
    glPushMatrix();
    glColor3f(0.3, 0.3, 0.3);
    glTranslatef(0, -2, -2);
    gluCylinder(pObj, 1.0f, 1.0f, 6.0f, 26, 13);
    glTranslatef(0, 0, 6);
    gluDisk(pObj, 0, 1.0, 26, 13);
    glPopMatrix();

    // Side Wings
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, planeTexture);
    glColor3f(0.2, 0.2, 0.1);
    for (int i = -1; i <= 1; i += 2) {
        glPushMatrix();
        glTranslatef(i * 8, -2, -12);
        glScalef(2, 0.7, 10);
        drawCubeWithTexture();
        glPopMatrix();
    }
    glPopMatrix();

    // Turrets
    glPushMatrix();
    glColor3f(0, 0, 0);
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -12; j <= -6; j += 2) {
            glPushMatrix();
            glTranslatef(i * 6.5, 0.5, j * 2);
            glRotatef(60 *i, -i * 6.5, 0.5, j * 2);
            glRotatef(turretAngle, 2.0f, 0.0f, 0.0f); // Rotate turret
            glScalef(0.2, 2.5, 0.2);
            drawCubeWithTexture();
            glPopMatrix();
        }
    }
    glPopMatrix();

    // Front Deck
    glPushMatrix();
    glColor3f(0.1, 0.1, 0.1);
    glTranslatef(0, -0.4, -18);
    glScalef(4.2, 0.1, 7.0);
    glBindTexture(GL_TEXTURE_2D, missileTexture);
    drawCubeWithTexture();
    glPopMatrix();


    // Front Cockpit (Pyramid)
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.9, 0.9, 0.9);
    glTranslatef(0, 30, -2);
    glScalef(6, 4, 1.5);
    drawPyramid();
    glPopMatrix();

    // Right-Angled Triangle
    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(-15.3f, -10.0f, -12.0f); // Adjust position as needed
    glScalef(2, 1, 2);
    drawRightAngledTriangularPrism();
    glTranslatef(0.0f, 19.0f, 0.0f); // Adjust position as needed
    drawRightAngledTriangularPrism();

    glPopMatrix();
gluQuadricNormals(pObj, GLU_SMOOTH);
    gluDeleteQuadric(pObj);
}
/////////// Draw Spaceship ///////////


/////////// Timer ///////////
void timer(int value) {
    if (rotateLeft) {
        turretAngle -= 0.1f;
        if (turretAngle <= -10.0f) {
            rotateLeft = false;
        }
    } else {
        turretAngle += 0.1f;
        if (turretAngle >= 30.0f) {
            rotateLeft = true;
        }
    }

    ringAngle += 2.0f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // Approximately 60 FPS
}
/////////// Timer ///////////



/////////// Satellite Animation ///////////
void updateSatelliteAnimation() {
    satelliteRotationY += 0.1f; // Adjust rotation speed as needed
    if (satelliteRotationY > 360.0f) {
        satelliteRotationY -= 360.0f; // Keep the angle within 0-360 degrees
    }
}
/////////// Satellite Animation ///////////


/////////// Spaceship Animation ///////////
void updateSpaceshipAnimation() {
    spaceshipPositionZ += 0.0005f; // Adjust the speed as needed
    if (spaceshipPositionZ > 5.0f) {
        spaceshipPositionZ = -5.0f; // Reset position if it moves too far
    }
}
/////////// Spaceship Animation ///////////



/////////// Display ///////////
void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers
    glLoadIdentity();   // Load identity matrix

    // Apply global wireframe mode
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Enable solid mode
    }

  // Translate and rotate the scene
    glTranslatef(camX, 0.0, 0.0); // Move the scene along the z-axis
    glRotatef(angleX, 1.0, 0.0, 0.0); // Rotate around the X-axis
    glRotatef(angleY, 0.0, 1.0, 0.0); // Rotate around the Y-axis
    glRotatef(angleZ, 0.0, 0.0, 1.0); // Rotate around the Z-axis

    // Disable fog for the background
    glDisable(GL_FOG);

    // Draw the stars background
    drawStarsBackground();

    // Re-enable fog for the rest of the scene
    glEnable(GL_FOG);

    // Draw Mars
    drawMars();

    // Draw asteroids
    drawAsteroid1();
    drawAsteroid2();

    // Draw the satellite
    drawSatellite();

    // Draw the plane with missiles
    drawPlane();

    // Draw Saturn
    drawSaturn();

    //Draw the spaceship
    glTranslatef(3 , -1.0, -4);
    glScalef(0.1, 0.1, 0.1);
    glRotatef(-70, 0,1,0);
    drawSpaceShip();

    //Draw the blackhole
    glTranslatef(-5 , 10, -55);
    glScalef(0.7, 0.7, 0.7);
    drawBlackHole(planeTexture, marsTexture, marsTexture);

    // Swap buffers
    glutSwapBuffers();
}
/////////// Display ///////////


/////////// Idle ///////////
void idle() {
    updateMarsAnimation();      // Update Mars animation
    updateSaturnAnimation(); // Update satellite animation
    updateAsteroid1Animation(); // Update asteroid animation
    updateAsteroid2Animation(); // Update asteroid animation
    updateSatelliteAnimation(); // Update satellite animation
    updateSpaceshipAnimation(); // Update Spaceship animation

    // Update missile translation
    if (launch1) {
        translateZ -= 0.1f;  // Adjust the speed as needed
        if (translateZ < -50.0f) {  // Reset the missile position after it goes off-screen
            translateZ = 0.0f;
            launch1 = false;  // Stop the launch after it completes
            first = 0;
            second = 0;
        }
    }

    glutPostRedisplay();        // Request redisplay
}
/////////// Idle ///////////


/////////// Reshape ///////////
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}
/////////// Reshape ///////////


/////////// Mouse ///////////
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastMouseX = x;
        lastMouseY = y;
        mouseDown = true;
    } else if (button == GLUT_LEFT_BUTTON and state == GLUT_UP) {
        mouseDown = false;
    }
}
/////////// Mouse ///////////


/////////// Motion ///////////
void motion(int x, int y) {
    // Define limits for viewing angles
    const float maxAngleX = 10.00f;   // Maximum angle looking up or down
    const float minAngleX = -10.00f;  // Minimum angle looking up or down
    const float maxAngleY = 30.00f;   // Maximum angle looking left or right
    const float minAngleY = -35.00f;  // Minimum angle looking left or right

    if (mouseDown) {
        // Calculate change in mouse position
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        // Update last mouse position
        lastMouseX = x;
        lastMouseY = y;

        // Adjust angles based on mouse movement
        angleY += deltaX * 0.1;  // Horizontal rotation

        // Vertical rotation with limits
        angleX += deltaY * 0.1;

        if (angleX > maxAngleX) angleX = maxAngleX;
        if (angleX < minAngleX) angleX = minAngleX;

        // Horizontal rotation with limits
        if (angleY > maxAngleY) angleY = maxAngleY;
        if (angleY < minAngleY) angleY = minAngleY;
        glutPostRedisplay();
    }
}
/////////// Motion ///////////


/////////// Keyboard ///////////
void myKeyboard(unsigned char key, int x, int y) {

    switch (key) {

        /////////// SATELLITE
            // Rotation controls
            case 'a': case 'A': // Rotate
                satelliteRotationX -= 5.0f;
                break;
            // Reset transformations
            case 'd': case 'D':
                satelliteRotationX = satelliteRotationY = satelliteRotationZ = 0.0f;
                break;


            // Translation controls
            case 'i': case 'I': // Translate up
                satelliteTranslationY += 0.1f;
                break;
            case 'k': case 'K': // Translate down
                satelliteTranslationY -= 0.1f;
                break;

            // Scaling controls
            case '+': case '=': // Scale up
                satelliteScaleX += 0.1f;
                satelliteScaleY += 0.1f;
                satelliteScaleZ += 0.1f;
                break;
            case '-':           // Scale down
                satelliteScaleX -= 0.1f;
                satelliteScaleY -= 0.1f;
                satelliteScaleZ -= 0.1f;
                break;

        /////////// SATELLITE


        /////////// WIREFRAME
            // Toggle wireframe view
            case 'w': case 'W':
                wireframeMode = !wireframeMode;
                break;
        /////////// WIREFRAME



        /////////// PLANE
            // To launch first set of missiles
            case 'q': case 'Q':
                if (second == 1)
                    second = 3;
                if (first == 0)
                    first = 1;
                else if (first == 1)
                    first = 3;
                launch1 = true;
                translateZ = 0;
                break;

            // To launch second set of missiles
            case 'e': case 'E':
                if (first == 1)
                    first = 3;
                if (second == 0)
                    second = 1;
                else if (second == 1)
                    second = 3;
                launch1 = true;
                translateZ = 0;
                break;

            // To reload missiles
            case 'r': case 'R':
                first = 0;
                second = 0;
                launch1 = false;
                translateZ = 0;
                break;
        /////////// PLANE
    }

    glutPostRedisplay();    // Mark the window for redrawing

}
/////////// Keyboard ///////////


/////////// Main ///////////
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL 3D Scene");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(myKeyboard);   // Register keyboard callback function
    glutIdleFunc(idle);             // Register idle function for animation
    glutTimerFunc(0, timer, 0);

    std::cout << "----CONTROLS FOR PLANE----\n\n";
    std::cout << "R to reload missiles\n";
    std::cout << "Q to launch first set of missiles\n";
    std::cout << "E to launch second set of missiles\n\n\n";

    std::cout << "---- CONTROLS FOR SATELLITE ----\n\n";
    std::cout << "Rotation:\n";
    std::cout << "  A: Rotate\n";
    std::cout << "  D: Reset Rotation\n\n";
    std::cout << "  I: Translate Up\n";
    std::cout << "  K: Translate Down\n\n";
    std::cout << "  +: Scale Up\n\n";
    std::cout << "  -: Scale Down\n\n";

    std::cout << "---- CONTROL FOR WIREFRAME ----\n\n";
    std::cout << "  W: Wireframe Enable\n\n";

    std::cout << "NOTE: BOTH UPPERCASE AND LOWERCASE LETTERS WORK FOR ALL THE FUNCTIONS\n\n";

    glutMainLoop();
    return 0;
}
/////////// Main ///////////
