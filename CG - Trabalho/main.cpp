#include <stdlib.h>
#include <cstdio>
#include <GL/freeglut.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma comment(lib, "winmm.lib")


int id;
float tempo = 0.0;

GLfloat TransladaPassarosV = 0.0;
GLfloat TransladaPato = 0.0;
GLfloat TransladaPassaroBranco = 0.0;
GLfloat TransladaArara = 0.0;

GLuint texturaPilastra;
GLuint texturaGrama;
GLuint texturaLivro1;
GLuint texturaLivro2;
GLuint texturaPele;
GLuint texturaRoupa;
GLuint texturaCabelo;
GLuint texturaFita;
GLuint texturaAgua;
GLuint texturaPato;
GLuint texturaPena;
GLuint texturaFundo;
GLuint texturaFundoLadrilho;

/*
GLuint CarregaTexturaPNG(const char* nomeArquivo) {
    GLuint texID;
    int width, height, nChannels;

    unsigned char* data = stbi_load(nomeArquivo, &width, &height, &nChannels, 4); // força 4 canais RGBA
    if (!data) return 0;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texID;
}*/

GLuint CarregaTextura(const char* nomeArquivo) {
    GLuint texID;
    int width, height;
    unsigned char* data;
    FILE* file = fopen(nomeArquivo, "rb");

    if (!file) return 0;

    fseek(file, 18, SEEK_SET);
    fread(&width, 4, 1, file);
    fread(&height, 4, 1, file);

    fseek(file, 54, SEEK_SET);
    data = (unsigned char*)malloc(width * height * 3);
    fread(data, width * height * 3, 1, file);
    fclose(file);

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(data);
    return texID;
}


// Funcao para carregar a textura do fundo (eh diferente pois eh png eh n mbp)
void carregarTexturaFundo(const char* caminho) {
    int largura, altura, canais;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* imagem = stbi_load(caminho, &largura, &altura, &canais, 0);

    if (!imagem) {
        printf("Erro ao carregar a imagem: %s\n", caminho);
        exit(1);
    }

    glGenTextures(1, &texturaFundo);
    glBindTexture(GL_TEXTURE_2D, texturaFundo);

    GLenum formato = (canais == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, formato, largura, altura, 0, formato, GL_UNSIGNED_BYTE, imagem);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(imagem);
}

void carregarTexturaFundoLadrilho(const char* caminho) {
    int largura, altura, canais;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* imagem = stbi_load(caminho, &largura, &altura, &canais, 0);  // NÃO force canais

    if (!imagem) {
        printf("Erro ao carregar a imagem: %s\n", caminho);
        exit(1);
    }

    glGenTextures(1, &texturaFundoLadrilho);
    glBindTexture(GL_TEXTURE_2D, texturaFundoLadrilho);

    GLenum formato = (canais == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, formato, largura, altura, 0, formato, GL_UNSIGNED_BYTE, imagem);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(imagem);
}

// Desenha um plano de fundo com a textura
void desenhaFundo() {
	// Salva estado de projecao
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);  // plano 2D normalizado

    // Salva estado de modelo
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST); // Desativa z-buffer para fundo
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaFundo);

    glColor3f(1, 1, 1);  

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(1, 0); glVertex2f(1, 0);
        glTexCoord2f(1, 1); glVertex2f(1, 1);
        glTexCoord2f(0, 1); glVertex2f(0, 1);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST); // Restaura z-buffer

    // Restaura projecao
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void desenharChaoLadrilhado() {
	glPushMatrix();

	float inicioX = -3.15;
	float inicioY = -3;
	float fimX = 3.0;
	float fimY = -0.6;
	float tamanho = 0.29; // Tamanho do ladrilho

	for (float y = inicioY; y < fimY; y += tamanho) {
		for (float x = inicioX; x < fimX; x += tamanho) {
			int linha = (int)((y - inicioY) / tamanho);
			int coluna = (int)((x - inicioX) / tamanho);
			int corAlternada = (linha + coluna) % 2;

			if (corAlternada == 0)
				glColor3f(0.85, 0.85, 0.85);  // Cor clara
			else
				glColor3f(0.65, 0.65, 0.65);  // Cor escura

			glBegin(GL_QUADS);
				glVertex3f(x, y, 0);
				glVertex3f(x + tamanho, y, 0);
				glVertex3f(x + tamanho, y + tamanho, 0);
				glVertex3f(x, y + tamanho, 0);
			glEnd();
		}
	}

	glPopMatrix();
}

