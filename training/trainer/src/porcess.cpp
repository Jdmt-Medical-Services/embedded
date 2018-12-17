#include <Arduino.h>
#include <sample0001.c>
// Read Image into Array (http://www.digole.com/tools/PicturetoC_Hex_converter.php) ...OK
// Process Grayscale into black and white binary Image ...OK
// Find bounding box of display
// Process image into binary hash for neuronal Network

// Use above method to creat train data
// Use resulting matrix from neuronal network to detect state based on image

const int height = 120;
const int width = 160;
int threshold = 35;
// These array could be reduced to one (move image function)
bool binaryImage [width][height];
bool bbImage [width][height];
int bbX1;
int bbY1;
int bbX2;
int bbY2;

void printImage() {
  for (int y=0; y<height; y+=6) {
    for (int x=0; x<width; x+=4) {
      if (binaryImage[x][y]) {
          Serial.print("o");
      } else {
          Serial.print(" ");
      }
    }
    Serial.println();
  }
}

bool checkMono(int count, int compare) {
  if (count == 0 || count == compare) {
    return true;
  }
  return false;
}

void findBorder(int &bb1, int &bb2, int dim1, int dim2, bool horizontal) {
  bb1 = 0;
  bb2 = 0;
  int c = dim1/2;
  for (int x=0; x<c; x++) {
    int count1 = 0;
    int count2 = 0;
    for (int y=0; y<dim2; y++) {
      if ((horizontal && binaryImage[c-x-1][y]) || (!horizontal  && binaryImage[y][c-x-1])) {
        count1++;
      }
      if ((horizontal && binaryImage[c+x][y]) || (!horizontal  && binaryImage[y][c+x])) {
        count2++;
      }
    }
    if (bb1 == 0 && checkMono(count1, dim2)) {
      bb1 = c-x-1;
    }
    if (bb2 == 0 && checkMono(count2, dim2)) {
      bb2 = c+x;
    }
  }
}

void superSimpleBoundingBox() {
  //from center to left and right
  findBorder(bbX1, bbX2, width, height, true);
  findBorder(bbY1, bbY2, height, width, false);

  // Iterate with new Matrix until bbX and bbY are stabel
  Serial.print("Left: ");
  Serial.println(bbX1);
  Serial.print("Right: ");
  Serial.println(bbX2);
  Serial.print("Top: ");
  Serial.println(bbY1);
  Serial.print("Bottom: ");
  Serial.println(bbY2);

  // Move in Matrix does not work
  for (int x=bbX1; x<bbX2; x++) {
    for (int y=bbY1; y<bbY2; y++) {
      binaryImage[x-bbX1][y-bbY1] = binaryImage[x][y];
    }
  }
  printImage();
  //from center to top
  //from center to bottem
}



void setup() {

  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }
  int l = sizeof(sample0001_map);
  int x = 0;
  int y = 0;
  for (int i=0; i<l; i++) {
    if(sample0001_map[i] > threshold) {
      binaryImage[x][y] = 1;
    } else {
      binaryImage[x][y] = 0;
    }
    x++;
    if (x == width) {
      y++;
      x=0;
    }
  }
  printImage();
  superSimpleBoundingBox();
}

void loop() {

}