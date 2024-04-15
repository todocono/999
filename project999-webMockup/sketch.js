
/***********
 * // Project 999
// Mockup of web

// Original idea from TT
// Implementation by RC & MC
// Coded Apr 06 2024 by RC
**************/


let fontSize = 128;

let displayedText = "";

let textIndex = 0;


function setup() {
  let cnv = createCanvas(windowWidth, windowHeight);
  cnv.touchStarted(changeText);
  textAlign(CENTER, CENTER);
}

function draw() {
  background(200, 200, 200);

  // A rectangle
  fill(0);
  noStroke();
  rect(20, 20, windowWidth - 40, windowHeight - 40);
  // uses global variables for width and height
  // let posX = (w - displayedText.length() * fontSize) / 2;
  // let posY = h / 2 + fontSize / 4;

  textSize(fontSize);
  fill('#D6E32C');
  text(displayedText, windowWidth / 2, windowHeight / 2);
  // // Top line.
  // textSize(16);
  // textAlign(RIGHT);
  // text('ABCD', 50, 30);
}

// assigns new values for width and height variables

function windowResized() {
  w = windowWidth;// window.innerWidth;
  h = windowHeight;//window.innerHeight;
  resizeCanvas(w, h);
}

// window.onresize = function () {
//   // assigns new values for width and height variables
//   w = window.innerWidth;
//   h = window.innerHeight;
//   canvas.size(w, h);
// }


function keyPressed() {
  if (key == '1') {
    fontSize = 30;
    displayedText = "请问您要喝冰的还是热的";
  } else if (key == '2') {
    fontSize = 60;
    displayedText = "热的还是冰的";
  } else if (key == '3') {
    fontSize = 90;
    displayedText = "要冰块吗";
  } else if (key == 'c') {
    changeText();
  } else {
    displayedText = "";
  }
}

function changeText() {
  if (textIndex++ > 3) {
    textIndex = 0;
  } else

    if (textIndex == 1) {
      fontSize = 60;
      displayedText = "请问您要喝冰的还是热的";
    } else if (textIndex == 2) {
      fontSize = 90;
      displayedText = "热的还是冰的";
    } else if (textIndex == 3) {
      fontSize = 180;
      displayedText = "要冰块吗";
    } else if (textIndex == 4) {
      fontSize = 35x0;
      displayedText = "要冰块吗?";
    }
    else {
      displayedText = "";
    }
}
