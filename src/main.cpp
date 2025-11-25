
#include <Adafruit_GFX.h>

#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include "snake_logic.h"

#define TFT_BL 2


class LGFX : public lgfx::LGFX_Device
{
public:

  lgfx::Bus_RGB     _bus_instance;
  lgfx::Panel_RGB   _panel_instance;

  LGFX(void)
  {


    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;
      
      cfg.pin_d0  = GPIO_NUM_8; // B0
      cfg.pin_d1  = GPIO_NUM_3;  // B1
      cfg.pin_d2  = GPIO_NUM_46;  // B2
      cfg.pin_d3  = GPIO_NUM_9;  // B3
      cfg.pin_d4  = GPIO_NUM_1;  // B4
      
      cfg.pin_d5  = GPIO_NUM_5;  // G0
      cfg.pin_d6  = GPIO_NUM_6; // G1
      cfg.pin_d7  = GPIO_NUM_7;  // G2
      cfg.pin_d8  = GPIO_NUM_15;  // G3
      cfg.pin_d9  = GPIO_NUM_16; // G4
      cfg.pin_d10 = GPIO_NUM_4;  // G5
      
      cfg.pin_d11 = GPIO_NUM_45; // R0
      cfg.pin_d12 = GPIO_NUM_48; // R1
      cfg.pin_d13 = GPIO_NUM_47; // R2
      cfg.pin_d14 = GPIO_NUM_21; // R3
      cfg.pin_d15 = GPIO_NUM_14; // R4

      cfg.pin_henable = GPIO_NUM_40;
      cfg.pin_vsync   = GPIO_NUM_41;
      cfg.pin_hsync   = GPIO_NUM_39;
      cfg.pin_pclk    = GPIO_NUM_0;
      cfg.freq_write  = 15000000;

      cfg.hsync_polarity    = 0;
      cfg.hsync_front_porch = 8;
      cfg.hsync_pulse_width = 4;
      cfg.hsync_back_porch  = 43;
      
      cfg.vsync_polarity    = 0;
      cfg.vsync_front_porch = 8;
      cfg.vsync_pulse_width = 4;
      cfg.vsync_back_porch  = 12;

      cfg.pclk_active_neg   = 1;
      cfg.de_idle_high      = 0;
      cfg.pclk_idle_high    = 0;

      _bus_instance.config(cfg);
    }
            {
      auto cfg = _panel_instance.config();
      cfg.memory_width  = 800;
      cfg.memory_height = 480;
      cfg.panel_width  = 800;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);
    setPanel(&_panel_instance);

  }
};


LGFX lcd;

//UI

int led;
SPIClass& spi = SPI;


/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;

int gridSize = 40;
int highScore = 0;
int currentScore = 0;

void drawSnake(Point snakeLoc[], int snakeLength) {
  for (int i = 0; i < snakeLength; i++) {
    int x = snakeLoc[i].x;
    int y = snakeLoc[i].y;

    int px = x * gridSize;
    int py = y * gridSize;

    lcd.fillRect(px, py, gridSize, gridSize, TFT_SKYBLUE);
  }
}

void drawFruit(Point fruitLoc[]) {
    int gx = fruitLoc[0].x;   // grid X
    int gy = fruitLoc[0].y;   // grid Y

    int px = gx * gridSize;
    int py = gy * gridSize;

    int cx = px + gridSize / 2;
    int cy = py + gridSize / 2;

    int radius = gridSize * 0.3;
    lcd.fillCircle(cx, cy, radius, TFT_RED);

    int stemLen = gridSize * 0.19;
    lcd.drawLine(cx, cy - radius, cx, cy - radius - stemLen, TFT_BROWN);
}