/*
void desenharChaoLadrilhado() {
	glPushMatrix();

	float inicioX = -3.15;
	float inicioY = -3;
	float fimX = 3.0;
	float fimY = -0.6;
	float tamanho = 0.29; // Tamanho do ladrilho

	// Rotacao para inclinar os ladrilhos
	glRotatef(2, 0.0, 0.0, 1);  // Rotaciona em torno do eixo X (30 graus)

	for (float y = inicioY; y < fimY; y += tamanho) {
		for (float x = inicioX; x < fimX; x += tamanho) {
			int linha = (int)((y - inicioY) / tamanho);
			int coluna = (int)((x - inicioX) / tamanho);
			int corAlternada = (linha + coluna) % 2;

			if (corAlternada == 0)
				glColor3f(0.85, 0.85, 0.85);  // Cor clara
			else
				glColor3f(0.65, 0.65, 0.65);  // Cor escura

			glBegin(GL_QUADS);
				glVertex3f(x, y, 0);
				glVertex3f(x + tamanho, y, 0);
				glVertex3f(x + tamanho, y + tamanho, 0);
				glVertex3f(x, y + tamanho, 0);
			glEnd();
		}
	}

	glPopMatrix();
}
*/

void desenhaReflexoFundoSobreLadrilho(float transparencia) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);  // coordenadas normalizadas

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaFundoLadrilho);

    glColor4f(1.0, 1.0, 1.0, transparencia);  // Transparencia leve

    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex2f(0.0, 0.0);       // canto inferior esquerdo
        glTexCoord2f(1.0, 0.0); glVertex2f(1.0, 0.0);       // canto inferior direito
        glTexCoord2f(1.0, 0.4); glVertex2f(1.0, 0.4);       // superior direito da faixa
        glTexCoord2f(0.0, 0.4); glVertex2f(0.0, 0.4);       // superior esquerdo da faixa
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void desenhaFundoSobreLadrilho() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);  // coordenadas normalizadas

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaFundo);

    glColor4f(1.0, 1.0, 1.0, 1);  // Transparencia leve

    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex2f(0.0, 0.0);       // canto inferior esquerdo
        glTexCoord2f(1.0, 0.0); glVertex2f(1.0, 0.0);       // canto inferior direito
        glTexCoord2f(1.0, 0.4); glVertex2f(1.0, 0.4);       // superior direito da faixa
        glTexCoord2f(0.0, 0.4); glVertex2f(0.0, 0.4);       // superior esquerdo da faixa
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void desenharPilastra(){
	// Corpo da pilastra
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPilastra);

	glBegin(GL_QUADS);
		glColor3f(1,1,1);
		glTexCoord2f(0.19, 0.0); glVertex3f(-0.20, -0.40, 0);
		glTexCoord2f(0.19, 1.0); glVertex3f(-0.20, 0.30, 0);
		glTexCoord2f(0.85, 1.0); glVertex3f(0.20, 0.30, 0);
		glTexCoord2f(0.85, 0.0); glVertex3f(0.20, -0.40, 0);
		
		// Topo da pilastra
		glColor3f(1,1,1);
		glTexCoord2f(0.19, 0.0); glVertex3f(-0.30, 0.20, 0);
		glTexCoord2f(0.19, 1.0); glVertex3f(-0.30, 0.30, 0);
		glTexCoord2f(0.85, 1.0); glVertex3f(0.30, 0.30, 0);
		glTexCoord2f(0.85, 0.0); glVertex3f(0.30, 0.20, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void desenharGrama(){
	// Base do terreno
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaGrama);

	glBegin(GL_QUADS);
	glColor3f(0.8,0.8,0.8);
		glTexCoord2f(0.0, 0.0); glVertex3f(-0.315, -0.35, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-0.315, -0.45, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.315, -0.45, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.315, -0.35, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Hastes da grama
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glColor3f(0,0.3,0);

	float amplitude = 0.015;
	float velocidade = 2.5;

	// Lado esquerdo
	for (float x = -0.30; x <= -0.2; x += 0.02) {
		float yBase = -0.35;
		float altura = 0.08;
		float xTopo = x + amplitude * sin(tempo * velocidade + x * 20);
		float yTopo = yBase + altura;

		glVertex2f(x, yBase);  // base fixa
		glVertex2f(xTopo, yTopo);  // topo se move
    }

	// Lado direito
	for (float x = 0.2; x <= 0.32; x += 0.02) {
		float yBase = -0.35;
		float altura = 0.08;
		float xTopo = x + amplitude * sin(tempo * velocidade + x * 20);
		float yTopo = yBase + altura;

		glVertex2f(x, yBase);  // base fixa
		glVertex2f(xTopo, yTopo);  // topo se move
	}

	glEnd();

	glPopMatrix();
}

void desenharLivro(){
	// Livro
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaLivro1);

	glBegin(GL_QUADS);
		glColor3f(0.96,1,0.72);
		glTexCoord2f(0.07, 0.0); glVertex3f(-0.70, 0.30, 0);
		glTexCoord2f(0.07, 0.97); glVertex3f(-0.65, 0.43, 0);
		glTexCoord2f(1.0, 0.97); glVertex3f(0, 0.43, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.05, 0.30, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaLivro2);

	glBegin(GL_QUADS);
		glColor3f(0.96,1,0.72);
		glTexCoord2f(0.07, 0.0); glVertex3f(0.05, 0.30, 0);
		glTexCoord2f(0.07, 0.97); glVertex3f(0, 0.43, 0);
		glTexCoord2f(1.0, 0.97); glVertex3f(0.65, 0.43, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.70, 0.30, 0);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Fita do livro
	glLineWidth (5.0);
	glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3f(-0.14, 0.384, 0);
		glVertex3f(-0.14, 0.10, 0);
	glEnd();

	glPopMatrix();
}

