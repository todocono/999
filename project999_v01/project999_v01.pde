int fontSize = 128;
int indexMax = 100;


String displayedText = "";
Table table;
int keyIndex = -1;



void setup() {
  PFont cnFont= createFont("MingLiU", 18);
  textFont(cnFont);
  //String[] fontList = PFont.list();
  //printArray(fontList);
  table = loadTable("sample100.csv", "header");

  println(table.getRowCount() + " total rows in table");

  for (TableRow row : table.rows()) {

    int id = row.getInt("id");
    String phrase = row.getString("phrase");

    println(phrase + "  has an ID of " + id);
  }


  textSize(fontSize);
  fill(#D6E32C);
  //background(0);

  //size(1000, 400); //to be commented when fullScreen is active

  fullScreen();
}


void draw() {

  int posX = (width - displayedText.length() * fontSize)/2;
  int posY = height/2 + fontSize/4;

  background(0);
  text( displayedText, posX, posY);
}


void readPhrase(int ind) {
  if (ind > -1 && ind < indexMax) {
    if (ind == 0) {
      displayedText = "";
    } else {
      TableRow row = table.getRow(ind);
      displayedText =row.getString("phrase");
    }
  }
}


void keyPressed() {

  if (key == CODED) {
    if (keyCode == UP) {
      println("UP");
    }
  } else {
    if (key == ENTER) {
      if (keyIndex > -1) {
        readPhrase(keyIndex);
      }
      println("Displaying phrase #" + keyIndex);
      keyIndex = -1;
    } else if (key == BACKSPACE) {
      readPhrase(0);
      keyIndex = -1;
      println("BACKSPACE");
    } else  if (key >= '0' && key <= '9') {
      if (keyIndex == -1) {
        keyIndex = key - '0';
      } else {
        keyIndex = keyIndex * 10 + key - '0';
      }
    }
  }
}
