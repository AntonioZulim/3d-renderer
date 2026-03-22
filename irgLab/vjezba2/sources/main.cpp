// Local Headers

// System Headers
#include "Grafika.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "glm/ext.hpp"

#include <random>

glm::vec3 randomColor() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	return glm::vec3(dis(gen), dis(gen), dis(gen));
}

struct Poly {
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> edges;
	bool isLeftOriented;
	int minX, maxX, minY, maxY;
};

std::vector<std::pair<int, int> >klikovi;
std::vector<std::tuple<int, int, glm::vec3> >testVerts;
Poly polygon;
bool isDrawing = true;

void iscrtajLiniju(Grafika& grafika, int x0, int y0, int x1, int y1, glm::vec3 lineColor) {
	// ako je kut <90, 180] ili [-180, -90> zamijeni tocke
	if (x0>x1) {
		int t = x0;
		x0 = x1;
		x1 = t;
		t = y0;
		y0 = y1;
		y1 = t;
	}
	float a = (y1-y0)/(float)(x1-x0);
	// ako je kut [0, 90]
	if (y0<=y1) {
		// ako je kut <45, 90] (tj. tan kuta > 1)
		if (a>1) {
			int t = x0;
			x0 = y0;
			y0 = t;
			t = x1;
			x1 = y1;
			y1 = t;
		}
		float diff = -0.5;
		int y = y0;
		for (int x = x0; x<=x1; x++) {
			if (a>1) {
				grafika.osvijetliFragment(y, x, lineColor);
				diff += 1/a;
			}
			else {
				grafika.osvijetliFragment(x, y, lineColor);
				diff += a;
			}
			if (diff>=0) {
				diff -= 1;
				y += 1;
			}
		}
	}
	// ako je kut [-90, 0>
	else {
		// ako je kut [-90, -45> (tj. tan kuta < -1)
		if (a<-1) {
			int t = x0;
			x0 = y1;
			y1 = t;
			t = x1;
			x1 = y0;
			y0 = t;
		}
		float diff = 0.5;
		int y = y0;
		for (int x = x0; x<=x1; x++) {
			if (a<-1) {
				grafika.osvijetliFragment(y, x, lineColor);
				diff += 1/a;
			}
			else {
				grafika.osvijetliFragment(x, y, lineColor);
				diff += a;
			}
			if (diff<=0) {
				diff += 1;
				y -= 1;
			}
		}
	}
}

void addVertex(int x, int y) {
	glm::vec3 t2(x, y, 1);
	size_t n = polygon.verts.size();
	if (n>0) {
		glm::vec3 t1 = polygon.verts.back();
		glm::vec3 edge = glm::cross(t1, t2);

		if (n>1) {
			float dist = glm::dot(t2, polygon.edges[polygon.edges.size()-1]);
			if (n>2) {
				glm::vec3 connecting_edge = glm::cross(t2, polygon.verts.front());
				float dist1 = glm::dot(polygon.verts[1], connecting_edge);
				float dist2 = glm::dot(polygon.verts.back(), connecting_edge);
				if ((polygon.isLeftOriented != dist>0) ||
					(polygon.isLeftOriented != dist1>0) ||
					(polygon.isLeftOriented != dist2>0)) {
					std::cout << "Odabrana tocka tvori konkavan poligon!" << std::endl;
					return;
				}
			}
			else {
				polygon.isLeftOriented = dist>0;
			}
			if (dist==0) {
				polygon.edges.pop_back();
				polygon.verts.pop_back();
			}
		}
		polygon.minX = std::min(polygon.minX, x);
		polygon.maxX = std::max(polygon.maxX, x);
		polygon.minY = std::min(polygon.minY, y);
		polygon.maxY = std::max(polygon.maxY, y);
		polygon.edges.push_back(edge);
	}
	else {
		polygon.minX = x;
		polygon.maxX = x;
		polygon.minY = y;
		polygon.maxY = y;
	}
	polygon.verts.push_back(t2);
}

void fillPolygon(Grafika& grafika, glm::vec3 color, int height) {
	for (int y = polygon.minY+1; y<=polygon.maxY; y++) {
		glm::vec3 scanLine(0, 1, -y);
		std::vector<glm::vec3> v;
		for (int i = 0; i<polygon.edges.size(); i++) {
			glm::vec3 t = glm::cross(polygon.edges[i], scanLine);
			t /= t.z;
			if (t.z!=0 && t.x>=polygon.minX && t.x<=polygon.maxX) {
				v.push_back(t);
				if (v.size()==2) {
					break;
				}
			}
		}
		iscrtajLiniju(grafika, v[0].x, height - v[0].y - 1, v[1].x, height - v[1].y - 1, color);
	}
}