void desenharBebe(){
	// Corpo do bebe
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaRoupa);

	glBegin(GL_QUADS);
		glColor3f(1,1,1);
		glTexCoord2f(0.09, 0.01); glVertex3f(-0.30, 0.43, 0);
		glTexCoord2f(0.09, 1.0); glVertex3f(-0.30, 0.70, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.30, 0.70, 0);
		glTexCoord2f(1.0, 0.01); glVertex3f(0.30, 0.43, 0);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPele);

	glBegin(GL_QUADS);

		// Braco do bebe
		glColor3f(0.85,0.60,0.60);
		glTexCoord2f(0.0, 0.01); glVertex3f(-0.40, 0.43, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-0.40, 0.52, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.15, 0.52, 0);
		glTexCoord2f(1.0, 0.01); glVertex3f(-0.15, 0.43, 0);

		glColor3f(0.85,0.60,0.60);
		glTexCoord2f(0.0, 0.0); glVertex3f(-0.25, 0.52, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-0.25, 0.58, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.15, 0.58, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.15, 0.52, 0);

		// Perna do bebe
		glColor3f(0.85,0.60,0.60);
		glTexCoord2f(0.0, 0.01); glVertex3f(0.15, 0.43, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(0.15, 0.52, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.40, 0.52, 0);
		glTexCoord2f(1.0, 0.01); glVertex3f(0.40, 0.43, 0);

		glColor3f(0.85,0.60,0.60);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.15, 0.52, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(0.15, 0.58, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.25, 0.58, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.25, 0.52, 0);

		// Cabeca do bebe
		glColor3f(0.85,0.60,0.60);
		glTexCoord2f(0.0, 0.01); glVertex3f(-0.47, 0.57, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-0.47, 0.72, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.27, 0.72, 0);
		glTexCoord2f(1.0, 0.01); glVertex3f(-0.27, 0.57, 0);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaCabelo);

	glBegin(GL_QUADS);

		// Cabelo do bebe
		glColor3f(1,1,1);
		glTexCoord2f(0.0, 0.01); glVertex3f(-0.47, 0.72, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-0.47, 0.81, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.27, 0.81, 0);
		glTexCoord2f(1.0, 0.01); glVertex3f(-0.27, 0.72, 0);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaFita);

	glBegin(GL_QUADS);

		// Fita do bebe
		glColor3f(1,1,1);
		glTexCoord2f(0.1, 0.0); glVertex3f(-0.47, 0.735, 0);
		glTexCoord2f(0.1, 1.0); glVertex3f(-0.47, 0.77, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.27, 0.77, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.27, 0.735, 0);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void desenharPassarosEmV(){
	// Passaros em V
	glPushMatrix();

	glTranslatef(TransladaPassarosV, 0, 0);

	glPointSize (4.0);

	glBegin(GL_POINTS);
		glColor3f(0,0,0);
		glVertex3f(-0.90, 0.84, 0);
		glVertex3f(-0.87, 0.86, 0);
		glVertex3f(-0.84, 0.88, 0);
		glVertex3f(-0.81, 0.9, 0);
		glVertex3f(-0.78, 0.92, 0);
		glVertex3f(-0.75, 0.90, 0);
		glVertex3f(-0.72, 0.88, 0);
		glVertex3f(-0.69, 0.86, 0);
		glVertex3f(-0.66, 0.84, 0);
	glEnd();

	glPopMatrix();
}

