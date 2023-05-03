#define CLK 2
#define DIO 3

#include <GyverTM1637.h>
GyverTM1637 disp(CLK, DIO);

enum state  {Setup1, Timer_game, Deploy, End};
int current_state = Setup1;
int time_on  = 1;
int time_off = 1;
uint32_t tic = 0;
uint32_t tac = 0;
uint32_t boom = 9; //время запала
uint32_t myTimer = 0;
uint32_t timerStop = 0;



void setup() {
  Serial.begin(9600); 
  disp.clear();        // чистим дисплей 
  disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)
  pinMode(4, INPUT_PULLUP); // старт
  pinMode(5, INPUT_PULLUP); // стоп
  pinMode(6, INPUT_PULLUP); // верх
  pinMode(7, INPUT_PULLUP); // вниз
  pinMode(8, INPUT_PULLUP); // триггер 
  digitalWrite(9, 0); // реле
}


void loop() {
  // Set Value
  bool buttonPlus = false;
  bool buttonMinus = false;
  bool buttonStart = false;
  bool buttonStop = false;
  bool timeEvent = false;
  bool pushButton = false;
  //bool relaypin = false; 
  
  bool DEBUG = true;
  
  if (digitalRead(4) == 0 || digitalRead(5) == 0 ||digitalRead(6) == 0 || digitalRead(7) == 0   ) {
    delay(200);//считывает состояние каждой кнопки каждые 200мс
               //создает событие при нажатии на кнопку
    if (digitalRead(6) == 0) buttonPlus = true,  pushButton = true;
    if (digitalRead(7) == 0) buttonMinus = true, pushButton = true;
    if (digitalRead(4) == 0) buttonStart = true, pushButton = true;
    if (digitalRead(5) == 0) buttonStop = true,  pushButton = true;

  }// создает событие при нажатии на кнопку

  if (millis() - myTimer >= 1000) {
    myTimer = millis();
    timeEvent = true;
  }

  if (current_state == Setup1) { // кнопками задаем время
    if (buttonPlus)          			time_on ++;
    else if (buttonMinus) 				time_on --;
  	else if (buttonStart) 				current_state = Timer_game, tic = 0;
		//else if (buttonStop)					continue;
    
    if (time_on < 1)				      time_on = 1; // нужно, чтобы не задавать время с минусом 
    if (timeEvent || pushButton)  disp.displayInt(time_on), Serial.println(time_on);	//printlsd(String("Setup 1 "), String("time: ") + String(time_on)); // Вывод на экран состояния
    
  }

  else if (current_state == Timer_game) {
    if (tic <= time_on){
      if (timeEvent) {
        disp.displayInt(time_on - tic), Serial.println(time_on - tic), Serial.println(tic); //printlsd(String("Game"), String("time: ") + String(time_on - tic)); // Вывод на экран состояния 
      	
        tic++;
      }
    } else {
      current_state = Deploy; 
      tic = 0, tac = 0;
    }
  }     
     
  	else if (current_state == Deploy) {
    if (tac < boom){
      if (timeEvent) {
        disp.displayInt(8888);
        digitalWrite(9, 0); 
      	tac++;
      }
    } else {
    	current_state = End; 
      digitalWrite(9, 1);
      tic = 0, tac = 0;
    }
  }
  
  	else if (current_state == End) {
      if (timeEvent) {
    Serial.println("end");
    }
  }
}










