#define CLK 2
  #define DIO 3

  #include <GyverTM1637.h>
  GyverTM1637 disp(CLK, DIO);

  enum state  {Setup1, ZeroTime, Kaboom, Idle};
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
    analogWrite(255, 0); // реле
  }


  void loop() {
    // Set Value
    bool buttonPlus = false;
    bool buttonMinus = false;
    bool buttonStart = false;
    bool buttonStop = false;
    bool timeEvent = false;
    bool pushButton = false;
    bool trigger = false;

    bool DEBUG = true;

    if (digitalRead(4) == 0 || digitalRead(5) == 0 ||digitalRead(6) == 0
|| digitalRead(7) == 0   ) {
      delay(200);//считывает состояние каждой кнопки каждые 200мс
                 //создает событие при нажатии на кнопку
      if (digitalRead(6) == 0) buttonPlus = true,  pushButton = true;
      if (digitalRead(7) == 0) buttonMinus = true, pushButton = true;
      if (digitalRead(4) == 0) buttonStart = true, pushButton = true;
      if (digitalRead(5) == 0) buttonStop = true,  pushButton = true;
      if (digitalRead(8) == 0) trigger = true,     pushButton = true;//Serial.println("trigger"); // проверить старт схему и в по

    }// создает событие при нажатии на кнопку

    if (millis() - myTimer >= 1000) {
      myTimer = millis();
      timeEvent = true;
    }

    if (current_state == Setup1) { // кнопками задаем время
      if (buttonPlus)                                          time_on ++;
      else if (buttonMinus)                                    time_on --;
      else if (buttonStart)         current_state = Idle, tic = 0;
      if (time_on < 1)                                      time_on = 1; // нужно, чтобы не задавать время с минусом
      if (timeEvent || pushButton)  disp.displayInt(time_on), Serial.println(time_on);         // Вывод на экран состояния
    }
    if (current_state == Idle) {
      Serial.println("IDLE");
      Serial.println(trigger);
      if( trigger == true){
        current_state == ZeroTime;
        tic = 0;
      }
      
    }
    if (current_state == ZeroTime){
     Serial.println("ZEROTIME");// вывести для всех стате состояние тригера
     Serial.println(trigger);
      if (trigger == false){
        current_state == Idle;
      }
      if (time_on > tic && timeEvent){
        
        disp.displayInt(time_on - tic), Serial.println(time_on - tic); // добавить минус на дисплей для понимания состояния
        tic++;
      }
      else{
        current_state == Kaboom;
      }
    }

    if (current_state == Kaboom){
      Serial.println("KABOOM");
      Serial.println(trigger);
      if (tac > boom && timeEvent){
        disp.displayInt(8888), Serial.println("boom");
        analogWrite(0, 0); // реле
        tac++;
      }
      else{
        analogWrite(255,0);
      }     
    }

  }