void desenharLadrilho(){
	// Agua
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaAgua);

	glBegin(GL_QUADS);
		glColor3f(1,1,1);
		glTexCoord2f(0.11, 0.0); glVertex3f(-0.026, -1.09, 0);
		glTexCoord2f(0.11, 1.0); glVertex3f(-0.026, -0.95, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.55, -0.95, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.55, -1.09, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaAgua);

	glBegin(GL_TRIANGLES);
		glColor3f(1,1,1);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.55, -1.09, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(0.55, -0.95, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(0.60, -1.09, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Ladrilho
	glPushMatrix();
	glLineWidth (1.0);
	glBegin(GL_LINES);
		glColor3f(0,0,0);
		glVertex3f(-0.026, -0.92, 0);
		glVertex3f(-0.026, -1.09, 0);

		glVertex3f(0.55, -0.92, 0);
		glVertex3f(0.55, -0.95, 0);

		glVertex3f(-0.026, -0.92, 0);
		glVertex3f(0.55, -0.92, 0);

		glVertex3f(-0.026, -0.95, 0);
		glVertex3f(0.55, -0.95, 0);

		glVertex3f(0.55, -0.92, 0);
		glVertex3f(0.62, -1.09, 0);

		glVertex3f(0.55, -0.95, 0);
		glVertex3f(0.60, -1.09, 0);
	glEnd();

	glPopMatrix();
}

void desenharPato(){
	// Corpo do pato
	glPushMatrix();

	glTranslatef(0, TransladaPato, 0);
	//glTranslatef(0.0f, 0.3f, 0.0f);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPato);

	glBegin(GL_QUADS);
		glColor3f(1,1,0);
		glTexCoord2f(0.13, 0.0); glVertex3f(0.25, -0.99, 0);
		glTexCoord2f(0.13, 0.99); glVertex3f(0.25, -0.90, 0);
		glTexCoord2f(1.0, 0.99); glVertex3f(0.45, -0.90, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.45, -0.99, 0);
	// Cabeca do pato
		glColor3f(1,1,0);
		glTexCoord2f(0.13, 0.01); glVertex3f(0.25, -0.90, 0);
		glTexCoord2f(0.13, 0.99); glVertex3f(0.25, -0.81, 0);
		glTexCoord2f(1.0, 0.99); glVertex3f(0.33, -0.81, 0);
		glTexCoord2f(1.0, 0.01); glVertex3f(0.33, -0.90, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Olhos do pato
	glPointSize (3.0);

	glBegin(GL_POINTS);
		glColor3f(0,0,0);
		glVertex3f(0.27, -0.84, 0);
		glVertex3f(0.31, -0.84, 0);
	glEnd();

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPena);

	// Bico do pato
	glBegin(GL_TRIANGLES);
		glColor3f(1,0.2,0);
		glTexCoord2f(0.0, 0.5); glVertex3f(0.20, -0.875, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.30, -0.85, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.30, -0.90, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Rabo do pato
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPato);

	glBegin(GL_TRIANGLES);
		glColor3f(1,1,0);
		glTexCoord2f(0.2, 0.4); glVertex3f(0.37, -0.90, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.46, -0.85, 0);
		glTexCoord2f(0.8, 0.0); glVertex3f(0.45, -0.93, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void desenharPassaroBranco(){
	glPushMatrix();

	glTranslatef(TransladaPassaroBranco, 0, 0);

	float anguloPassaroBranco = 145 * sin(tempo); // angulo de oscilacao da asa

	// Asa superior (tras)
	glPushMatrix();
	
	// Fixamos a base da asa na posicao do corpo (em relacao ao passaro)
	glTranslatef(-0.76, 0, 0);
	glRotatef(anguloPassaroBranco, 1, 0, 0);  // A rotacao acontece em torno do eixo X
	glTranslatef(0.76, 0, 0);  // Retorna ao ponto de fixacao da asa

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPena);

	glBegin(GL_QUADS);
		glColor3f(0.76,0.72,0.72);
		glTexCoord2f(0.0, 0.0); glVertex3f(-0.76, 0, 0);
		glTexCoord2f(0.0, 0.7); glVertex3f(-0.75, 0.1, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.67, 0.18, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.67, 0, 0);
	glEnd();

	glPopMatrix();

	// Corpo do passaro branco
	glBegin(GL_QUADS);
		glColor3f(0.9,0.85,0.85);
		glTexCoord2f(0.11, 0.0); glVertex3f(-0.90, -0.05, 0);
		glTexCoord2f(0.11, 1.0); glVertex3f(-0.90, 0, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.65, 0, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.65, -0.05, 0);

	// Cabeca do passaro branco
		glColor3f(1,0,0);
		glTexCoord2f(0.0, 0.0); glVertex3f(-0.65, -0.05, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-0.65, 0, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.60, 0, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.60, -0.05, 0);
	glEnd();

	// Asa inferior (frente)
	glPushMatrix();
	glTranslatef(-0.83, 0, 0);
	glRotatef(-anguloPassaroBranco, 1, 0, 0);  // Rotacao invertida para a asa inferior
	glTranslatef(0.83, 0, 0);
	glBegin(GL_QUADS);
		glColor3f(0.9,0.85,0.85);
		glTexCoord2f(0.0, 0.0); glVertex3f(-0.83, 0, 0);
		glTexCoord2f(0.0, 0.7); glVertex3f(-0.82, 0.10, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.74, 0.18, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.74, 0, 0);
	glEnd();

	glPopMatrix();

	glLineWidth (3.5);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPena);

	glBegin(GL_LINES);
		glColor3f(1,0,0);
		glTexCoord2f(0.0, 0.0); glVertex3f(-0.71, -0.045, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.64, -0.045, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Olho do passaro branco
	glPointSize (3.0);

	glBegin(GL_POINTS);
		glColor3f(0,0,0);
		glVertex3f(-0.624, -0.015, 0);
	glEnd();

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPena);

	// Bico do passaro branco
	glBegin(GL_TRIANGLES);
		glColor3f(0.9,0.85,0.85);
		glTexCoord2f(0.05, 0.0); glVertex3f(-0.606, -0.04, 0);
		glTexCoord2f(0.05, 1.0); glVertex3f(-0.606, -0.01, 0);
		glTexCoord2f(1.0, 0.5); glVertex3f(-0.564, -0.025, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Rabo do passaro branco
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPena);

	glBegin(GL_TRIANGLES);
		glColor3f(0.9,0.85,0.85);
		glTexCoord2f(0.0, 1.0); glVertex3f(-0.95, 0.05, 0);
		glTexCoord2f(1.0, 0.3); glVertex3f(-0.84, 0, 0);
		glTexCoord2f(0.5, 0.0); glVertex3f(-0.90, -0.02, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Pernas do passaro branco
	glLineWidth (1.5);

	glBegin(GL_LINES);
		glColor3f(0,0,0);
		glVertex3f(-0.90, -0.05, 0);
		glVertex3f(-0.92, -0.10, 0);

		glVertex3f(-0.88, -0.05, 0);
	glVertex3f(-0.90, -0.10, 0);
	glEnd();

	glPopMatrix();
}

void desenharArara(){
	glPushMatrix();

	glTranslatef(TransladaArara, 0, 0);

	// Animacaodas asas
	float anguloArara = 145 * sin(tempo);

	// Asa superior (azul)
	glPushMatrix();
	glTranslatef(0.87, 0.805, 0);
	glRotatef(-anguloArara, 1, 0, 0);
	glTranslatef(-0.87, -0.805, 0);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPena);

	glBegin(GL_QUADS);
		glColor3f(0.4,0.4,1);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.77, 0.805, 0);
		glTexCoord2f(0.2, 1.0); glVertex3f(0.79, 0.90, 0);
		glTexCoord2f(1.0, 0.4); glVertex3f(0.84, 0.83, 0);
		glTexCoord2f(0.8, 0.0); glVertex3f(0.83, 0.805, 0);
	glEnd();

	glPopMatrix();

	// Corpo da arara
	glBegin(GL_QUADS);
		glColor3f(1,0,0);
		glTexCoord2f(0.1, 0.0); glVertex3f(0.80, 0.77, 0);
		glTexCoord2f(0.1, 1.0); glVertex3f(0.80, 0.805, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.90, 0.805, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.90, 0.77, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPena);

	// Cabeca da arara
	glBegin(GL_QUADS);
		glColor3f(1,1,1);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.90, 0.77, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(0.90, 0.805, 0);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.94, 0.805, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.94, 0.77, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Bico da arara
	glBegin(GL_TRIANGLES);
	glColor3f(0,0,0);
		glVertex3f(0.92, 0.77, 0);
		glVertex3f(0.94, 0.77, 0);
		glVertex3f(0.922, 0.75, 0);
	glEnd();

	// Rabo da arara
	glLineWidth (4.5);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPena);

	glBegin(GL_LINES);
		glColor3f(0.4,0.4,1);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.75, 0.798, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.802, 0.798, 0);

		glColor3f(1,0,0);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.60, 0.798, 0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.75, 0.798, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Olho da arara
	glPointSize (3.0);

	glBegin(GL_POINTS);
		glColor3f(0,0,0);
		glVertex3f(0.925, 0.79, 0);
	glEnd();

	// Asa inferior (vermelha)
	glPushMatrix();
	glTranslatef(0.88, 0.805, 0);
	glRotatef(anguloArara, 1, 0, 0);
	glTranslatef(-0.88, -0.805, 0);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPena);

	glBegin(GL_QUADS);
		glColor3f(1,0,0);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.80, 0.805, 0);
		glTexCoord2f(0.2, 1.0); glVertex3f(0.82, 0.92, 0);
		glTexCoord2f(1.0, 0.4); glVertex3f(0.87, 0.85, 0);
		glTexCoord2f(0.8, 0.0); glVertex3f(0.86, 0.805, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	glPopMatrix();
}