void testVertex(int x, int y) {
	glm::vec3 color(0, 1, 0);
	for (int i = 0; i<polygon.edges.size(); i++) {
		float dist = glm::dot(glm::vec3(x, y, 1), polygon.edges[i]);
		if (polygon.isLeftOriented!=dist>=0) {
			color = glm::vec3(1, 0, 0);
			break;
		}
	}
	testVerts.push_back(std::tuple(x, y, color));
}

void klikMisa(int x, int y, int vrsta) {
	if (vrsta == 0 && isDrawing) {
		std::cout << "tocka: ";
		klikovi.push_back(std::make_pair(x, y));
		std::cout << x << " " << y<< std::endl;
		addVertex(x, y);
	}
	if (vrsta == 1) {
		if (polygon.verts.size()>2) {
			polygon.edges.push_back(glm::cross(polygon.verts.back(), polygon.verts.front()));
			if (!isDrawing) {
				testVertex(x, y);
			}
			isDrawing = false;
		}
		else {
			std::cout << "Trebaju biti zadana barem 3 vrha" << std::endl;
		}
	}
}

int main(int argc, char * argv[]) {
	int width = 97, height = 97;
	std::cout << argv[0] << std::endl;
	Grafika grafika(width, height, glm::vec3(0, 0, 0), argv[0]);

	glm::vec3 primaryColor(0.2, 0.2, 0.9);
	glm::vec3 secondaryColor(0.6, 0.2, 0);

	//prosljeduje se pokazivac na funkciju. Ta funkcija ce se pozvati kada korisnik pritisne misem
	grafika.registrirajFunkcijuZaKlikMisa(klikMisa);

	while (grafika.trebaZatvoriti()) {

		//osvjezavanje pozicije razlicitih objekata i fizikalna simulacija bi se izvrsavala u ovom dijelu

		grafika.pobrisiProzor();

		//iscrtavanje uzorka sahovnice
		for (int i = 0; i < height; i += 1)
			for (int j = 0; j < width; j += 1) {
				if ((i + j) % 2 == 0)
					grafika.osvijetliFragment(i, j, glm::vec3(0.1, 0.1, 0.1));
				if (i % 10 == 0 && j % 10 == 0)
					grafika.osvijetliFragment(i, j, glm::vec3(0.15, 0.15, 0.2));
			}


		//iscrtavanje pritisnutih fragmenata
		//ishodiste koordinatnog sustava za operacijski sustav je u gornjem lijevom kutu, a za OpenGL je u donjem lijevom, pa je potrebno okrenuti predznak
		size_t n = polygon.verts.size();
		// linija koja bi zatvorila poligon
		if (n>2) {
			glm::vec3 color = isDrawing ? secondaryColor : primaryColor;
			iscrtajLiniju(
				grafika,
				polygon.verts.front().x,
				height - polygon.verts.front().y - 1,
				polygon.verts.back().x,
				height - polygon.verts.back().y - 1,
				color
			);
		}
		// pocetna tocka
		else if (n==1) {
			grafika.osvijetliFragment(polygon.verts.front().x, height - polygon.verts.front().y - 1, secondaryColor);
		}
		// nacrtani poligon
		for (int i = 1; i < n; i++) {
			iscrtajLiniju(
				grafika,
				polygon.verts[i-1].x,
				height - polygon.verts[i-1].y - 1,
				polygon.verts[i].x,
				height - polygon.verts[i].y - 1,
				primaryColor
			);
		}
		if (!isDrawing) {
			fillPolygon(grafika, primaryColor, height);
		}

		// testne tocke
		for (int i = 0; i<testVerts.size(); i++) {
			grafika.osvijetliFragment(
				std::get<0>(testVerts[i]),
				height - std::get<1>(testVerts[i]) - 1,
				std::get<2>(testVerts[i])
			);
		}

		grafika.iscrtajRaster();

		//kako program ne bi trosio previse resursa, na kraj petlje se moze dodati poziv funkcije za cekanje
		//kao npr Sleep ili od c++11 na dalje this_thread::sleep_for(chrono::milliseconds(16));
	}   
	

    return EXIT_SUCCESS;
}