void drawBoard(){
  lcd.fillScreen(TFT_GREENYELLOW);
  for(int i = 1; i <= 24; i++){
    for(int j = 1; j <= 12; j++){
      if (i%2 == 0){
        if(j%2 != 0){
          lcd.fillRect(gridSize*i - gridSize, gridSize*j-gridSize, gridSize, gridSize, TFT_GREEN);
        }
      }
      else{
        if(j%2 == 0){
          lcd.fillRect(gridSize*i - gridSize, gridSize*j-gridSize, gridSize, gridSize, TFT_GREEN);
        }
      }
    }
  }
}

// fake snake and fruit for start screen, wrapped around start button
Point fakeSnake[14] = {
  {12,8}, {13,8}, {14,8}, {16,8}, {17,8}, {18,8},
  {18,7}, {18,6}, {18,5}, {17,5}, {16,5}, {15,5}, {15, 8}, {14, 5}
};
Point fakeFruit[1] = {
  {11,8}
};

void drawStartScreen() {
  drawBoard();
  int buttonWidth = 400;
  int buttonHeight = 40;
  int buttonX = (screenWidth - buttonWidth) / 2;
  int buttonY = (screenHeight - buttonHeight) / 2 + 20;
  lcd.fillRect(buttonX, buttonY, buttonWidth, buttonHeight, TFT_SKYBLUE);
  lcd.setCursor(buttonX + 10, buttonY + 10);
  lcd.setTextColor(TFT_WHITE);
  lcd.setTextSize(3);
  lcd.print("say 'START' to begin");
  // draw fake snake around button
  drawSnake(fakeSnake, 14);
  drawFruit(fakeFruit);
  // draw eye on the snake head by fruit with white circle and black dot
  int eyeX = fakeSnake[0].x * gridSize + gridSize / 2;
  int eyeY = fakeSnake[0].y * gridSize + gridSize / 2 - 8;
  lcd.fillCircle(eyeX-3, eyeY, 6, TFT_WHITE);
  lcd.fillCircle(eyeX-3, eyeY, 3, TFT_BLACK);
  // draw high score in top left
  lcd.setCursor(10, 10);
  lcd.setTextColor(TFT_SKYBLUE);
  lcd.setTextSize(3);
  lcd.print("High Score: ");
  lcd.print(highScore);
  // write name of game "SPEECH SNAKE" at top center
  lcd.setCursor(screenWidth / 2 - 160, 60);
  lcd.setTextColor(TFT_SKYBLUE);
  lcd.setTextSize(5);
  lcd.print("SPEECH SNAKE");
}


void drawCurrScore(int score){
    lcd.setCursor(10, 10);
    lcd.setTextColor(TFT_BLACK); // Black text with green
    lcd.setTextSize(2);
    lcd.print("Score: ");
    lcd.print(score);
}

void setup(){
  
  Serial.begin(115200);
  Wire.begin(19, 20);
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  
  // Init Display
  lcd.begin();
  lcd.fillScreen(TFT_BLACK);

  screenWidth = lcd.width();
  screenHeight = lcd.height();

#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif

}

unsigned long prev = 0;
String command = "";
bool startScreenDrawn = false;
void loop()
{
  if(Serial.available() > 0){
    command = Serial.readStringUntil('\n');
    command.trim();
    prev-=200; // speed up response time after command received to compensate for delay
  }
  if (!snakeAlive){
    if (!startScreenDrawn){
      startScreenDrawn = true;
      drawStartScreen();
    }
    if (command == "start"){
      startScreenDrawn = false;
      snakeAlive = true;
      currentScore = 0;
      initGame();
      drawBoard();
      drawSnake(snake, snakeLength);
      drawFruit(&fruit);
      command = "";
    }
  }
  else{
    if (millis() - prev > 1000) {
      prev = millis();
      updateDirection(command.c_str());
      updateSnake();
      if (snakeAlive){
        drawBoard();
        drawSnake(snake, snakeLength);
        drawFruit(&fruit);
        highScore = max(highScore, snakeLength - 3);
        currentScore = snakeLength - 3;
        drawCurrScore(currentScore);
      }
    }
  }
}