void desenha(void){
	glClearColor(0.4,1,0.4,0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	glLoadIdentity();
	
	desenhaFundo();
	
	glTranslatef(0.0f, 0.0f, -0.5f); // frente do fundo

	desenharChaoLadrilhado();
	
	//desenhaReflexoFundoSobreLadrilho(0.6);
	
	desenhaFundoSobreLadrilho();

	desenharPilastra();

	desenharGrama();

	desenharLivro();

	desenharBebe();

	desenharPassarosEmV();

	desenharLadrilho();

	desenharPato();

	desenharPassaroBranco();

	desenharArara();

	glFlush();

	glutSwapBuffers();
}

void animacao(int value) {
	// Movimento continuo da esquerda para a direita
	TransladaPassarosV += 0.001;
	TransladaPassaroBranco += 0.004;
	TransladaArara += 0.003;
	tempo += 0.02;  // Atualiza o tempo

	// Passaros em V
	if (TransladaPassarosV > 2.7) TransladaPassarosV = -1.3;

	// Passaro branco
	if (TransladaPassaroBranco > 2.7) TransladaPassaroBranco = -1.5;

	// Arara
	if (TransladaArara > 1.46) TransladaArara = -2.7;

	// Pato
	TransladaPato = sin(tempo) * 0.02;

	glutPostRedisplay();
	glutTimerFunc(16, animacao, 0);
}

void reshape(int w, int h) {
	if (h == 0) h = 1; // Evita divisao por zero
	float aspect = (float)w / (float)h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Mantem a proporcao correta
	if (w >= h) {
		// Janela mais larga que alta
		gluOrtho2D(-aspect, aspect, -1.0, 1.0);
	} else {
		// Janela mais alta que larga
		gluOrtho2D(-1.0, 1.0, -1.0/aspect, 1.0/aspect);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Teclado (unsigned char key, int x, int y){
	if (key == 27){
		glutDestroyWindow(id);
		exit(0);
	}
}

void carregarTexturtas(){
	texturaPilastra = CarregaTextura("pilastra.bmp");
	texturaGrama = CarregaTextura("grama.bmp");
	texturaLivro1 = CarregaTextura("livro1.bmp");
	texturaLivro2 = CarregaTextura("livro2.bmp");
	texturaPele = CarregaTextura("pele.bmp");
	texturaRoupa = CarregaTextura("roupa.bmp");
	texturaCabelo = CarregaTextura("cabelo.bmp");
	texturaFita = CarregaTextura("fita.bmp");
	texturaAgua = CarregaTextura("agua.bmp");
	texturaPato = CarregaTextura("pato.bmp");
	texturaPena = CarregaTextura("pena.bmp");
	carregarTexturaFundo("fundo.png");
	carregarTexturaFundoLadrilho("ladrilho_agua_.png");
}

// Programa principal
int main(int argc,char ** argv){
	//Audio
	PlaySound(TEXT("som.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	
	//Inicia a telinha
	glutInit(&argc,argv);

	// Define do modo de operacao da GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Especifica o tamanho inicial em pixels da janela GLUT
	glutInitWindowSize(500,500);

	// Especifica a posicao inicial da janela
	glutInitWindowPosition(100,100);

	// Cria a janela 
	id = glutCreateWindow("Bebe dormindo");
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Carrega as texturas
	carregarTexturtas();

	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

	// Registra a funcao de redesenho da janela de visualizacao
	glutDisplayFunc(desenha);

	// Redimensionar a tela
	glutReshapeFunc(reshape);

	// Inicia a animacao
	glutTimerFunc(25, animacao, 0);  

	// Registra a funcao para tratamento das teclas ASCII
	glutKeyboardFunc(Teclado);

	glutMainLoop();

	return 0;
}
