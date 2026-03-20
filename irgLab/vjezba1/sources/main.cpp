// Local Headers

// System Headers
#include "Grafika.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <iostream>

std::vector<std::pair<int, int> >klikovi;
int width = 97, height = 97;
bool is_cutting = false;

void provjeriFragment(Grafika& grafika, int x, int y, glm::vec3 boja) {
	if (!is_cutting || (x>=width/4 && x<=3*width/4 && y>=height/4 && y<=3*height/4)) {
		grafika.osvijetliFragment(x, y, boja);
	}
}

void iscrtajLiniju(Grafika &grafika, int x0, int y0, int x1, int y1, glm::vec3 line_color) {
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
				provjeriFragment(grafika, y, x, line_color);
				diff += 1/a;
			}
			else {
				provjeriFragment(grafika, x, y, line_color);
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
				provjeriFragment(grafika, y, x, line_color);
				diff += 1/a;
			}
			else {
				provjeriFragment(grafika, x, y, line_color);
				diff += a;
			}
			if (diff<=0) {
				diff += 1;
				y -= 1;
			}
		}
	}
}

void klikMisa(int x, int y, int vrsta) {

	if (vrsta == 0) {
		std::cout << "tocka: ";
		klikovi.push_back(std::make_pair(x, y));
		std::cout << x << " " << y<< std::endl;
	}
	if (vrsta == 1) {
		is_cutting = !is_cutting;
		std::cout << "okvir: " << is_cutting << std::endl;
	}
}

int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	Grafika grafika(width, height, glm::vec3(0, 0, 0), argv[0]);

	//prosljeduje se pokazivac na funkciju. Ta funkcija ce se pozvati kada korisnik pritisne misem
	grafika.registrirajFunkcijuZaKlikMisa(klikMisa);

	while (grafika.trebaZatvoriti()) {

		//osvjezavanje pozicija razlicitih objekata i fizikalna simulacija bi se izvrsavala u ovom dijelu

		grafika.pobrisiProzor();

		//iscrtavanje uzorka sahovnice
		for (int i = 0; i < height; i += 1)
			for (int j = 0; j < width; j += 1) {
				if ((i + j) % 2 == 0)
					grafika.osvijetliFragment(i, j, glm::vec3(0.1, 0.1, 0.1));
				if (i % 10 == 0 && j % 10 == 0)
					grafika.osvijetliFragment(i, j, glm::vec3(0.15, 0.15, 0.2));
			}

		// iscrtavanje okvira
		if (is_cutting) {
			glm::vec3 line_color(0, 1, 0);
			iscrtajLiniju(grafika, width/4, height/4, 3*width/4, height/4, line_color);
			iscrtajLiniju(grafika, width/4, 3*height/4, 3*width/4, 3*height/4, line_color);
			iscrtajLiniju(grafika, width/4, height/4, width/4, 3*height/4, line_color);
			iscrtajLiniju(grafika, 3*width/4, height/4, 3*width/4, 3*height/4, line_color);
		}

		//iscrtavanje linija
		//ishodiste koordinatnog sustava za operacijski sustav je u gornjem lijevom kutu, a za OpenGL je u donjem lijevom, pa je potrebno okrenuti predznak
		for (int i = 0; i < klikovi.size(); i++) {
			if (i % 2 == 1) {
				iscrtajLiniju(
					grafika,
					klikovi[i-1].first,
					height - klikovi[i-1].second - 1,
					klikovi[i].first,
					height - klikovi[i].second - 1,
					glm::vec3(0.2, 0.2, 0.9)
				);
			}
		}
		if (klikovi.size()%2==1) {
			std::pair<int,int> last = klikovi.back();
			grafika.osvijetliFragment(last.first, height - last.second -1, glm::vec3(0.6, 0.2, 0));
		}

		grafika.iscrtajRaster();

		//kako program ne bi trosio previse resursa, na kraj petlje se moze dodati poziv funkcije za cekanje
		//kao npr Sleep ili od c++11 na dalje this_thread::sleep_for(chrono::milliseconds(16));
	}   
	

    return EXIT_SUCCESS;
}
