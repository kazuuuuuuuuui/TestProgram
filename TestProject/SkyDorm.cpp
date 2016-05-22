#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<assert.h>
#include<vector>
#include<string>
#include<Windows.h>
#include"glm\glm.hpp"
#include"glut.h"

//�A���O��
float angle = 0.0f;

//���_��
unsigned int vertexNum;

//�C���f�b�N�X��
unsigned int indexNum;

//�@����
unsigned int normalNum;

//uv���W��
unsigned int texNum;

std::vector<glm::vec3>m_vertex;
std::vector<unsigned short>m_index;
std::vector<glm::vec3>m_normal;
std::vector<glm::vec2>m_texture;

struct RGB
{
	unsigned char r, g, b;
};

void LoadXFile(const char *_FileName)
{
	FILE *fp = fopen(_FileName, "r");
	char buf[256];

	if (NULL == fp)
	{
		printf("�t�@�C����������܂���\n");
		assert(0);
	}

	//-------------------------------------
	//�ǂݔ�΂�

	while (1)
	{
		fscanf(fp, "%s", buf);

		if (0 == strcmp("Mesh", buf))
		{
			break;
		}
	}
	fscanf(fp, "%s", buf); //{
	
	
	/*************
	���_���ǂݍ���
	**************/
	fscanf(fp, "%u", &vertexNum);
	printf("���_��:%u\n", vertexNum);

	fscanf(fp, "%s", buf); //;�̓ǂݔ�΂�

	/***********
	���_�ǂݍ���
	************/
	for (unsigned int i = 0; i < vertexNum; i++)
	{
		glm::vec3 v;

		fscanf(fp, "%f;%f;%f;", &v.x, &v.y, &v.z);

		m_vertex.push_back(v);

		fscanf(fp, "%s", buf);// ,�̓ǂݔ�΂�	
	}

	
	/*********************
	�C���f�b�N�X���ǂݍ���
	*********************/
	fscanf(fp, "%u", &indexNum);
	printf("�C���f�b�N�X��:%u\n", indexNum);

	fscanf(fp, "%s", buf); //;�̓ǂݔ�΂�

	/********************
	�C���f�b�N�X�ǂݍ���
	*********************/
	for (unsigned int i = 0; i < indexNum; i++)
	{
		unsigned short index[3];

		fscanf(fp, "%s", buf);// 3; �ǂݔ�΂�

		fscanf(fp, "%hu,%hu,%hu", &index[0], &index[1], &index[2]);

		fscanf(fp, "%s", buf);// , �ǂݔ�΂�

		m_index.push_back(index[0]);
		m_index.push_back(index[1]);
		m_index.push_back(index[2]);
	}

	//-------------------------------------
	//�ǂݔ�΂�

	while (1)
	{
		fscanf(fp, "%s", buf);

		if (0 == strcmp("MeshNormals", buf))
		{
			break;
		}
	}
	fscanf(fp, "%s", buf); //{


   /*************
   �@�����ǂݍ���
   **************/
	fscanf(fp, "%u", &normalNum);
	printf("�@����:%u\n", normalNum);

	fscanf(fp, "%s", buf); //;�̓ǂݔ�΂�

	/***********
	�@���ǂݍ���
	************/
	for (unsigned int i = 0; i < normalNum; i++)
	{
		glm::vec3 n;

		fscanf(fp, "%f;%f;%f;", &n.x, &n.y, &n.z);

		fscanf(fp, "%s", buf);// ,�̓ǂݔ�΂�	

		m_normal.push_back(n);
	}

	//-------------------------------------
	//�ǂݔ�΂�

	while (1)
	{
		fscanf(fp, "%s", buf);

		if (0 == strcmp("MeshTextureCoords", buf))
		{
			break;
		}
	}
	fscanf(fp, "%s", buf); //{

	/***********
	 uv���ǂݍ���
	************/
	fscanf(fp, "%u", &texNum);
	printf("uv��:%u\n", texNum);

	fscanf(fp, "%s", buf); //;�̓ǂݔ�΂�

	/***********
	uv�ǂݍ���
	************/
	for (unsigned int i = 0; i < texNum; i++)
	{
		glm::vec2 t;

		fscanf(fp, "%f;%f;", &t.x, &t.y);

		m_texture.push_back(t);

		fscanf(fp, "%s", buf);// ,�̓ǂݔ�΂�	
	}

}

void LoadImage3f(const char *_fileName)
{
	FILE *fp;
	fp = fopen(_fileName, "rb");

	assert(fp != NULL);

	BITMAPFILEHEADER bh;
	fread(&bh, sizeof(BITMAPFILEHEADER), 1, fp);

	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(BITMAPINFOHEADER), 1, fp);

	int imageSize = bih.biWidth * bih.biHeight * sizeof(RGB);

	RGB *pixels = (RGB*)malloc(imageSize);

	pixels = (RGB*)malloc(imageSize);

	fread(pixels, imageSize, 1, fp);

	fclose(fp);

	//�s�N�Z���P�ʂ�R��B���t�]������
	for (int i = 0; i < bih.biWidth * bih.biHeight; i++)
	{
		unsigned char tmp;
		tmp = pixels[i].r;
		pixels[i].r = pixels[i].b;
		pixels[i].b = tmp;
	}

	//�s�N�Z���P�ʂŏ㉺���]
	for (int i = 0; i < bih.biWidth; i++)
	{
		for (int n = 0; n < bih.biHeight / 2; n++)
		{
			RGB temp = pixels[bih.biWidth * n + i];
			pixels[bih.biWidth * n + i] = pixels[bih.biWidth*(bih.biHeight - n - 1) + i];
			pixels[bih.biWidth*(bih.biHeight - n - 1) + i] = temp;
		}
	}

	unsigned int handle;

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,//��������e�N�X�`���̃t�H�[�}�b�g
		bih.biWidth,
		bih.biHeight,
		0,
		GL_RGB,//�s�N�Z���f�[�^�̃t�H�[�}�b�g
		GL_UNSIGNED_BYTE,
		pixels
		);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);

	free(pixels);
}

void timer(int value){
	angle += 0.5f;

	glutPostRedisplay();
	glutTimerFunc(
		1000 / 60,
		timer,
		0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		60,//GLdouble fovy,
		1,//GLdouble aspect,
		0.1,//GLdouble zNear,
		100);//GLdouble zFar
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		0, 0, -3,
		0, 0, -1,
		0, 1, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	auto v = m_vertex.begin();
	glVertexPointer(3, GL_FLOAT, 0, &(*v));

	glEnableClientState(GL_NORMAL_ARRAY);
	auto n = m_normal.begin();
	glNormalPointer(GL_FLOAT,0,&(*n));

	auto t = m_texture.begin();
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, &(*t));


	glTranslatef(0, 0, 0);
	glRotatef(angle, 0, 1, 0);
	glRotated(-90, 1, 0, 0);
	glScalef(10, 10, 10);

	auto i = m_index.begin();
	glDrawElements(GL_TRIANGLES,m_index.size(),GL_UNSIGNED_SHORT,&(*i));

	glFlush();
}

int main(int argc, char *argv[]) 
{
	glutInit(&argc, argv);
	glutInitWindowSize(900, 780);
	glutCreateWindow("�X�J�C�h�[��");

	glutDisplayFunc(display);//void (GLUTCALLBACK *func)(void)

	glutTimerFunc(
		0,
		timer,
		0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	LoadImage3f("sky.bmp");
	LoadXFile("sky.x");
	//LoadXFile("propeller.x");

	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);

	//glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);

	glutMainLoop();

}