#include <list>
#include <iostream>
using namespace std;

#include "CImg-1.5.7/CImg.h"
using namespace cimg_library;

#include <string>
#include "lib/ColorHist.h"
#include "lib/Color.h"

/*
  pixelArtMaker "file" nb_color
  
*/

struct point {
  Color color;
  unsigned int cluster;
};


inline Color getPixel(unsigned int const x, unsigned int const y, CImg<unsigned char> const& image) {
  return Color(image(x, y, 0), image(x, y, 1), image(x, y, 2));
}

int main(int argc, char* argv[]) {

  char * file;
  unsigned int K;

  list<point> pointList;
  Color kmean[K];
  ColorHist ch;
  
  point p;

  // TODO
  /*  if (argc < 3) {
      file = "lena.jpg";
      K = 16;
      } else {*/
  file = argv[1];
  K = atoi(argv[2]);
  //  }

  cout << endl;
  cout << "============================" << endl;
  cout << "Bienvenue dans PixelArtMaker" << endl;

  /* Charger l'image */

  CImg<unsigned char> image(file);
  CImg<unsigned char> visu(image.width(), image.height(), 1, 3, 0);

  /* Remplir l'histogramme et la liste des couleurs. */
  for (int i = 0 ; i < image.height() ; i++) {
    for (int j = 0 ; j < image.width() ; j++) {
      p = {getPixel(i, j, image), 0}; //WARNING -std=c++11
      if (!ch.addColor(p.color))
	pointList.push_front(p);
    }
  }

  Color c;

  
  CImgDisplay orig_disp(image, "Original"), visu_disp(visu, "Test");

  /*  
  //TEST
  int x = 0, y = 0;

  for (list<point>::iterator iterator = pointList.begin() ; 
  iterator != pointList.end() ; 
  iterator++) {
  c = (*iterator).color;
  for (int j = 0; j < ch.getColor(c); j++) {
  visu(x, y, 0) = c.getR();
  visu(x, y, 1) = c.getG();
  visu(x, y, 2) = c.getB();
      
  if(x++ > image.width()) {
  x = 0;
  y++;
  }
  // cout << c << "\t" << ch.getColor(c) << endl;
  }
  }

  visu.display(visu_disp);
  while(!orig_disp.is_closed() && !visu_disp.is_closed()) {
  orig_disp.wait();
  }

  */

  ////////////////// K-Mean ////////////////////////


  // Initialiser les clusters avec des couleurs aléatoires.
  list<point>::iterator iterator = pointList.begin();
  bool enoughColors = true;

  for(int i = 0 ; i < K ; i++) {
    if (iterator = pointList.end()) {
      enoughColors = false;
      break;
    }
    kmean[i] = (*iterator).color;
    iterator++;
  }
  
  if (enoughColors) {
    // Algorithme des k-moyennes //////////////////////////////
    
    bool unchanged = false;
    unsigned int nearest;
    double nearestDist;
    double dist;
    int clusterWeight[K];
    Triplet nextKMean[K];

    while(!unchanged) {
      
      for (int i = 0 ; i < K ; i++ ) {
	nextKMean[i] = Color(0,0,0);
      }
    
      // Mettre à jour les clusters
      for (list<point>::iterator iterator = pointList.begin() ; 
	   iterator != pointList.end() ; iterator++) {
	c = (*iterator).color;
      
	nearest = 0;
	nearestDist = 4096; // sqrt(256*256*256)
	// Calculer la moyenne la plus proche
	for (int i = 0 ; i < K ; i++ ) {
	  dist = Color::distance(c, kmean[i]);
	  if (dist < nearestDist) {
	    nearestDist = dist;
	    nearest = i;
	  }
	}
      
	(*iterator).cluster = nearest;
      }

      // Mettre à jour les moyennes    

      for (list<point>::iterator iterator = pointList.begin() ; 
	   iterator != pointList.end() ; iterator++) {
	c = (*iterator).color;
	
	kmean[(*iterator).cluster].add(c);
      }
    }
    
  } else {
    cout << "L'image originale contient moins de couleur que le nombre demandé." << endl;
  }
  
  return 0;
}
